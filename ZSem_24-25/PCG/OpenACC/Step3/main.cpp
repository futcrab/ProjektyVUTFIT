/**
 * @file      main.cpp
 *
 * @author    Peter Durica \n
 *            Faculty of Information Technology \n
 *            Brno University of Technology \n
 *            xduric05@fit.vutbr.cz
 *
 * @brief     PCG Assignment 2
 *
 * @version   2023
 *
 * @date      04 October   2023, 09:00 (created) \n
 */

#include <cmath>
#include <cstdio>
#include <chrono>
#include <string>

#include "nbody.h"
#include "h5Helper.h"

/**
 * Main rotine
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  if (argc != 7)
  {
    std::printf("Usage: %s <N> <dt> <steps> <write intesity> <input> <output>\n", argv[0]);
    std::exit(1);
  }

  // Number of particles
  const unsigned N         = static_cast<unsigned>(std::stoul(argv[1]));
  // Length of time step
  const float    dt        = std::stof(argv[2]);
  // Number of steps
  const unsigned steps     = static_cast<unsigned>(std::stoul(argv[3]));
  // Write frequency
  const unsigned writeFreq = static_cast<unsigned>(std::stoul(argv[4]));

  // Log benchmark setup
  std::printf("       NBODY GPU simulation\n"
              "N:                       %u\n"
              "dt:                      %f\n"
              "steps:                   %u\n",
              N, dt, steps);

  const std::size_t recordsCount = (writeFreq > 0) ? (steps + writeFreq - 1) / writeFreq : 0;

  Particles particles[2]{Particles{N}, Particles{N}};

  /********************************************************************************************************************/
  /*                                     TODO: Fill memory descriptor parameters                                      */
  /********************************************************************************************************************/

  /*
   * Caution! Create only after CPU side allocation
   * parameters:
   *                            Stride of two            Offset of the first
   *       Data pointer       consecutive elements        element in FLOATS,
   *                          in FLOATS, not bytes            not bytes
  */
  MemDesc md(&particles[0].positions[0].x,                 3,                          0, // PosX
             &particles[0].positions[0].y,                 3,                          0, // PosY
             &particles[0].positions[0].z,                 3,                          0, // PosZ
             &particles[0].velocities[0].x,                3,                          0, // VelX
             &particles[0].velocities[0].y,                3,                          0, // VelY
             &particles[0].velocities[0].z,                3,                          0, // VelZ
             particles[0].weights,                         1,                          0, // Weight
             N,
             recordsCount);

  // Initialisation of helper class and loading of input data
  H5Helper h5Helper(argv[5], argv[6], md);

  try
  {
    h5Helper.init();
    h5Helper.readParticleData();
  }
  catch (const std::exception& e)
  {
    std::fprintf(stderr, "Error: %s\n", e.what());
    return EXIT_FAILURE;
  }

  /********************************************************************************************************************/
  /*                   TODO: Allocate memory for center of mass buffer. Remember to clear it.                         */
  /********************************************************************************************************************/
  float4* comBuffer = new float4[256];
  
  // Zero out the array
  std::fill(comBuffer, comBuffer + 256, float4{0.0f, 0.0f, 0.0f, 0.0f});
  
  #pragma acc enter data create(comBuffer[0:256])
  #pragma acc update device(comBuffer[0:256])

  /********************************************************************************************************************/
  /*                                      TODO: Set openacc stream ids                                                */
  /********************************************************************************************************************/
  unsigned calculateStream = 1;
  unsigned transferStream = 2;
  unsigned comStream = 3;
  /********************************************************************************************************************/
  /*                                     TODO: Memory transfer CPU -> GPU                                             */
  /********************************************************************************************************************/
  // Fill second particle struct with the same values since h5Helper has only filled one
  for (unsigned i = 0; i < N; i++){
    particles[1].positions[i] = particles[0].positions[i];
    particles[1].velocities[i] = particles[0].velocities[i];
    particles[1].weights[i] = particles[0].weights[i];
  }
  
  particles[0].copyToDevice();
  particles[1].copyToDevice();


  // Lambda for checking if we should write current step to the file
  auto shouldWrite = [writeFreq](unsigned s) -> bool
  {
    return writeFreq > 0u && (s % writeFreq == 0u);
  };

  // Lamda for getting record number
  auto getRecordNum = [writeFreq](unsigned s) -> unsigned
  {
    return s / writeFreq;
  };
  
  // Start measurement
  const auto start = std::chrono::steady_clock::now();

  /********************************************************************************************************************/
  /*            TODO: Edit the loop to work asynchronously and overlap computation with data transfers.               */
  /*                  Use shouldWrite lambda to determine if data should be outputted to file.                        */
  /*                           if (shouldWrite(s, writeFreq)) { ... }                                                 */
  /*                        Use getRecordNum lambda to get the record number.                                         */
  /********************************************************************************************************************/
  for (unsigned s = 0u; s < steps; ++s)
  {
    const unsigned srcIdx = s % 2;        // source particles index
    const unsigned dstIdx = (s + 1) % 2;  // destination particles index

    /******************************************************************************************************************/
    /*                                        TODO: GPU computation                                                   */
    /******************************************************************************************************************/
    // Calculate velocity on stream 1
    calculateVelocity(particles[srcIdx], particles[dstIdx], N, dt, calculateStream);

    if(shouldWrite(s)){
      unsigned recordNum = getRecordNum(s);

      // Calculate centerOfMass on stream 3
      centerOfMass(particles[srcIdx], comBuffer, N, comStream);

      // Transfer values from gpu on stream 2
      particles[srcIdx].copyToHost(transferStream);

      // if transfer occured on second struct fill also the first one so h5Helper can access it
      if (srcIdx == 1){
        for (unsigned i = 0; i < N; i++){
          particles[0].positions[i] = particles[srcIdx].positions[i];
          particles[0].velocities[i] = particles[srcIdx].velocities[i];
          particles[0].weights[i] = particles[srcIdx].weights[i];
        }
      }

      // Wait for calculation to finnish then write values into file
      #pragma acc wait(comStream, transferStream)
      h5Helper.writeParticleData(recordNum);
      h5Helper.writeCom(comBuffer[0], recordNum); // Result value is stored at first spot in buffer memory after calculation
      // Zero out comBuffer
      std::fill(comBuffer, comBuffer + 256, float4{0.0f, 0.0f, 0.0f, 0.0f});
      // When finished wait for calculation of next step to start
      #pragma acc wait(calculateStream)
    }
  }

  const unsigned resIdx = steps % 2;    // result particles index

  /********************************************************************************************************************/
  /*                          TODO: Invocation of center of mass kernel, do not forget to add                         */
  /*                              additional synchronization and set appropriate stream                               */
  /********************************************************************************************************************/
  #pragma acc wait // wait for all kernels to finish

  centerOfMass(particles[resIdx], comBuffer, N, comStream);

  // End measurement
  const auto end = std::chrono::steady_clock::now();

  // Approximate simulation wall time
  const float elapsedTime = std::chrono::duration<float>(end - start).count();
  std::printf("Time: %f s\n", elapsedTime);

  

  /********************************************************************************************************************/
  /*                                     TODO: Memory transfer GPU -> CPU                                             */
  /********************************************************************************************************************/
  particles[resIdx].copyToHost(transferStream);

  // If the result is stored in second struct transfer it back to first since it can be accessed by h5Helper
  if (resIdx == 1){
    for (unsigned i = 0; i < N; i++){
      particles[0].positions[i] = particles[resIdx].positions[i];
      particles[0].velocities[i] = particles[resIdx].velocities[i];
      particles[0].weights[i] = particles[resIdx].weights[i];
    }
  }

  // Result value is stored at first spot in buffer memory after calculation
  const float4 comFinal = comBuffer[0];


  // Compute reference center of mass on CPU
  const float4 refCenterOfMass = centerOfMassRef(md);

  std::printf("Reference center of mass: %f, %f, %f, %f\n",
              refCenterOfMass.x,
              refCenterOfMass.y,
              refCenterOfMass.z,
              refCenterOfMass.w);

  std::printf("Center of mass on GPU: %f, %f, %f, %f\n",
              comFinal.x,
              comFinal.y,
              comFinal.z,
              comFinal.w);

  // Writing final values to the file
  h5Helper.writeComFinal(comFinal);
  h5Helper.writeParticleDataFinal();

  /********************************************************************************************************************/
  /*                                TODO: Free center of mass buffer memory                                           */
  /********************************************************************************************************************/
  #pragma acc exit data delete(comBuffer[0:256])
  delete [] comBuffer;

}// end of main
//----------------------------------------------------------------------------------------------------------------------
