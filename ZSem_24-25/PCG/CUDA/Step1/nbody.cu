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
 * CUDA kernel to calculate new particles velocity and position
 * @param pIn  - particles in
 * @param pOut - particles out
 * @param N    - Number of particles
 * @param dt   - Size of the time step
 */
__global__ void calculateVelocity(Particles pIn, Particles pOut, const unsigned N, float dt)
{
  /********************************************************************************************************************/
  /*          TODO: CUDA kernel to calculate new particles velocity and position, collapse previous kernels           */
  /********************************************************************************************************************/
    unsigned gId = blockIdx.x * blockDim.x + threadIdx.x;

    if (gId < N) {
        // Variables for writing resulting velocity
        // Gravitational
        float newGVelX{};
        float newGVelY{};
        float newGVelZ{};
        // Collision
        float newCVelX{};
        float newCVelY{};
        float newCVelZ{};

        // Variables used in calculation
        float dx, dy, dz, r, r_col, r2, f, par2Vel_X, par2Vel_Y, par2Vel_Z, par2Weight;

        // Getting repeating values from global memory
        const float posX = pIn.pos_X[gId];
        const float posY = pIn.pos_Y[gId];
        const float posZ = pIn.pos_Z[gId];
        const float velX = pIn.vel_X[gId];
        const float velY = pIn.vel_Y[gId];
        const float velZ = pIn.vel_Z[gId];
        const float weight = pIn.weights[gId];

        for (unsigned i = 0u; i < N; i++) {
            // Filling variables used in calculation
            par2Vel_X = pIn.vel_X[i];
            par2Vel_Y = pIn.vel_Y[i];
            par2Vel_Z = pIn.vel_Z[i];
            par2Weight = pIn.weights[i];
            
            dx = pIn.pos_X[i] - posX;
            dy = pIn.pos_Y[i] - posY;
            dz = pIn.pos_Z[i] - posZ;

            r2 = dx * dx + dy * dy + dz * dz;
            r_col = std::sqrt(r2);
            r = r_col + FLT_MIN;

            f = G * weight * pIn.weights[i] / r2 + FLT_MIN;
            // Gravitational force calculation
            newGVelX += (r > COLLISION_DISTANCE) ? dx / r * f : 0.f;
            newGVelY += (r > COLLISION_DISTANCE) ? dy / r * f : 0.f;
            newGVelZ += (r > COLLISION_DISTANCE) ? dz / r * f : 0.f;

            // Collision force calculation
            newCVelX += (r_col > 0.f && r_col < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velX + (2.f * par2Weight) * par2Vel_X) / (weight + par2Weight)) - velX) : 0.f;
            newCVelY += (r_col > 0.f && r_col < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velY + (2.f * par2Weight) * par2Vel_Y) / (weight + par2Weight)) - velY) : 0.f;
            newCVelZ += (r_col > 0.f && r_col < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velZ + (2.f * par2Weight) * par2Vel_Z) / (weight + par2Weight)) - velZ) : 0.f;
        }

        newGVelX *= dt / weight;
        newGVelY *= dt / weight;
        newGVelZ *= dt / weight;

        // Update particle
        pOut.vel_X[gId] = velX + newGVelX + newCVelX;
        pOut.pos_X[gId] = posX + (velX + newGVelX + newCVelX) * dt;

        pOut.vel_Y[gId] = velY + newGVelY + newCVelY;
        pOut.pos_Y[gId] = posY + (velY + newGVelY + newCVelY) * dt;

        pOut.vel_Z[gId] = velZ + newGVelZ + newCVelZ;
        pOut.pos_Z[gId] = posZ + (velZ + newGVelZ + newCVelZ) * dt;
    }
  
}// end of calculate_velocity
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
