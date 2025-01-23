/**
 * @file      nbody.h
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

#ifndef NBODY_H
#define NBODY_H

#include "h5Helper.h"
#include "Vec.h"

/**
 * @brief Particles data structure
 */
struct Particles
{
  /// @brief Default constructor not allowed
  Particles() = delete;

  /**
   * @brief Constructor
   * @param N - Number of particles
   */
  Particles(const unsigned N);

  /// @brief Copy constructor not allowed
  Particles(const Particles&) = delete;

  /// @brief Move constructor not allowed
  Particles(Particles&&) = delete;

  /// @brief Destructor
  ~Particles();

  /// @brief Copy assignment operator not allowed
  Particles& operator=(const Particles&) = delete;

  /// @brief Move assignment operator not allowed
  Particles& operator=(Particles&&) = delete;

  /**
   * @brief Copy particles from host to device
   */
  void copyToDevice();

  /**
   * @brief Copy particles from device to host
   * @param streamNum - Number of stream that will be used to transfer data asynchronously
   */
  void copyToHost(unsigned streamNum);  // I added new paramater to specify which stream will be used

  /********************************************************************************************************************/
  /* TODO: Particles data structure optimized for use on GPU. Use float3 and float4 structures defined in file Vec.h  */
  /********************************************************************************************************************/
  unsigned N;

  // Decided to split positions and weights because i wanted to have cleaner code while using overloaded float3 operators
  float3 *positions;
  float3 *velocities;
  float *weights;
  
};

/**
 * Calculate velocity
 * @param pIn  - particles input
 * @param pOut - particles output
 * @param N    - Number of particles
 * @param dt   - Size of the time step
 * @param streamNum - Number of stream that will be used in calculation
 */
void calculateVelocity(Particles&     pIn,
                       Particles&     pOut,
                       const unsigned N,
                       float          dt,
                       const unsigned streamNum); // I added new paramater to specify which stream will be used

/**
 * Calculate particles center of mass
 * @param p         - particles
 * @param comBuffer - pointer to a center of mass buffer
 * @param N         - Number of particles
 * @param streamNum - Number of stream that will be used in calculation
 */
void centerOfMass(Particles&     p,
                  float4*        comBuffer,
                  const unsigned N,
                  const unsigned streamNum); // I added new paramater to specify which stream will be used

/**
 * CPU implementation of the Center of Mass calculation
 * @param memDesc - Memory descriptor of particle data on CPU side
 */
float4 centerOfMassRef(MemDesc& memDesc);

#endif /* NBODY_H */
