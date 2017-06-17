
#ifndef KERNEL_CU
#define KERNEL_CU

#define INF 16 * 16 * 16
#include <stdio.h>
enum States { LIQUID = 0, SOLID };

__device__ const float TMelt = 60;
__device__ const float PI = 3.141592653;
__device__ const float MU = 10.0;
__device__ const float DT = 0.00096;
//__device__ const float SIGMA = 0.0005;
__device__ const float CONDUCTIVITY = 0.15;
__device__ const float REST_DENSITY = 1.001225;
__device__ const float GAS_CONST = 998888.0144621;
__device__ const float MASS = 1000.0 / (20 * 20 * 20);
__device__ const float ROOM_TEMP = 10.0;
//=====Math functions
//---Basic math
__device__ __host__ float pow2(const float &x) { return x * x; }
__device__ __host__ float pow3(const float &x) { return x * x * x; }
__device__ __host__ float pow6(const float &x) { return pow3(x) * pow3(x); }
__device__ __host__ float pow7(const float &x) { return pow6(x) * x; }
__device__ __host__ float pow9(const float &x) { return pow6(x) * pow3(x); }
__device__ __host__ float3 operator+(const float3 &a, const float3 &b)
{
    return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}
__device__ __host__ int3 operator+(const int3 &a, const int3 &b)
{
    return make_int3(a.x + b.x, a.y + b.y, a.z + b.z);
}
__device__ __host__ float3 operator-(const float3 &a, const float3 &b)
{
    return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

__device__ __host__ float3 operator*(const float &n, const float3 &v)
{
    return make_float3(n * v.x, n * v.y, n * v.z);
}
__device__ __host__ float3 operator*(const float3 &v, const float &n)
{
    return make_float3(n * v.x, n * v.y, n * v.z);
}
__device__ __host__ float3 operator/(const float3 &v, const float &n)
{
    return make_float3(v.x / n, v.y / n, v.z / n);
}
__device__ __host__ float norm(const float3 &v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
__device__ float3 normalize(const float3 &v) { return v / norm(v); }
__device__ float dot(const float3 &v1, const float3 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Kernel functions
__device__ float surfaceTensionCoef(const float3 &v1, const float3 &v2,
                                    const float &coreR)
{
    float3 r = v1 - v2;
    float dis = norm(r);
    float ans = 32 / (PI * pow9(coreR));
    if (2 * dis > coreR && dis <= coreR)
        ans *= pow3(coreR - dis) * pow3(dis);
    else if (dis > 0.0 && 2 * dis <= coreR)
        ans *= 2 * pow3(coreR - dis) * pow3(dis) - pow6(coreR) / 64;
    else
        ans *= 0.0;
    return ans;
}
__device__ float kernelPoly6(const float3 &v1, const float3 &v2,
                             const float &coreR)
{
    float3 r = v1 - v2;
    float dis = norm(r);
    if (dis > coreR)
        return 0.0;
    else
        return 315.0 / (64.0f * PI * pow9(coreR)) *
               pow3(pow2(coreR) - dot(r, r));
}

__device__ float3 kernelPoly6Grad(const float3 &v1, const float3 &v2,
                                  const float &coreR)
{
    float3 r = v1 - v2;
    float dis = norm(r);
    if (dis <= coreR)
        return -945.0 / (32.0 * PI * pow9(coreR)) *
               pow2(pow2(coreR) - dot(r, r)) * r;
    else
        return make_float3(0, 0, 0);
}
__device__ float kerenlPoly6Lap(const float3 &v1, const float3 &v2,
                                const float &coreR)
{
    float3 r = v1 - v2;
    float dis = norm(r);
    if (dis <= coreR)
        return 945.0 / (32.0 * PI * pow9(coreR)) * (pow2(coreR) - dot(r, r)) *
               (7.0 * dot(r, r) - 3.0 * pow2(coreR));
    else
        return 0.0;
}

__device__ float3 kernelSpikyGrad(float3 v1, float3 v2, float coreR)
{
    float3 r = v1 - v2;
    float dis = norm(r);

    if (dis > coreR || fabs(dis - 0.0) < 0.00001)
        return make_float3(0.0, 0.0, 0.0);
    else
        return -45.0 / (PI * pow6(coreR)) * pow2(coreR - dis) * r / dis;
}
__device__ float kernelViscosityLap(const float3 &v1, const float3 &v2,
                                    float coreR)
{
    float dis = norm(v2 - v1);
    if (dis > coreR)
        return 0.0;
    else
        return 45.0 / (PI * pow6(coreR)) * (coreR - dis);
}
__device__ float F(const float3 &v1, const float3 &v2, const float &coreR)
{
    float q = norm(v2 - v1) / coreR;

    float kSigma = 1 / (PI * pow3(coreR));
    if (q < 0.5)
        return kSigma * (9 / 4 * norm(v2 - v1) / pow3(coreR) - 3 / pow2(coreR));
    else if (q <= 1)
        return kSigma * (1 / 2 * pow2(2 - q)) / (coreR * norm(v2 - v1));
    else
        return 0.0;
}
// simple conversion functions
// return INF if the cell is out of bundary
__device__ __host__ uint getIdxByXYZ(const int3 &xyz)
{
    // assume grid is 16x16x16
    if (xyz.x < 16 && xyz.x >= 0 && xyz.y < 16 && xyz.y >= 0 && xyz.z < 16 &&
        xyz.z >= 0)
        return (uint)(xyz.x + xyz.y * 16 + xyz.z * 16 * 16);
    else
        return INF;
}
__device__ __host__ int3 getXYZByIdx(const uint &idx)
{
    return make_int3((int)(idx % 16), (int)(idx % (16 * 16) / 16),
                     (int)(idx / (16 * 16)));
}
//================Math functions end==

__global__ void cpy2Tmp(uint *tmpCellIDs, uint *curCellIDs, float3 *tmpSpeed,
                        float3 *curSpeed, float3 *tmpPos, float3 *curPos,
                        States *tmpStates, States *curStates, float *tmpTemp,
                        float *curTemp, uint numOfPar)
{
    uint idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint stride = blockDim.x * gridDim.x;
    while (idx < numOfPar) {
        // pointIdx[idx] = idx;
        // deltaTemp[idx] = 0;
        tmpSpeed[idx] = curSpeed[idx];
        tmpPos[idx] = curPos[idx];
        tmpCellIDs[idx] = curCellIDs[idx];
        tmpStates[idx] = curStates[idx];
        tmpTemp[idx] = curTemp[idx];
        idx += stride;
    }
}

__global__ void resetGrid(uint *count, uint3 gridRes)
{
    uint idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint stride = blockDim.x * gridDim.x;
    while (idx < gridRes.x * gridRes.y * gridRes.z) {
        count[idx] = 0;
        idx += stride;
    }
}

__global__ void insertPars(
    // particle parms
    float3 *pos,
    uint *cellIDs,  // output
    uint *numOfPar,
    uint *offset,  // output
    // grid params
    float3 startPoint, float3 cellSize, uint3 gridRes, uint *count,
    uint numPar)  // output
{
    uint idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint stride = blockDim.x * gridDim.x;
    while (idx < numPar) {
        float3 curPos = pos[idx];
        if (curPos.x > 14.9 || curPos.x < -14.9 || curPos.y > 14.9 ||
            curPos.y < -14.9 || curPos.z > 14.9 || curPos.z < -14.9) {
            cellIDs[idx] = INF;
            uint temp = atomicSub(numOfPar, 1);
        }
        else {
            uint x = floor((curPos.x - startPoint.x) / cellSize.x);
            uint y = floor((curPos.y - startPoint.y) / cellSize.y);
            uint z = floor((curPos.z - startPoint.z) / cellSize.z);
            int3 xyz = make_int3(x, y, z);
            uint cellID = getIdxByXYZ(xyz);
            uint curOffset = atomicAdd(&(count[cellID]), (uint)1);
            offset[idx] = curOffset;
            cellIDs[idx] = cellID;
        }
        idx += stride;
    }
}

// perform prefix sum to determin the positions of first elements
#define BLOCK_SIZE 1024
__global__ void prefixSumKernel(const uint *count, uint *firstIdx,
                                uint *sumArray, uint numOfCells)
{
    int t = threadIdx.x;
    int b = blockIdx.x;

    // copy value into shared memo
    __shared__ uint sumTemp[2 * BLOCK_SIZE];
    sumTemp[t] = count[2 * b * blockDim.x + t];
    sumTemp[t + blockDim.x] = count[2 * b * blockDim.x + blockDim.x + t];
    __syncthreads();

    // reduction
    for (uint stride = 1; stride <= blockDim.x; stride *= 2) {
        uint index = (threadIdx.x + 1) * stride * 2 - 1;
        if (index < 2 * blockDim.x) sumTemp[index] += sumTemp[index - stride];
        __syncthreads();
    }
    // reverse sweep
    if (threadIdx.x == 0) {
        if (sumArray) sumArray[blockIdx.x] = sumTemp[blockDim.x * 2 - 1];
        sumTemp[blockDim.x * 2 - 1] = 0;
    }
    __syncthreads();

    for (uint stride = blockDim.x; stride > 0; stride /= 2) {
        uint index = (threadIdx.x + 1) * stride * 2 - 1;
        if (index < 2 * blockDim.x) {
            uint temp = sumTemp[index];
            sumTemp[index] += sumTemp[index - stride];
            sumTemp[index - stride] = temp;
        }
        __syncthreads();
    }
    // copy back to array
    uint outputIndx = 2 * blockDim.x * blockIdx.x + t;
    if (outputIndx < numOfCells) firstIdx[outputIndx] = sumTemp[t];
    outputIndx += blockDim.x;
    if (outputIndx < numOfCells) firstIdx[outputIndx] = sumTemp[t + blockDim.x];
}

__global__ void prefexSumPostShort(uint *firstIdx, uint *sumArray,
                                   int sumArrayLength)
{
    if (blockIdx.x == 0)
        return;
    else {
        __shared__ int sum;
        if (threadIdx.x == 0) sum = sumArray[blockIdx.x - 1];
        __syncthreads();
        firstIdx[2 * blockDim.x * blockIdx.x + threadIdx.x] += sum;
        firstIdx[2 * blockDim.x * blockIdx.x + blockDim.x + threadIdx.x] += sum;
    }
}

__global__ void countingSort(uint *startIdx, uint *offset, uint *srcCellIDs,
                             uint *distCellIDs, uint *count,
                             // values to swap
                             float3 *srcSpeed, float3 *distSpeed,

                             float3 *srcPos, float3 *distPos,

                             States *srcStates, States *disStates,

                             float *srcTemp, float *distTemp, uint numPar)
{
    uint i = blockDim.x * blockIdx.x + threadIdx.x;
    uint stride = blockDim.x * gridDim.x;
    while (i < numPar) {
        uint cellID = srcCellIDs[i];
        if (cellID < INF) {
            int index = startIdx[cellID] + offset[i];
            if (offset[i] >= count[cellID]) {
                printf(
                    "\nindex out of range: %d\nCellID: %d\nstartIdx: "
                    "%d\noffset: %d\nCount: %d\n Pidx: %d\nPos: "
                    "%f,%f,%f\nnumPar: %d\n",
                    index, cellID, startIdx[cellID], offset[i], count[cellID],
                    i, srcPos[i].x, srcPos[i].y, srcPos[i].z, numPar);
            }
            distPos[index] = srcPos[i];
            distSpeed[index] = srcSpeed[i];
            disStates[index] = srcStates[i];
            distTemp[index] = srcTemp[i];
            distCellIDs[index] = cellID;
        }
        i += stride;
    }
}
__device__ const int3 neighborList[27] = {
    {0, 0, 0},   {-1, -1, -1}, {-1, -1, 0}, {-1, -1, 1}, {-1, 0, -1},
    {-1, 0, 0},  {-1, 0, 1},   {-1, 1, -1}, {-1, 1, 0},  {-1, 1, 1},

    {0, -1, -1}, {0, -1, 0},   {0, -1, 1},  {0, 0, -1},  {0, 0, 1},
    {0, 1, -1},  {0, 1, 0},    {0, 1, 1},

    {1, -1, -1}, {1, -1, 0},   {1, -1, 1},  {1, 0, -1},  {1, 0, 0},
    {1, 0, 1},   {1, 1, -1},   {1, 1, 0},   {1, 1, 1}};

__global__ void
calculateDensitiesGlobal(  // Calculate densities without shared memory
    float3 *pos, float *densiteis, uint *cellIDs, uint *startIndex, uint *count,
    uint numPar)
{
    uint idx = threadIdx.x + blockDim.x * blockIdx.x;
    uint stride = blockDim.x * gridDim.x;

    while (idx < numPar) {
        float density = 0;
        float3 curPos = pos[idx];
        uint curCellID = cellIDs[idx];
        for (int i = 0; i < 27; i++) {
            uint cellIdx =
                getIdxByXYZ(getXYZByIdx(curCellID) + neighborList[i]);
            if (cellIdx == INF) continue;
            uint startIdx = startIndex[cellIdx];
            for (uint j = 0; j < count[cellIdx]; j++) {
                density += kernelPoly6(pos[startIdx + j], curPos, 1.8) * MASS;
            }
        }
        densiteis[idx] = density;
        idx += stride;
    }
}

#define MAX_N_NUM 1350
__global__ void
calculateDensitiesShared(  // Calculate densities without shared memory
    float3 *pos, float *densiteis, uint *cellIDs, uint *startIndex, uint *count,
    uint numPar)
{
    if (count[blockIdx.x] == 0) return;
    __shared__ float3 nPos[MAX_N_NUM];
    // load neighbor pos into shared memory
    __shared__ int sharedIdx;
    if (threadIdx.x == 0)  // using first thread to load shared memory
    {
        sharedIdx = 0;
        for (int i = 0; i < 27; i++) {
            uint cellIdx =
                getIdxByXYZ(getXYZByIdx(blockIdx.x) + neighborList[i]);
            if (cellIdx == INF) continue;
            uint startIdx = startIndex[cellIdx];
            for (int j = 0; j < count[cellIdx]; j++) {
                nPos[sharedIdx++] = pos[startIdx + j];
            }
        }
    }
    __syncthreads();

    uint stride = blockDim.x;
    uint idx = threadIdx.x;
    while (idx < count[blockIdx.x]) {
        float dens = 0.0;
        uint parIdx = startIndex[blockIdx.x] + idx;
        float3 curPos = nPos[idx];
        for (int i = 0; i < sharedIdx; i++)
            dens += kernelPoly6(nPos[i], curPos, 1.8) * MASS;
        densiteis[parIdx] = dens;
        idx += stride;
    }
}

__global__ void computeColorGradsGlobal(float3 *pos, float *densities,
                                        float3 *colorGrads, float *temperature,
                                        float *densSum, uint *densCount,
                                        uint *states, uint *cellIDs,
                                        uint *startIndex, uint *count,
                                        uint numPar)
{
    uint idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint stride = blockDim.x * gridDim.x;
    while (idx < numPar) {
        float3 colorGrad = make_float3(0.0, 0.0, 0.0);
        float du = 0.0;
        float curDens = densities[idx];
        uint curCellID = cellIDs[idx];
        float curTemp = temperature[idx];
        float3 curPos = pos[idx];
        for (int i = 0; i < 27; i++) {
            uint cellIdx =
                getIdxByXYZ(getXYZByIdx(curCellID) + neighborList[i]);
            uint startIdx = startIndex[cellIdx];
            for (uint j = 0; j < count[cellIdx]; j++) {
                // if (startIdx + j == idx)
                //    continue;
                float nDens = densities[startIdx + j];
                float3 nPos = pos[startIdx + j];
                float nTemp = temperature[startIdx + j];
                // Color filed
                colorGrad =
                    colorGrad +
                    1.5 * MASS / nDens * kernelPoly6Grad(curPos, nPos, 1.8);
                // Kernel radius should be large enough
                float Fkernel = F(curPos, nPos, 1.8);
                // update delta temperature
                du += MASS / (curDens * nDens) * 4 * CONDUCTIVITY *
                      CONDUCTIVITY / (2 * CONDUCTIVITY) * (curTemp - nTemp) *
                      Fkernel;
            }
        }
        colorGrads[idx] = colorGrad;
        if (norm(colorGrad) > 0.5) {
            atomicAdd(densSum, densities[idx]);
            atomicAdd(densCount, 1);
        }
        float newTemp = curTemp + du;
        if (norm(colorGrad) < 0.1) {
            atomicAdd(densSum, densities[idx]);
            atomicAdd(densCount, 1);
        }

        if (norm(colorGrad) > 0.01)
            ;
        newTemp += 30.0 * (ROOM_TEMP - newTemp) * DT;
        if (newTemp > TMelt)
            states[idx] = LIQUID;
        else
            states[idx] = SOLID;

        temperature[idx] = curTemp + du;
        idx += stride;
    }
}

__global__ void
calculateForcesGlobal(  // calculate forces without shared memory
    float3 *pos, float *densities, float3 *velos, float3 *pressures,
    float3 *colorGrads, uint *cellIDs, uint *states, uint *startIndex,
    uint *count, float3 *normal, uint numPar)
{
    uint idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint stride = blockDim.x * gridDim.x;
    while (idx < numPar) {
        float3 pressure = make_float3(0.0, 0.0, 0.0);
        float3 viscosity = make_float3(0.0, 0.0, 0.0);
        float3 externalF = make_float3(0.0, -980.0, 0.0);
        uint curCellID = cellIDs[idx];
        if (curCellID == INF || states[idx] == SOLID) {
            idx += stride;
            continue;
        }
        float3 curPos = pos[idx];
        float curDens = densities[idx];
        float curP = max(0.0, GAS_CONST * (curDens - REST_DENSITY));
        float3 curVelo = velos[idx];
        float3 curCG = colorGrads[idx];

        float3 surfaceTensionF = make_float3(0.0, 0.0, 0.0);

        // neighbor temp values
        float3 nPos;
        float nDens;
        float3 nVelo;
        float nP;
        float3 nCG;  // neighbor color gradient
        for (int i = 0; i < 27; i++) {
            uint cellIdx =
                getIdxByXYZ(getXYZByIdx(curCellID) + neighborList[i]);
            if (cellIdx == INF) continue;
            uint startIdx = startIndex[cellIdx];
            for (uint j = 0; j < count[cellIdx]; j++) {
                if (startIdx + j == idx) continue;

                nPos = pos[startIdx + j];
                nDens = densities[startIdx + j];
                nVelo = velos[startIdx + j];
                nCG = colorGrads[startIdx + j];

                if (norm(curPos - nPos) > 1.8) continue;

                // pressure
                nP = max(0.0, GAS_CONST * (nDens - REST_DENSITY));
                pressure = pressure -
                           MASS * (curP + nP) / (2 * nDens) *
                               kernelSpikyGrad(curPos, nPos, 1.8);

                // Viscosity
                viscosity = viscosity +
                            MU * MASS * (nVelo - curVelo) / nDens *
                                kernelViscosityLap(curPos, nPos, 1.8);
            }
        }

        normal[idx] = surfaceTensionF;
        float3 acc =
            (pressure + viscosity + surfaceTensionF) / curDens + externalF;
        float3 newVelo = curVelo + acc * DT;
        float3 newPos = curPos + newVelo * DT;

        float cubeSize = 14.5;
        const float COEF = 0.7;
        //if (newPos.x > cubeSize) {
        //    newVelo.x *= -COEF;
        //    newPos.x = cubeSize;
        //}
        //if (newPos.x < -cubeSize) {
        //    newVelo.x *= -COEF;
        //    newPos.x = -cubeSize;
        //}
        //if (newPos.y > cubeSize) {
        //    newVelo.y *= -COEF;
        //    newPos.y = cubeSize;
        //}
        //if (newPos.z > cubeSize) {
        //    newVelo.z *= -COEF;
        //    newPos.z = cubeSize;
        //}
        //if (newPos.z < -cubeSize) {
        //    newVelo.z *= -COEF;
        //    newPos.z = -cubeSize;
        //}
        if (newPos.y < -cubeSize) {
            newVelo.y *= -COEF;
            newPos.y = -cubeSize;
        }

        velos[idx] = newVelo;
        pos[idx] = newPos;

        idx += stride;
    }
}

// PCI SPH Iteration
__global__ void predictPos();
__global__ void predictDens();
__global__ void predictPressure();

#endif
