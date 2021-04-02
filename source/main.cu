//Triangle AABB intersection detection based on Separating axis theoram.  
/* Reference: https://gamedev.stackexchange.com/questions/44500/how-many-and-which-axes-to-use-for-3d-obb-collision-with-sat
              https://gamedev.stackexchange.com/questions/118707/zero-vector-test-in-projection-intersection-test-in-sat/118718#118718
              https://bit.ly/2C7bjmM
              http://programmerart.weebly.com/separating-axis-theorem.html
              https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
              https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/aabb-triangle.html
              https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/pubs/tribox.pdf
              http://www.dyn4j.org/2010/01/sat/#sat-proj
              http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt
*/
// prefix: coord = coordinates, vec = vector  

#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include "utils.h"
#include "stl_utils.h"
#include "grid_utils.h"
#include "stl_reader.h"
#include "dem_core.h"
#include "ParticleGenerator.h"
#include "z_hash.h"
#include <bitset>
#include <algorithm>
#include <cuda.h>
#include <cuda_runtime.h>
#include "vector_types.h"
#include "triangle4.h"
#include "params.h"
#include "triangle4_utils.cuh"



using namespace std;



int main() {  

    clock_t tStart_WithGrid = clock();


    Point gridOrigin = Point(GridParam::grid_origin_x, GridParam::grid_origin_y, GridParam::grid_origin_z);
    Grid G1 = Grid(GridParam::delX, GridParam::delY, GridParam::delZ, GridParam::nx, GridParam::ny, GridParam::nz, gridOrigin);
//    G1.GenerateCellMinMaxID();
//    G1.GenerateVTK();
    //Initiating Grid Data - END

    //Assigning minimum tria size tolerance (TODO : used for filtering out triangles which need not be checked for SAT)

    cout << "READING STL MESH - START" << endl;
    stl_reader::StlMesh <float, unsigned int> mesh ("./../../../INPUTS/ball.stl");

    //Initialize Mesh Data
    TriangleMesh mesh_Tria;

    // Reading Data from .stl mesh file into TriangleMesh class
    // Vertices of the triangle (Anti-Clockwise v0,v1,v2)
    vector<float> min;
    vector<float> max;


    vector<Triangle4> trias;
    unsigned int n_trias = 0; //NUMBER OF TRIANGLES IN STL MESH
    
    //TODO: FIND TRIANGLE MESH BOUNDING BOX TO GET GRID MIN MAX
    for(size_t itri = 0; itri < mesh.num_tris(); ++itri) {

        const float* v0 = mesh.tri_corner_coords(itri, 0);
        const float* v1 = mesh.tri_corner_coords(itri, 1);
        const float* v2 = mesh.tri_corner_coords(itri, 2);
        const float* n = mesh.tri_normal(itri);

        Triangle4 tria4;

        tria4.v0 = make_float3(v0[0],v0[1], v0[2]);
        tria4.v1 = make_float3(v1[0],v1[1], v1[2]);
        tria4.v2 = make_float3(v2[0],v2[1], v2[2]);
        tria4.n = make_float3(n[0], n[1], n[2]);

        trias.push_back(tria4);

        //used only for testing portion of the code - from line 111
        Triangle tria = Triangle(Point(v0[0],v0[1],v0[2]), Point(v1[0],v1[1],v1[2]), Point(v2[0],v2[1],v2[2]),itri);
        mesh_Tria.Append(tria, itri);
        n_trias = n_trias + 1;

    }

//This portion of the code is a minimal implementation with linear cell hashing and serves as a primer of GPU implementation.///////////////////////////////
//The outcome is a vector of vectors where each subvector reprecents a cell in the cartesian grid and contains tria ID's belonging to the cell.
    vector<vector<int>> cellToTriaMap(GridParam::nx * GridParam::ny * GridParam::nz);
//    cellToTriaMap.resize(GridParam::nx * GridParam::ny * GridParam::nz);

    cout << "Building cellToTriaMap on CPU ---- START" << endl;
    int triaId = 0;
    for(Triangle4 &tria: trias) {
        TriaLocalGridIntersection(tria, triaId, cellToTriaMap);
        triaId = triaId + 1;
    }
    cout << "Building cellToTriaMap on CPU ---- END" << endl;

    //call GenerateMeshSpatialDecompositionData(.....) here to generate GPU relevant data

//This portion of the code is a minimal implementation nd and serves as a primer of GPU implementation.///////////////////////////////////////////////////


    //The piece of code above is a cleaner implementation.
    // Initial CPU implementation. It is used only for testing purposes and has some limitations.
    //The triangle size is limited such that it should not exceed beyond the 27 cell neighbourhood.
    //Initialize variables for SAT
    vector<double> vec_h;                                                          //absolute half length vector of AABB
    Vector vec_BoxCentre;                                                           //centroid of AABB
    Bbox tria_BV;
    Bbox Bbox_parentCell;                                                           //bounding volume of triangle as Bbox object
    Triangle vec_tria_mov;
    Vector vec_TriaFaceNormal;
    vector<Vector> vec_tria_Edges;
    Vector axis_e_Edge2;
    bool temp_status;
    vector<Vector> vec_es = {Vector(1,0,0), Vector(0,1,0), Vector(0,0,1)};
    double scaling = Vector::Norm(Vector(SMOOTHING_RADIUS, SMOOTHING_RADIUS, SMOOTHING_RADIUS));

    clock_t tStart = clock();


    for(auto const &triaIdPair: mesh_Tria.GetMesh()) {

        const Triangle &tria = triaIdPair.second;


        //PRECOMPUTING TRIANGLE NORMAL AND EDGES - START (Triangle translation during SAT has no impact on the edge vector. Edge does not have to be recomputed for each Tria Box intersection)
        vec_TriaFaceNormal = tria.GetNormal(tria);

        vec_tria_Edges.push_back(tria.GetEdge(tria, 0));
        vec_tria_Edges.push_back(tria.GetEdge(tria, 1));
        vec_tria_Edges.push_back(tria.GetEdge(tria, 2));
        //PRECOMPUTING TRIANGLE NORMAL AND EDGES - END

        //Parent cell represents the cell which contains the centroid of the tria being tested for intersection.
        vector<int> parentCellId = tria.GetParentCellByCentroid(gridOrigin, G1.GetXstep(), G1.GetYstep(), G1.GetZstep(), tria);
        Bbox_parentCell = Bbox(parentCellId, G1.GetGridOrigin(), G1.GetXstep(), G1.GetYstep(), G1.GetZstep());
        std::ostringstream filename;
        filename << "./../../../../OUTPUTS/PARENT_CELL/TEST_PARENT_CELL" << parentCellId[0] << parentCellId[1] << parentCellId[2] << ".stl";
        Bbox_parentCell.Create_box_stl(filename.str(), Bbox_parentCell.GetMin(), Bbox_parentCell.GetMax());

        //Append parent cell ID with this triangle (If triangle centroid is inside the parent cell then there is definite overlap)
        G1.InsertToCellTriaMap(parentCellId, tria);
        //

        //Fetching neighbours to parent cell
        vector<vector<int>> neighbours = G1.GetNeighbours(parentCellId);

        // Broad phase collision check followed by narrow phase for neighbours of parent cell
        tria_BV = tria.BoundingVolume(tria);
        tria_BV.Create_box_stl("./../../../OUTPUTS/TEST_TRIA_BV.stl", tria_BV.GetMin(), tria_BV.GetMax());
        Bbox tmp_BboxNeighbour;

        // Single Triangle V/s all neighbours checked using bounding volume - START

        int i = 0;
        for(auto &neighbour: neighbours) {

            std::ostringstream filename_NonExtended;

            filename_NonExtended << "./../../../OUTPUTS/NONEXTENDED_BOX/TEST_NONEXTENDED_BOX_" << parentCellId[0] << " " << parentCellId[1] << " " << parentCellId[2] << "_" <<  neighbour[0] << neighbour[1] << neighbour[2] << ".stl";
            tmp_BboxNeighbour = Bbox(neighbour, G1.GetGridOrigin(), G1.GetXstep(), G1.GetYstep(), G1.GetZstep());
            tmp_BboxNeighbour.Create_box_stl(filename_NonExtended.str(), tmp_BboxNeighbour.GetMin(), tmp_BboxNeighbour.GetMax());


            std::ostringstream filename;
            filename << "./../../../OUTPUTS/EXTENDED_BOX/TEST_EXTENDED_BOX_" << neighbour[0] << neighbour[1] << neighbour[2] << ".stl";
            tmp_BboxNeighbour = tmp_BboxNeighbour.ExtendBox(tmp_BboxNeighbour, scaling);
            tmp_BboxNeighbour.Create_box_stl(filename.str(), tmp_BboxNeighbour.GetMin(), tmp_BboxNeighbour.GetMax());
            i++;

            // Can be optimized more by consdering only those cells which lie within the circumsphere of the triangle

            // Checking if bounding volume of Triangle overlaps with expanded cell. If not overlapping continue to next neighbour else perform SAT TEST
            if(tmp_BboxNeighbour.IsOverlappingBbox(tmp_BboxNeighbour, tria_BV)) {

                //Starting SAT test
                //Compute the half length vector of expanded cell. (vec_h has absolute values)
                vec_h = tmp_BboxNeighbour.ComputeHalfLength(tmp_BboxNeighbour);

                //Compute centroid(Required to move box to centre and triangle relative to box)
                vec_BoxCentre = Vector(tmp_BboxNeighbour.GetCentroid().GetX(),tmp_BboxNeighbour.GetCentroid().GetY(), tmp_BboxNeighbour.GetCentroid().GetZ());

                // Translate triangle as relative to AABB moved to origin(0,0,0) (Translation is done towards origin)
                vec_tria_mov = tria.Translate(tria, vec_BoxCentre);

                // Individual Triagle V/s Individual box - START
                // Compute test axes and check for overlap
                // 9 Text axes from edge cross product
                int k=0;
                for(auto &vec_e: vec_es) {

                    for(auto &vec_tria_Edge2 : vec_tria_Edges) {

                        // (e0 X Edge0)=0,(e0 X Edge1)=1,(e0 X Edge2) = 2, ........
                        axis_e_Edge2 = Vector::CrossProduct(vec_e,vec_tria_Edge2);

                        /*
                        Overlap test

                        Axis normalization not necessary since both
                        triangle vertices and AABB half length are
                        dotted with axis without normalization
                        */

                        temp_status = PerformOverlapTest2(vec_tria_mov,axis_e_Edge2,vec_h);

                        if(!temp_status) {

                            if(!SILENT_OVERLAP) {
                                cout << "NO OVERLAP ON CROSS PRODUCT AXIS_" << k << endl;
                            }

                            break;

                        }


                        k++;

                    }

                    if(!temp_status) {

                        break;

                    }

                }

                if(!temp_status) {

                    //No overlap of current triangle with current neighbour. Continue to next neighbour of same triangle. If neigbhour being
                    // checked is the last neighbour then go to the next triangle (Handled by foreach loop)
                    continue;

                }

                // Test axes corresponding to the face normals of AABB
                // X,Y,Z-Axis
                if(!(PerformOverlapTest2_AABBnormals(vec_tria_mov, vec_h))) {

                    if(!SILENT_OVERLAP) {
                        cout << "NO OVERLAP ON AABB Axis of cell " << endl;
                    }

                    continue;

                }

                // Test Triangle face normal (AABB Plane intersection)
                temp_status = PerformOverlapTest2(vec_tria_mov,vec_TriaFaceNormal,vec_h);

                if(!temp_status) {

                    if(!SILENT_OVERLAP) {
                        cout << "NO OVERLAP ON AABB Axis of cell " << endl;
                    }

                    continue;

                }
                if(!SILENT_OVERLAP) {
                    cout << "OVERLAP FOUND" << endl;
                }

                //Append neighbour cell ID with this triangle
                G1.InsertToCellTriaMap(neighbour, tria);
                //

                // Individual Triagle V/s Individual box - END
            }

        }

    }


    cout << "STARTING PARTICLE GENERATION" << endl;
    clock_t particle_generation_clock = clock();
    ParticleGenerator particles = ParticleGenerator(MASS_PARTICLE, SMOOTHING_RADIUS);
    particles.CreateParticlesByBbox(0.5,1,1,2.5,2.3,2,particles.particle_radius_,0.01);
    map<int,FluidParticle> particleIdMap = particles.GetParticles();
    printf("Time taken for particle generation: %.2fs\n", (double)(clock() - particle_generation_clock)/CLOCKS_PER_SEC);

    cout << "STARTING DEM CORE" <<endl;
    clock_t DEM_clock = clock();
    dem_core environment = dem_core(particleIdMap, G1.GetCellTriaMap(),G1, 10000, 1000);
    environment.StartSim(10000, 0.001);
    printf("Time taken for simulation: %.2fs\n", (double)(clock() - DEM_clock)/CLOCKS_PER_SEC);


}
