//
// Created by shriram.krishna on 11/9/20.
//

#ifndef AABB_TRIANGLE_INTERSECTION_TRIANGLE4_UTILS_CUH
#define AABB_TRIANGLE_INTERSECTION_TRIANGLE4_UTILS_CUH

#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "triangle4.h"
#include "cuda_math_utils.h"
#include "params.h"
#include <cuda_occupancy.h>
#include "cuda_error_check.h"

void __host__ TriaLocalGridIntersection(Triangle4 &tria, int triaId ,std::vector<std::vector<int>> &cellToTriaMap);
void __device__ __host__ TriaBvMinMaxCoord(Triangle4 &tria, float3 BvMinMax[2]);
void __device__ __host__ TriaBvMinMaxId(Triangle4 &tria, float3 gridOrigin, float3 gridSize, float3 BvMinMaxId[2]);
void __device__ __host__ TriaEdge_E0E1E2(Triangle4 &tria, float3 triaEdges[3]);
bool __host__ __device__ CheckOverlap_AabbNormals(float3 translated_vertices[3], const float3 &aabbHalfLength);
bool __host__ __device__ CheckOverlap_GenericAxis(float3 translated_vertices[3], float3 &axis, const float3 &aabbHalfLength);
__global__ void  GPU_triaGridIntersection(Triangle4* tria, int* intersectedCells, float3* BvMinMaxId,
                                          float3* triaEdge_E0E1E2, float3* nxnynz, float3* gridOrigin,
                                          float3* cellSize, int* indexCounter);

//Flatten 3-D array
inline float3 __device__ __host__ CellIdToCoord(int i, int j, int k, float delX, float delY, float delZ,float grid_origin_x, float grid_origin_y, float grid_origin_z) {
    return make_float3((i*delX + 0.5f*delX) + grid_origin_x,
                       (j*delY + 0.5f*delY) + grid_origin_y,
                       (k*delZ + 0.5f*delZ) + grid_origin_z);
}

inline float3 getCellRefFromIndex(int idx,int delX, int delY,int delZ) {
    float3 result;
    int a = (delX * delY);
    result.z = idx / a;
    int b = idx - a * result.z;
    result.y = b / delX;
    result.x = b % delY;
    return result;
}

inline static __device__ __host__ int CellHash(int cellId_x, int cellId_y, int cellId_z, int nx, int ny, int nz) {
    return ((ny*nz) * cellId_x + nz * cellId_y + cellId_z);
	//x + HEIGHT * (y + WIDTH * z)
}

void __host__ GenerateMeshSpatialDecompositionData(std::vector<std::vector<int>> &cellToTriaMap,
                                                   std::vector<unsigned int> &view_triaIds_flat,
                                                   std::vector<unsigned int>   &view_numTriasPerCell,
                                                   std::vector<unsigned int>  &view_cellStart);

#endif //AABB_TRIANGLE_INTERSECTION_TRIANGLE4_UTILS_CUH
