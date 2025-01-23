/**
 * @file      nbody.cpp
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

#include <cfloat>
#include <cmath>

#include "nbody.h"
#include "Vec.h"

/* Constants */
constexpr float G                  = 6.67384e-11f;
constexpr float COLLISION_DISTANCE = 0.01f;

/*********************************************************************************************************************/
/*                TODO: Fullfill Partile's and Velocitie's constructors, destructors and methods                     */
/*                                    for data copies between host and device                                        */
/*********************************************************************************************************************/

/**
 * @brief Constructor
 * @param N - Number of particles
 */
Particles::Particles(const unsigned N)
: N(N)
{
  positions = new float3[N];
  velocities = new float3[N];
  weights = new float[N];

  #pragma acc enter data copyin(this[0:1])
  #pragma acc enter data create(positions[0:N])
  #pragma acc enter data create(velocities[0:N])
  #pragma acc enter data create(weights[0:N])
}

/// @brief Destructor
Particles::~Particles()
{
  #pragma acc exit data delete(positions[0:N])
  #pragma acc exit data delete(velocities[0:N])
  #pragma acc exit data delete(weights[0:N])
  #pragma acc exit data delete(this[0:1])

  delete [] positions;
  delete [] velocities;
  delete [] weights;
}

/**
 * @brief Copy particles from host to device
 */
void Particles::copyToDevice()
{
  #pragma acc update device(positions[0:N])
  #pragma acc update device(velocities[0:N])
  #pragma acc update device(weights[0:N])
}

/**
 * @brief Copy particles from device to host
 */
void Particles::copyToHost()
{
  #pragma acc update host(positions[0:N])
  #pragma acc update host(velocities[0:N])
  #pragma acc update host(weights[0:N])
}

/*********************************************************************************************************************/

/**
 * Calculate velocity
 * @param pIn  - particles input
 * @param pOut - particles output
 * @param N    - Number of particles
 * @param dt   - Size of the time step
 */
void calculateVelocity(Particles& pIn, Particles& pOut, const unsigned N, float dt)
{
  /*******************************************************************************************************************/
  /*                    TODO: Calculate gravitation velocity, see reference CPU version,                             */
  /*                            you can use overloaded operators defined in Vec.h                                    */
  /*******************************************************************************************************************/
  // Edited code from cpu implementation with overloaded float3 operators
  // Decided not to use tile(x, x), in my case it resulted that every xth result value (16th, 32nd, ...) was NaN
  #pragma acc parallel loop gang vector present(pIn, pOut)
  for (unsigned i = 0u; i < N; i++){
    const float3 myParticle = pIn.positions[i];
    const float3 myVelocity = pIn.velocities[i];
    const float myWeight = pIn.weights[i];

    float3 newVelG{};
    float3 newVelC{};
    for (unsigned j = 0u; j < N; j++){
      const float3 otherParticle = pIn.positions[j];
      const float3 otherVelocity = pIn.velocities[j];
      const float otherWeight = pIn.weights[j];

      const float3 d = otherParticle - myParticle;

      const float r = d.abs(); // I used abs() function from Vec.h library since it does sqrt(d.x * d.x + d.y * d.y + ...) of given vector

      const float f = G * myWeight * otherWeight / ((r * r) + FLT_MIN);

      newVelG += (r > COLLISION_DISTANCE) ? d / r * f : 0.f;
      newVelC += (r > 0.f && r < COLLISION_DISTANCE)
                ? (((myWeight * myVelocity - otherWeight * myVelocity + 2.f * otherWeight * otherVelocity) / (myWeight + otherWeight)) - myVelocity) 
                : 0.f;
    }

    // Both gravitational and collisional velocity is independetly calculated and then added together at the end
    newVelG *= dt / myWeight;

    pOut.velocities[i] += newVelG + newVelC;
    pOut.positions[i] += pOut.velocities[i] * dt;
  }

}// end of calculate_gravitation_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * Calculate particles center of mass
 * @param p    - particles
 * @param com  - pointer to a center of mass
 * @param lock - pointer to a user-implemented lock
 * @param N    - Number of particles
 */
void centerOfMass(Particles& p, float4* comBuffer, const unsigned N)
{

}// end of centerOfMass
//----------------------------------------------------------------------------------------------------------------------

/**
 * CPU implementation of the Center of Mass calculation
 * @param particles - All particles in the system
 * @param N         - Number of particles
 */
float4 centerOfMassRef(MemDesc& memDesc)
{
  float4 com{};

  for (std::size_t i{}; i < memDesc.getDataSize(); i++)
  {
    const float3 pos = {memDesc.getPosX(i), memDesc.getPosY(i), memDesc.getPosZ(i)};
    const float  w   = memDesc.getWeight(i);

    // Calculate the vector on the line connecting current body and most recent position of center-of-mass
    // Calculate weight ratio only if at least one particle isn't massless
    const float4 d = {pos.x - com.x,
                      pos.y - com.y,
                      pos.z - com.z,
                      ((memDesc.getWeight(i) + com.w) > 0.0f)
                        ? ( memDesc.getWeight(i) / (memDesc.getWeight(i) + com.w))
                        : 0.0f};

    // Update position and weight of the center-of-mass according to the weight ration and vector
    com.x += d.x * d.w;
    com.y += d.y * d.w;
    com.z += d.z * d.w;
    com.w += w;
  }

  return com;
}// enf of centerOfMassRef
//----------------------------------------------------------------------------------------------------------------------
