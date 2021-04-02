//
// Created by Shriram on 11/9/20.
//

#include "triangle4_utils.cuh"


using namespace CudaMathUtils;

//Gives the min and max bounding volume coordinate of a triangle. Coordinate values are held in
//a float3 array
void __device__ __host__ TriaBvMinMaxCoord(Triangle4 &tria, float3 BvMinMax[2]) {

    BvMinMax[0].x = fminf(fminf(tria.v0.x, tria.v1.x), tria.v2.x);
    BvMinMax[0].y = fminf(fminf(tria.v0.y, tria.v1.y), tria.v2.y);
    BvMinMax[0].z = fminf(fminf(tria.v0.z, tria.v1.z), tria.v2.z);

    BvMinMax[1].x = fmaxf(fmaxf(tria.v0.x, tria.v1.x), tria.v2.x);
    BvMinMax[1].y = fmaxf(fmaxf(tria.v0.y, tria.v1.y), tria.v2.y);
    BvMinMax[1].z = fmaxf(fmaxf(tria.v0.z, tria.v1.z), tria.v2.z);

}

//Gives the gird index values start and end cell of tria BV
void __device__ __host__ TriaBvMinMaxId(Triangle4 &tria, float3 gridOrigin, float3 gridSize, float3 BvMinMaxId[2]) {

    float3 BvMinMax[2];
    TriaBvMinMaxCoord(tria, BvMinMax);

    float3 min_moved = make_float3(BvMinMax[0].x - gridOrigin.x, BvMinMax[0].y - gridOrigin.y, BvMinMax[0].z - gridOrigin.z);
    float3 max_moved = make_float3(BvMinMax[1].x - gridOrigin.x, BvMinMax[1].y - gridOrigin.y, BvMinMax[1].z - gridOrigin.z);

    BvMinMaxId[0] = make_float3(int(min_moved.x/gridSize.x), int(min_moved.y/gridSize.y), int(min_moved.z/gridSize.z));

    BvMinMaxId[1] = make_float3(int(max_moved.x/gridSize.x), int(max_moved.y/gridSize.y), int(max_moved.z/gridSize.z));;

}

void __device__ __host__ TriaEdge_E0E1E2(Triangle4 &tria, float3 triaEdges[3]) {

    triaEdges[0] = make_float3(tria.v1.x - tria.v0.x, tria.v1.y - tria.v0.y, tria.v1.z - tria.v0.z);
    triaEdges[1] = make_float3(tria.v2.x - tria.v1.x, tria.v2.y - tria.v1.y, tria.v2.z - tria.v1.z);
    triaEdges[2] = make_float3(tria.v0.x - tria.v2.x, tria.v0.y - tria.v2.y, tria.v0.z - tria.v2.z);

}

bool __host__ __device__ CheckOverlap_GenericAxis(float3 translated_vertices[3], float3 &axis, const float3 &aabbHalfLength) {

    if(CudaMathUtils::Norm(axis) == 0) {

        return true;                // Check for overlap not necessary if cross product results in 0 vector

    }

    float p0 = CudaMathUtils::Dot(translated_vertices[0], axis);

    float p1 = CudaMathUtils::Dot(translated_vertices[1], axis);

    float p2 = CudaMathUtils::Dot(translated_vertices[2], axis);

    float max = fmaxf(p0, fmaxf(p1,p2));

    float min = fminf(p0, fminf(p1,p2));

    // Computing radius of AABB projected on testing axis
    float r = (aabbHalfLength.x)*abs(axis.x)+ (aabbHalfLength.y)*abs(axis.y) + (aabbHalfLength.z)*abs(axis.z);

    // Check overlap
    if(min > r || max < -r) {
        // Min and Max projected vertex of triangle lie outside projected half length of AABB
        return false;
    }

    return true;

}

bool __host__ __device__ CheckOverlap_AabbNormals(float3 translated_vertices[3], const float3 &aabbHalfLength){
    
    float max = fmaxf(translated_vertices[0].x, fmaxf(translated_vertices[1].x,translated_vertices[2].x));
    float min = fminf(translated_vertices[0].x, fminf(translated_vertices[1].x,translated_vertices[2].x));

    if(min > aabbHalfLength.x || max < -aabbHalfLength.x) {
        return false;                                              
    } 

    max = fmaxf(translated_vertices[0].y, fmaxf(translated_vertices[1].y,translated_vertices[2].y));
    min = fminf(translated_vertices[0].y, fminf(translated_vertices[1].y,translated_vertices[2].y));

    if(min > aabbHalfLength.y || max < -aabbHalfLength.y) {
        return false;                                              
    }

    max = fmaxf(translated_vertices[0].z, fmaxf(translated_vertices[1].z,translated_vertices[2].z));
    min = fminf(translated_vertices[0].z, fminf(translated_vertices[1].z,translated_vertices[2].z));

    if(min > aabbHalfLength.z || max < -aabbHalfLength.z) {
        return false;                                              
    }


    return true;

}

__global__ void  GPU_triaGridIntersection(Triangle4* tria, int* intersectedCells, float3* BvMinMaxId,
                                          float3* triaEdge_E0E1E2, float3* nxnynz, float3* gridOrigin,
                                          float3* cellSize, int* indexCounter) {

    bool temp_status = false;
//    __shared__ Triangle4 shared_tria = *tria;
    Triangle4 local_tria = *tria;
    float3 local_BvMinId = BvMinMaxId[0];
    float3 local_BvMaxId = BvMinMaxId[1];
    float3 local_triaEdge_E0E1E2[3] = {triaEdge_E0E1E2[0], triaEdge_E0E1E2[1], triaEdge_E0E1E2[2]};
    float3 local_e[3] = {make_float3(1,0,0), make_float3(0,1,0), make_float3(0,0,1)};
    float3 local_cellSize = *cellSize;
    float3 local_aabbHalfLength = make_float3(local_cellSize.x/2.0f, local_cellSize.y/2.0f, local_cellSize.z/2.0f );


    //Global Thread ID
    unsigned int tid_x =  blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int tid_y =  blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int tid_z =  blockIdx.z * blockDim.z + threadIdx.z;
    int arrIndex = CellHash(tid_x, tid_y, tid_z, GridParam::nx, GridParam::ny, GridParam::nz);
    intersectedCells[arrIndex] = -1;
    printf("%d",arrIndex);

    unsigned int cellId_x = tid_x + local_BvMinId.x;
    unsigned int cellId_y = tid_y + local_BvMinId.y;
    unsigned int cellId_z = tid_z + local_BvMinId.z;

    unsigned int stride_x = blockDim.x * gridDim.x;
    unsigned int stride_y = blockDim.y * gridDim.y;
    unsigned int stride_z = blockDim.z * gridDim.z;

    unsigned int index;

//    printf("Minmax %f, %f, %f\n", BvMinMaxId[0].x, BvMinMaxId[0].y, BvMinMaxId[0].z);
    printf("GLOBAL THREAD ID : %d, %d, %d GLOBAL CELL ID : %d, %d, %d\n",tid_x, tid_y, tid_z, cellId_x, cellId_y, cellId_z);
    for(int i = cellId_z; i <=local_BvMaxId.z; i += stride_z) {
        for(int j = cellId_y; j <= local_BvMaxId.y; j += stride_y) {
            for(int k = cellId_x; k <= local_BvMaxId.x; k += stride_x) {

//                printf("GLOBAL THREAD ID : %d, %d, %d GLOBAL CELL ID : %d, %d, %d i j k : %d %d %d\n",tid_x, tid_y, tid_z, cellId_x, cellId_y, cellId_z, k, j, i);
                float3 cellCentroid = CellIdToCoord(k, j, i, local_cellSize.x, local_cellSize.y, local_cellSize.z, gridOrigin->x, gridOrigin->y, gridOrigin->z);
//                printf("cellCentroid %f %f %f\n", cellCentroid.x, cellCentroid.y, cellCentroid.z);
                float3 translationVector = -cellCentroid;              //float3 vector used to translate triangle relative to cell centroid being moved to (0,0,0)
//                printf("%d %d %d translation vector %f %f %f\n",tid_x, tid_y, tid_z, translationVector.x, translationVector.y, translationVector.z);

                //Translate tria
                float3 vertex_translated[3];
                vertex_translated[0] = local_tria.v0 + translationVector;
                vertex_translated[1] = local_tria.v1 + translationVector;
                vertex_translated[2] = local_tria.v2 + translationVector;
//                printf("translated vertices %f %f %f, %f %f %f, %f %f %f\n",  vertex_translated[0].x, vertex_translated[0].y, vertex_translated[0].z,
//                                                                            vertex_translated[1].x, vertex_translated[1].y, vertex_translated[1].z,
//                                                                            vertex_translated[2].x, vertex_translated[2].y, vertex_translated[2].z);
//
                for(int cross_i = 0; cross_i < 3; cross_i++) {

                    for(int cross_j = 0; cross_j < 3; cross_j++) {

                        float3 axis = CudaMathUtils::Cross(local_e[cross_i], local_triaEdge_E0E1E2[cross_j]);
//                        printf("%d %d %d : axis = (e X Edge) => %f %f %f X %f %f %f = %f %f %f\n",  tid_x, tid_y, tid_z, local_e[cross_i].x, local_e[cross_i].y, local_e[cross_i].z,
//                                                                                                    local_triaEdge_E0E1E2[cross_j].x, local_triaEdge_E0E1E2[cross_j].y, local_triaEdge_E0E1E2[cross_j].z,
//                                                                                                    axis.x, axis.y, axis.z);

                        /*
                        Overlap test

                        Axis normalization not necessary since both
                        triangle vertices and AABB half length are
                        dotted with axis without normalization
                        */
                        temp_status = CheckOverlap_GenericAxis(vertex_translated, axis, local_aabbHalfLength);
//
                        if( !temp_status) {
                            break;
                        }

                    }

                    if(!temp_status) {
                        break;
                    }

                }

                if(!temp_status) {
                    continue;
                }

                // Test axes corresponding to the face normals of AABB
                // X,Y,Z-Axis
                if(!(CheckOverlap_AabbNormals(vertex_translated, local_aabbHalfLength))) {
                    continue;
                }

                // Test Triangle face normal (AABB Plane intersection)
                if(!(CheckOverlap_GenericAxis(vertex_translated, local_tria.n, local_aabbHalfLength))) {
                    continue;
                }

                //All tests for current tria passed. Add ID to cell tria map
                int cellIndex = CellHash(k, j, i, GridParam::nx, GridParam::ny, GridParam::nz);

//                index = atomicAdd(indexCounter, 1);
//                printf("INDEX COUNTER %d\n", index);
//                intersectedCells[arrIndex] = cellIndex; // change to intersectedCells[hash from thread id] = [hash from cell id];
//                printf("%d %d %d : %d\n", cellId_x, cellId_y, cellId_z, cellIndex);

            }
        }
    }

    //compute cell index from tid
}






//TODO: MAKE THIS A KERNEL AND LAUNCH A THREAD PER TRIANGLE IF n_CELLS < n_TRIAS
//checks the passed tria against all the cells for intersection in the grid.
void __host__ TriaLocalGridIntersection(Triangle4 &tria, int triaId ,std::vector<std::vector<int>> &cellToTriaMap) {
    std::cout << "NEW TRIA : " << triaId << std::endl;
    bool temp_status = false;

    float3 BvMinMaxId[2];

    //get local cell start and end ID based on triangle bounding volume
    TriaBvMinMaxId(tria, make_float3(GridParam::grid_origin_x, GridParam::grid_origin_y, GridParam::grid_origin_z), make_float3(GridParam::delX, GridParam::delY, GridParam::delZ), BvMinMaxId);

    //Launch Cell intersection on CPU or GPU based on number of cells to test(TODO: Profile code to get optimum number of cells)

    //If min and max of tri BV is within one cell append cell ID to cellToTriaMap and return. SAT not necessary.
    if(CudaMathUtils::equalTo(BvMinMaxId[0], BvMinMaxId[1])) {
        int cellIndex = CellHash(BvMinMaxId[0].x, BvMinMaxId[0].y, BvMinMaxId[0].z, GridParam::nx, GridParam::ny, GridParam::nz);
        cellToTriaMap[cellIndex].push_back(triaId);
        std::cout << "Kernel launch not required. Tria belongs to single cell : " << triaId << std::endl;
        return;
    }

    float3 triaEdges_E0E1E2[3];
    TriaEdge_E0E1E2(tria, triaEdges_E0E1E2);

    unsigned int n_cellCheckCount = (BvMinMaxId[1].x - BvMinMaxId[0].x + 1) * (BvMinMaxId[1].y - BvMinMaxId[0].y + 1) * (BvMinMaxId[1].z - BvMinMaxId[0].z + 1);

//    GPU(If number of cells to be checked exceeds threshold)
//TODO: Needs testing
    if(n_cellCheckCount >= 1) {

        float3* d_nxnynz;
        const unsigned int NUM_CELLS_BYTES = sizeof(d_nxnynz);
        CudaSafeCall(cudaMallocManaged(&d_nxnynz, NUM_CELLS_BYTES));
        d_nxnynz->x = GridParam::nx; d_nxnynz->y = GridParam::ny; d_nxnynz->z = GridParam::nz;

        float3* d_gridOrigin;
        const unsigned int GRID_ORIGIN_BYTES = sizeof(d_gridOrigin);
        CudaSafeCall(cudaMallocManaged(&d_gridOrigin, GRID_ORIGIN_BYTES));
        d_gridOrigin->x = GridParam::grid_origin_x; d_gridOrigin->y = GridParam::grid_origin_y; d_gridOrigin->z = GridParam::grid_origin_z;

        float3* d_cellSize;
        const unsigned int CELL_SIZE_BYTES = sizeof(d_cellSize);
        CudaSafeCall(cudaMallocManaged(&d_cellSize, CELL_SIZE_BYTES));
        d_cellSize->x = GridParam::delX; d_cellSize->y = GridParam::delY; d_cellSize->z = GridParam::delZ;

        float3* d_triaEdge_E0E1E2;
        const unsigned int EDGE_ARRAY_BYTES = sizeof(triaEdges_E0E1E2);
        CudaSafeCall(cudaMalloc(&d_triaEdge_E0E1E2, EDGE_ARRAY_BYTES));
        CudaSafeCall(cudaMemcpy(d_triaEdge_E0E1E2, &triaEdges_E0E1E2, EDGE_ARRAY_BYTES, cudaMemcpyHostToDevice));

        int* d_intersectedCells;
        const unsigned int CELL_COUNT = n_cellCheckCount;
        const unsigned int ARRAY_BYTES = CELL_COUNT * sizeof(int*);
        CudaSafeCall(cudaMallocManaged(&d_intersectedCells, ARRAY_BYTES));
//        CudaSafeCall(cudaMemset(d_intersectedCells, -1, ARRAY_BYTES));

        Triangle4* d_tria;
        const unsigned int TRIA_BYTES = sizeof(Triangle4);
        CudaSafeCall(cudaMalloc(&d_tria, TRIA_BYTES));
        CudaSafeCall(cudaMemcpy(d_tria, &tria, TRIA_BYTES, cudaMemcpyHostToDevice));


        float3* d_BvMinMaxId;
        const unsigned int MINMAX_ARRAY_BYTES = sizeof(BvMinMaxId);
        CudaSafeCall(cudaMallocManaged(&d_BvMinMaxId, MINMAX_ARRAY_BYTES));
        d_BvMinMaxId[0].x = BvMinMaxId[0].x; d_BvMinMaxId[0].y = BvMinMaxId[0].y; d_BvMinMaxId[0].z = BvMinMaxId[0].z;
        d_BvMinMaxId[1].x = BvMinMaxId[1].x; d_BvMinMaxId[1].y = BvMinMaxId[1].y; d_BvMinMaxId[1].z = BvMinMaxId[1].z;

        unsigned int n_threads = n_cellCheckCount;

        int* d_indexCounter;
        CudaSafeCall(cudaMallocManaged(&d_indexCounter, sizeof(int)));
        *d_indexCounter = -1;

        int n_threads_x = BvMinMaxId[1].x - BvMinMaxId[0].x + 1;
        int n_threads_y = BvMinMaxId[1].y - BvMinMaxId[0].y + 1;
        int n_threads_z = BvMinMaxId[1].z - BvMinMaxId[0].z + 1;

        dim3 threadBlock(8,8,8); //make round multiple of warp size wiz 32.
        dim3 grid((n_threads_x+threadBlock.x-1)/threadBlock.x, (n_threads_y+threadBlock.y-1)/threadBlock.y, (n_threads_z+threadBlock.z-1)/threadBlock.z);

//        int blockSize;      // The launch configurator returned block size
//        int minGridSize;    // The minimum grid size needed to achieve the maximum occupancy for a full device launch
//        int gridSize;       // The actual grid size needed, based on input size
//        cudaOccupancyMaxPotentialBlockSize(&minGridSize, &blockSize, GPU_triaGridIntersection, 0, n_cellCheckCount);
//        gridSize = (n_cellCheckCount + blockSize - 1) / blockSize;

        GPU_triaGridIntersection<<<dim3(1,1,1), dim3(1,1,1)>>>(d_tria, d_intersectedCells, d_BvMinMaxId, d_triaEdge_E0E1E2, d_nxnynz, d_gridOrigin, d_cellSize, d_indexCounter);
        cudaDeviceSynchronize();

        printf("%d %d", d_intersectedCells[0], d_intersectedCells[1]);

        for(int i = 0; i < CELL_COUNT; i++) {
            if(d_intersectedCells[i] == -1) {
                continue;
            }
            cellToTriaMap[d_intersectedCells[i]].push_back(triaId);
        }

        CudaSafeCall(cudaFree(d_tria));
        CudaSafeCall(cudaFree(d_intersectedCells));
        CudaSafeCall(cudaFree(d_BvMinMaxId));
        CudaSafeCall(cudaFree(d_triaEdge_E0E1E2));
        CudaSafeCall(cudaFree(d_nxnynz));
        CudaSafeCall(cudaFree(d_gridOrigin));
        CudaSafeCall(cudaFree(d_cellSize));
        CudaSafeCall(cudaFree(d_indexCounter));
    }

    //CPU tria intersection
    for(int i = BvMinMaxId[0].z; i <= BvMinMaxId[1].z; i++) {
        for(int j = BvMinMaxId[0].y; j <= BvMinMaxId[1].y; j++) {
            for(int k = BvMinMaxId[0].x; k <= BvMinMaxId[1].x; k++) {

                float3 cellCentroid = CellIdToCoord(k, j, i, GridParam::delX, GridParam::delY, GridParam::delZ, GridParam::grid_origin_x, GridParam::grid_origin_y, GridParam::grid_origin_z);
                float3 translationVector = -cellCentroid;              //float3 vector used to translate triangle relative to cell centroid being moved to (0,0,0)

                //Translate tria
                float3 vertex_translated[3];
                vertex_translated[0] = tria.v0 + translationVector;
                vertex_translated[1] = tria.v1 + translationVector;
                vertex_translated[2] = tria.v2 + translationVector;

                
                for(int cross_i = 0; cross_i < 3; cross_i++) {

                    for(int cross_j = 0; cross_j < 3; cross_j++) {

                        float3 axis = CudaMathUtils::Cross(SatParam::e[cross_i], triaEdges_E0E1E2[cross_j]);

                        /*
                        Overlap test

                        Axis normalization not necessary since both
                        triangle vertices and AABB half length are
                        dotted with axis without normalization
                        */
                        temp_status = CheckOverlap_GenericAxis(vertex_translated, axis, SatParam::aabbHalfLength);

                        if( !temp_status) {
                            break;
                        }

                    }

                    if(!temp_status) {
                        break;
                    }

                }

                if(!temp_status) {
                    continue;
                }

                // Test axes corresponding to the face normals of AABB
                // X,Y,Z-Axis
                if(!(CheckOverlap_AabbNormals(vertex_translated, SatParam::aabbHalfLength))) {
                    continue;
                }

                // Test Triangle face normal (AABB Plane intersection)
                if(!(CheckOverlap_GenericAxis(vertex_translated, tria.n, SatParam::aabbHalfLength))) {
                    continue;
                }

                //All tests for current tria passed. Add ID to cell tria map
                int cellIndex = CellHash(k, j, i, GridParam::nx, GridParam::ny, GridParam::nz);
                std::cout << k << " " << j << " " << i << " : " << cellIndex << std::endl;
//                cellToTriaMap[cellIndex].push_back(triaId);
            }
        }

    }

    return;
}

//Generates spatial decomposition data for input stl mesh in a form viable for GPU usage.
void __host__ GenerateMeshSpatialDecompositionData(std::vector<std::vector<int>> &cellToTriaMap,
                                                   std::vector<unsigned int> &view_triaIds_flat,
                                                   std::vector<unsigned int>   &view_numTriasPerCell,
                                                   std::vector<unsigned int>  &view_cellStart) {

    unsigned int start = 0;
    unsigned int tria_count = 0;
    unsigned int count = 0;
    for(auto cell : cellToTriaMap) {

        unsigned int offset = 0;
        for(unsigned int triaId: cell) {

//            triaIds_flat.push_back(triaId);
            view_triaIds_flat[tria_count] = triaId;

            tria_count++;
            offset++;
        }
//        numTriasPerCell.push_back(offset);
        view_numTriasPerCell[count] = offset;

//        cellStart.push_back(start);
        view_cellStart[count] = start;
        count++;
        start = start + offset;
    }

}