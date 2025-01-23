/**
 * @file      main.cu
 *
 * @author    Peter Durica \n
 *            Faculty of Information Technology \n
 *            Brno University of Technology \n
 *            xduric05@fit.vutbr.cz
 *
 * @brief     PCG Assignment 1
 *
 * @version   2024
 *
 * @date      04 October   2023, 09:00 (created) \n
 */

#include <cmath>
#include <cstdio>
#include <chrono>
#include <string>

#include "nbody.cuh"
#include "h5Helper.h"

/**
 * @brief CUDA error checking macro
 * @param call CUDA API call
 */
#define CUDA_CALL(call) \
  do { \
    const cudaError_t _error = (call); \
    if (_error != cudaSuccess) \
    { \
      std::fprintf(stderr, "CUDA error (%s:%d): %s\n", __FILE__, __LINE__, cudaGetErrorString(_error)); \
      std::exit(EXIT_FAILURE); \
    } \
  } while(0)

/**
 * Main rotine
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  if (argc != 10)
  {
    std::printf("Usage: nbody <N> <dt> <steps> <threads/block> <write intesity> <reduction threads> <reduction threads/block> <input> <output>\n");
    std::exit(1);
  }

  // Number of particles
  const unsigned N                   = static_cast<unsigned>(std::stoul(argv[1]));
  // Length of time step
  const float    dt                  = std::stof(argv[2]);
  // Number of steps
  const unsigned steps               = static_cast<unsigned>(std::stoul(argv[3]));
  // Number of thread blocks
  const unsigned simBlockDim         = static_cast<unsigned>(std::stoul(argv[4]));
  // Write frequency
  const unsigned writeFreq           = static_cast<unsigned>(std::stoul(argv[5]));
  // number of reduction threads
  const unsigned redTotalThreadCount = static_cast<unsigned>(std::stoul(argv[6]));
  // Number of reduction threads/blocks
  const unsigned redBlockDim         = static_cast<unsigned>(std::stoul(argv[7]));

  // Size of the simulation CUDA grid - number of blocks
  const unsigned simGridDim = (N + simBlockDim - 1) / simBlockDim;
  // Size of the reduction CUDA grid - number of blocks
  const unsigned redGridDim = (redTotalThreadCount + redBlockDim - 1) / redBlockDim;

  // Log benchmark setup
  std::printf("       NBODY GPU simulation\n"
              "N:                       %u\n"
              "dt:                      %f\n"
              "steps:                   %u\n"
              "threads/block:           %u\n"
              "blocks/grid:             %u\n"
              "reduction threads/block: %u\n"
              "reduction blocks/grid:   %u\n",
              N, dt, steps, simBlockDim, simGridDim, redBlockDim, redGridDim);

  const std::size_t recordsCount = (writeFreq > 0) ? (steps + writeFreq - 1) / writeFreq : 0;

  Particles hParticles{};

  /********************************************************************************************************************/
  /*                              TODO: CPU side memory allocation (pinned)                                           */
  /********************************************************************************************************************/
  unsigned ParticleMemSize = 7u; // one particle consists of 7 floats

  float *hostParticlesMemory =(float *) malloc(N * ParticleMemSize * sizeof(float));

  hParticles.pos_X = hostParticlesMemory;
  hParticles.pos_Y = &hostParticlesMemory[N];
  hParticles.pos_Z = &hostParticlesMemory[2 * N];

  hParticles.vel_X = &hostParticlesMemory[3 * N];
  hParticles.vel_Y = &hostParticlesMemory[4 * N];
  hParticles.vel_Z = &hostParticlesMemory[5 * N];

  hParticles.weights = &hostParticlesMemory[6 * N];


  /********************************************************************************************************************/
  /*                              TODO: Fill memory descriptor layout                                                 */
  /********************************************************************************************************************/
  /*
   * Caution! Create only after CPU side allocation
   * parameters:
   *                            Stride of two            Offset of the first
   *       Data pointer       consecutive elements        element in FLOATS,
   *                          in FLOATS, not bytes            not bytes
  */
  MemDesc md(hParticles.pos_X,        1,                          0,
             hParticles.pos_Y,        1,                          0,
             hParticles.pos_Z,        1,                          0,
             hParticles.vel_X,        1,                          0,
             hParticles.vel_Y,        1,                          0,
             hParticles.vel_Z,        1,                          0,
             hParticles.weights,      1,                          0,
             N,
             recordsCount);

  // Initialisation of helper class and loading of input data
  H5Helper h5Helper(argv[8], argv[9], md);

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

  Particles  dParticles{};
  Velocities dTmpVelocities{};

  /********************************************************************************************************************/
  /*                                     TODO: GPU side memory allocation                                             */
  /********************************************************************************************************************/
  float *deviceParticlesMemory, *deviceVelocitiesMemory;
  unsigned VelocityMemSize = 3u; // one velocity struct consist of 3 floats
  
  CUDA_CALL(cudaMalloc(&deviceParticlesMemory, N * ParticleMemSize * sizeof(float)));
  CUDA_CALL(cudaMalloc(&deviceVelocitiesMemory, N * VelocityMemSize * sizeof(float)));

  dParticles.pos_X = deviceParticlesMemory;
  dParticles.pos_Y = &deviceParticlesMemory[N];
  dParticles.pos_Z = &deviceParticlesMemory[2 * N];

  dParticles.vel_X = &deviceParticlesMemory[3 * N];
  dParticles.vel_Y = &deviceParticlesMemory[4 * N];
  dParticles.vel_Z = &deviceParticlesMemory[5 * N];

  dParticles.weights = &deviceParticlesMemory[6 * N];

  dTmpVelocities.dir_X = deviceVelocitiesMemory;
  dTmpVelocities.dir_Y = &deviceVelocitiesMemory[N];
  dTmpVelocities.dir_Z = &deviceVelocitiesMemory[2 * N];
  

  /********************************************************************************************************************/
  /*                                     TODO: Memory transfer CPU -> GPU                                             */
  /********************************************************************************************************************/
  CUDA_CALL(cudaMemcpy(deviceParticlesMemory, hostParticlesMemory, N* ParticleMemSize * sizeof(float), cudaMemcpyHostToDevice));

  
  // Start measurement
  const auto start = std::chrono::steady_clock::now();

  for (unsigned s = 0u; s < steps; ++s)
  {
    /******************************************************************************************************************/
    /*                                     TODO: GPU kernels invocation                                               */
    /******************************************************************************************************************/
      CUDA_CALL(cudaMemset(deviceVelocitiesMemory, 0, N * VelocityMemSize * sizeof(float)));
      calculateGravitationVelocity<<<simGridDim, simBlockDim>>>(dParticles, dTmpVelocities, N, dt);
      calculateCollisionVelocity<<<simGridDim, simBlockDim>>>(dParticles, dTmpVelocities, N, dt);
      updateParticles<<<simGridDim, simBlockDim>>>(dParticles, dTmpVelocities, N, dt);


  }

  // Wait for all CUDA kernels to finish
  CUDA_CALL(cudaDeviceSynchronize());

  // End measurement
  const auto end = std::chrono::steady_clock::now();

  // Approximate simulation wall time
  const float elapsedTime = std::chrono::duration<float>(end - start).count();
  std::printf("Time: %f s\n", elapsedTime);


  /********************************************************************************************************************/
  /*                                     TODO: Memory transfer GPU -> CPU                                             */
  /********************************************************************************************************************/
  CUDA_CALL(cudaMemcpy(hostParticlesMemory, deviceParticlesMemory, N * ParticleMemSize * sizeof(float), cudaMemcpyDeviceToHost));


  // Compute reference center of mass on CPU
  const float4 refCenterOfMass = centerOfMassRef(md);

  std::printf("Reference center of mass: %f, %f, %f, %f\n",
              refCenterOfMass.x,
              refCenterOfMass.y,
              refCenterOfMass.z,
              refCenterOfMass.w);

  std::printf("Center of mass on GPU: %f, %f, %f, %f\n", 0.f, 0.f, 0.f, 0.f);

  // Writing final values to the file
  h5Helper.writeComFinal(refCenterOfMass);
  h5Helper.writeParticleDataFinal();

  /********************************************************************************************************************/
  /*                                     TODO: GPU side memory deallocation                                           */
  /********************************************************************************************************************/
  CUDA_CALL(cudaFree(deviceParticlesMemory));
  CUDA_CALL(cudaFree(deviceVelocitiesMemory));

  /********************************************************************************************************************/
  /*                                     TODO: CPU side memory deallocation                                           */
  /********************************************************************************************************************/
  free(hostParticlesMemory);

}// end of main
//----------------------------------------------------------------------------------------------------------------------
