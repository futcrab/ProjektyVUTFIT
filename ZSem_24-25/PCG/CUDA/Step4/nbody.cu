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

 //I Added this because Intellisense kept underlining __syncthreads()
#ifndef __CUDACC__  
#define __CUDACC__
#endif

// Added this because of FLT_MIN not being found on karolina
#include <cfloat>

#include <device_launch_parameters.h>

#include "nbody.cuh"

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
  /*  TODO: CUDA kernel to calculate new particles velocity and position, use shared memory to minimize memory access */
  /********************************************************************************************************************/
    extern __shared__ float sharedMem[];

    float* shared_posX = sharedMem;
    float* shared_posY = &sharedMem[blockDim.x];
    float* shared_posZ = &sharedMem[2 * blockDim.x];
    float* shared_velX = &sharedMem[3 * blockDim.x];
    float* shared_velY = &sharedMem[4 * blockDim.x];
    float* shared_velZ = &sharedMem[5 * blockDim.x];
    float* shared_weight = &sharedMem[6 * blockDim.x];

    unsigned gId = blockIdx.x * blockDim.x + threadIdx.x;

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
    const float posX = gId < N ? pIn.pos_X[gId] : 0.0f;
    const float posY = gId < N ? pIn.pos_Y[gId] : 0.0f;
    const float posZ = gId < N ? pIn.pos_Z[gId] : 0.0f;
    const float velX = gId < N ? pIn.vel_X[gId] : 0.0f;
    const float velY = gId < N ? pIn.vel_Y[gId] : 0.0f;
    const float velZ = gId < N ? pIn.vel_Z[gId] : 0.0f;
    const float weight = gId < N ? pIn.weights[gId] : 0.0f;

    float tileCount = ceil(float(N) / blockDim.x);

    //running trough all 
    for (unsigned tileID = 0u; tileID < tileCount; tileID++) {
        unsigned tileOffsetPos = tileID * blockDim.x;
        unsigned par2Pos = tileOffsetPos + threadIdx.x;

        shared_posX[threadIdx.x] = (par2Pos < N) ? pIn.pos_X[par2Pos] : 0.0f;
        shared_posY[threadIdx.x] = (par2Pos < N) ? pIn.pos_Y[par2Pos] : 0.0f;
        shared_posZ[threadIdx.x] = (par2Pos < N) ? pIn.pos_Z[par2Pos] : 0.0f;
        shared_velX[threadIdx.x] = (par2Pos < N) ? pIn.vel_X[par2Pos] : 0.0f;
        shared_velY[threadIdx.x] = (par2Pos < N) ? pIn.vel_Y[par2Pos] : 0.0f;
        shared_velZ[threadIdx.x] = (par2Pos < N) ? pIn.vel_Z[par2Pos] : 0.0f;
        shared_weight[threadIdx.x] = (par2Pos < N) ? pIn.weights[par2Pos] : 0.0f;

        __syncthreads();

        if (gId < N) {
            for (unsigned i = 0u; i < blockDim.x; i++) {
                // Filling variables used in calculation
                par2Vel_X = shared_velX[i];
                par2Vel_Y = shared_velY[i];
                par2Vel_Z = shared_velZ[i];
                par2Weight = shared_weight[i];

                dx = shared_posX[i] - posX;
                dy = shared_posY[i] - posY;
                dz = shared_posZ[i] - posZ;

                r2 = dx * dx + dy * dy + dz * dz;
                r_col = std::sqrt(r2);
                r = r_col + FLT_MIN;

                f = G * weight * shared_weight[i] / r2 + FLT_MIN;
                // Gravitational force calculation
                newGVelX += (r > COLLISION_DISTANCE) ? dx / r * f : 0.f;
                newGVelY += (r > COLLISION_DISTANCE) ? dy / r * f : 0.f;
                newGVelZ += (r > COLLISION_DISTANCE) ? dz / r * f : 0.f;

                // Collision force calculation
                newCVelX += (r_col > 0.f && r_col < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velX + (2.f * par2Weight) * par2Vel_X) / (weight + par2Weight)) - velX) : 0.f;
                newCVelY += (r_col > 0.f && r_col < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velY + (2.f * par2Weight) * par2Vel_Y) / (weight + par2Weight)) - velY) : 0.f;
                newCVelZ += (r_col > 0.f && r_col < COLLISION_DISTANCE) ? ((((weight - par2Weight) * velZ + (2.f * par2Weight) * par2Vel_Z) / (weight + par2Weight)) - velZ) : 0.f;
            }
        }
        __syncthreads();
    }

    if (gId < N) {
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

  
}// end of calculate_gravitation_velocity
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
  /********************************************************************************************************************/
  /*           TODO: CUDA kernel to calculate particles center of mass, see reference CPU implementation,             */
  /*                                 use CUDA predefined warpSize variable                                            */
  /********************************************************************************************************************/
    auto warpReduce = [](float4 val) {
        // Reduce function using stride to gradually calculate center of mass
        // which will be stored in first thread of warp
#pragma unroll
        for (unsigned stride = warpSize / 2; stride > 0u; stride >>= 1u) {
            // Create d variable for delta values
            float4 d{};
            // get weight just once
            float newParticleWeight = __shfl_down_sync(0xFFFFFFFF, val.w, stride, warpSize);
            // Get values from thread down by stride.
            // Decided not to use mask to mask out threads because it kept calling it
            // an illegal command when i used mask 0x0000FFFF and lesser on my own pc, on karolina it worked fine.
            // Using all threads shouldnt have negative effect on final value.
            d.x = __shfl_down_sync(0xFFFFFFFF, val.x, stride, warpSize) - val.x;
            d.y = __shfl_down_sync(0xFFFFFFFF, val.y, stride, warpSize) - val.y;
            d.z = __shfl_down_sync(0xFFFFFFFF, val.z, stride, warpSize) - val.z;
            d.w = ((newParticleWeight + val.w) > 0.f)
                ? (newParticleWeight / (newParticleWeight + val.w))
                : 0.f;
            // Save new calculated values in variable
            val.x += d.x * d.w;
            val.y += d.y * d.w;
            val.z += d.z * d.w;
            val.w += newParticleWeight;
        }
        return val;
        };

    // Shared memory
    extern __shared__ float sharedComMem[];
    float* shared_posX = sharedComMem;
    float* shared_posY = &sharedComMem[blockDim.x / warpSize];
    float* shared_posZ = &sharedComMem[2 * (blockDim.x / warpSize)];
    float* shared_weight = &sharedComMem[3 * (blockDim.x / warpSize)];

    // Get particle data from global memory
    unsigned gId = blockIdx.x * blockDim.x + threadIdx.x;
    float4 partialCOM{};
    if (gId < N) {
        partialCOM.x = p.pos_X[gId];
        partialCOM.y = p.pos_Y[gId];
        partialCOM.z = p.pos_Z[gId];
        partialCOM.w = p.weights[gId];
    }

    // If the're more particles then threads load them as well
    for (unsigned i = (gridDim.x * blockDim.x) + gId; i < N; i += gridDim.x * blockDim.x) {
        // Calculate partialCOM for every thread
        // Create d variable for delta values
        float4 d{};
        // get weight from global memory just once.
        float newParticleWeight = p.weights[i];

        d.x = p.pos_X[i] - partialCOM.x;
        d.y = p.pos_Y[i] - partialCOM.y;
        d.z = p.pos_Z[i] - partialCOM.z;
        d.w = ((newParticleWeight + partialCOM.w) > 0.0f)
            ? (newParticleWeight / (newParticleWeight + partialCOM.w))
            : 0.f;

        // save new values in partial sum
        partialCOM.x += d.x * d.w;
        partialCOM.y += d.y * d.w;
        partialCOM.z += d.z * d.w;
        partialCOM.w += newParticleWeight;
    }

    // get PartialCOM for warp
    partialCOM = warpReduce(partialCOM);

    // first threads in warps save their warps partialCOMs in shared memory 
    if ((threadIdx.x & (warpSize - 1)) == 0u)
    {
        shared_posX[threadIdx.x / warpSize] = partialCOM.x;
        shared_posY[threadIdx.x / warpSize] = partialCOM.y;
        shared_posZ[threadIdx.x / warpSize] = partialCOM.z;
        shared_weight[threadIdx.x / warpSize] = partialCOM.w;
    }

    __syncthreads();

    // depending on number of warps in block first few threads load partialCOMs from shared memory
    // others reset their values
    partialCOM.x = (threadIdx.x < blockDim.x / warpSize) ? shared_posX[threadIdx.x] : 0.f;
    partialCOM.y = (threadIdx.x < blockDim.x / warpSize) ? shared_posY[threadIdx.x] : 0.f;
    partialCOM.z = (threadIdx.x < blockDim.x / warpSize) ? shared_posZ[threadIdx.x] : 0.f;
    partialCOM.w = (threadIdx.x < blockDim.x / warpSize) ? shared_weight[threadIdx.x] : 0.f;

    // Calculate center of mass for whole block
    partialCOM = warpReduce(partialCOM);


    // now first thread in the block holds center of mass for its block
    // use atomic lock to prevent race condition
    // calculation of final COM using COMs of blocks
    if (threadIdx.x == 0u) {
        while (atomicCAS(lock, 0, 1) != 0);
        float4 d;
        d.x = partialCOM.x - com->x;
        d.y = partialCOM.y - com->y;
        d.z = partialCOM.z - com->z;
        d.w = ((partialCOM.w + com->w) > 0.0f) ? (partialCOM.w / (partialCOM.w + com->w)) : 0.0f;

        com->x += d.x * d.w;
        com->y += d.y * d.w;
        com->z += d.z * d.w;
        com->w += partialCOM.w;

        // after calculating unlock lock
        atomicExch(lock, 0);
    }

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
