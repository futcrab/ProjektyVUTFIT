/**
 * @file      nbody.cu
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

#include <device_launch_parameters.h>

#include "nbody.cuh"

// Added this because of FLT_MIN not being found on karolina
#include <cfloat>

/* Constants */
constexpr float G                  = 6.67384e-11f;
constexpr float COLLISION_DISTANCE = 0.01f;

/**
 * CUDA kernel to calculate gravitation velocity
 * @param p      - particles
 * @param tmpVel - temp array for velocities
 * @param N      - Number of particles
 * @param dt     - Size of the time step
 */
__global__ void calculateGravitationVelocity(Particles p, Velocities tmpVel, const unsigned N, float dt)
{
  /********************************************************************************************************************/
  /*              TODO: CUDA kernel to calculate gravitation velocity, see reference CPU version                      */
  /********************************************************************************************************************/
    unsigned gId = blockIdx.x * blockDim.x + threadIdx.x;

    if (gId < N) {
        // Variables for writing resulting velocity
        float newVelX{};
        float newVelY{};
        float newVelZ{};

        // Variables used in calculation
        float dx, dy, dz, r, r2, f;

        // Getting repeating values from global memory
        const float posX = p.pos_X[gId];
        const float posY = p.pos_Y[gId];
        const float posZ = p.pos_Z[gId];
        const float weight = p.weights[gId];

        for (unsigned i = 0u; i < N; i++) {
            // Calculating gravitational force of all other particles and summing them together
            dx = p.pos_X[i] - posX;
            dy = p.pos_Y[i] - posY;
            dz = p.pos_Z[i] - posZ;

            r2 = dx * dx + dy * dy + dz * dz;
            r = std::sqrt(r2) + FLT_MIN;

            f = G * weight * p.weights[i] / r2 + FLT_MIN;

            newVelX += (r > COLLISION_DISTANCE) ? dx / r * f : 0.f;
            newVelY += (r > COLLISION_DISTANCE) ? dy / r * f : 0.f;
            newVelZ += (r > COLLISION_DISTANCE) ? dz / r * f : 0.f;
        }

        newVelX *= dt / weight;
        newVelY *= dt / weight;
        newVelZ *= dt / weight;

        // Adding resulting velocity to temporal memory
        tmpVel.dir_X[gId] = newVelX;
        tmpVel.dir_Y[gId] = newVelY;
        tmpVel.dir_Z[gId] = newVelZ;
    }
}// end of calculate_gravitation_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to calculate collision velocity
 * @param p      - particles
 * @param tmpVel - temp array for velocities
 * @param N      - Number of particles
 * @param dt     - Size of the time step
 */
__global__ void calculateCollisionVelocity(Particles p, Velocities tmpVel, const unsigned N, float dt)
{
  /********************************************************************************************************************/
  /*              TODO: CUDA kernel to calculate collision velocity, see reference CPU version                        */
  /********************************************************************************************************************/
    unsigned gId = blockIdx.x * blockDim.x + threadIdx.x;

    if (gId < N) {
        // Variables for writing resulting velocity
        float newVelX{};
        float newVelY{};
        float newVelZ{};

        // Variables used in calculation
        float dx, dy, dz, r, r2, par2Vel_X, par2Vel_Y, par2Vel_Z, par2Weight;

        // Getting repeating values from global memory
        const float posX = p.pos_X[gId];
        const float posY = p.pos_Y[gId];
        const float posZ = p.pos_Z[gId];
        const float velX = p.vel_X[gId];
        const float velY = p.vel_Y[gId];
        const float velZ = p.vel_Z[gId];
        const float weight = p.weights[gId];
        
        // Calculating gravitational force of all other particles and summing them together
        for (unsigned i = 0u; i < N; i++) {
            par2Vel_X = p.vel_X[i];
            par2Vel_Y = p.vel_Y[i];
            par2Vel_Z = p.vel_Z[i];
            par2Weight = p.weights[i];

            dx = p.pos_X[i] - posX;
            dy = p.pos_Y[i] - posY;
            dz = p.pos_Z[i] - posZ;

            r2 = dx * dx + dy * dy + dz * dz;
            r = std::sqrt(r2);

            newVelX += (r > 0.f && r < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velX + (2.f * par2Weight) * par2Vel_X) / (weight + par2Weight)) - velX) : 0.f;
            newVelY += (r > 0.f && r < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velY + (2.f * par2Weight) * par2Vel_Y) / (weight + par2Weight)) - velY) : 0.f;
            newVelZ += (r > 0.f && r < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velZ + (2.f * par2Weight) * par2Vel_Z) / (weight + par2Weight)) - velZ) : 0.f;
        }
        // Adding resulting velocity to temporal memory
        tmpVel.dir_X[gId] += newVelX;
        tmpVel.dir_Y[gId] += newVelY;
        tmpVel.dir_Z[gId] += newVelZ;
    }

}// end of calculate_collision_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to update particles
 * @param p      - particles
 * @param tmpVel - temp array for velocities
 * @param N      - Number of particles
 * @param dt     - Size of the time step
 */
__global__ void updateParticles(Particles p, Velocities tmpVel, const unsigned N, float dt)
{
  /********************************************************************************************************************/
  /*             TODO: CUDA kernel to update particles velocities and positions, see reference CPU version            */
  /********************************************************************************************************************/
    unsigned gId = blockIdx.x * blockDim.x + threadIdx.x;

    if (gId < N) {
        // Setting new position and velocity of particles for next step
        p.vel_X[gId] += tmpVel.dir_X[gId];
        p.pos_X[gId] += p.vel_X[gId] * dt;

        p.vel_Y[gId] += tmpVel.dir_Y[gId];
        p.pos_Y[gId] += p.vel_Y[gId] * dt;

        p.vel_Z[gId] += tmpVel.dir_Z[gId];
        p.pos_Z[gId] += p.vel_Z[gId] * dt;
    }

}// end of update_particle
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to calculate particles center of mass
 * @param p    - particles
 * @param com  - pointer to a center of mass
 * @param lock - pointer to a user-implemented lock
 * @param N    - Number of particles
 */
__global__ void centerOfMass(Particles p, float4* com, int* lock, const unsigned N)
{

}// end of centerOfMass
//----------------------------------------------------------------------------------------------------------------------

/**
 * CPU implementation of the Center of Mass calculation
 * @param particles - All particles in the system
 * @param N         - Number of particles
 */
__host__ float4 centerOfMassRef(MemDesc& memDesc)
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
