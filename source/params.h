//
// Created by shriram on 11/11/20.
//

#ifndef PARAMS_H
#define PARAMS_H

#include <cuda.h>
#include <cuda_runtime.h>
#include <vector_types.h>

const double EPSILON = 0.0000001;
const  int SILENT = 1;
const  int SILENT_OVERLAP = 1;
const  double SMOOTHING_RADIUS = 0.1;
const  double MASS_PARTICLE = 0.2;

namespace GridParam {
    //Initialize Grid Data - START
    const float grid_origin_x = 0; //domain (or) grid origin x coordinate
    const float grid_origin_y = 0; //domain (or) grid origin y coordinate
    const float grid_origin_z = 0; //domain (or) grid origin z coordinate
    const float delX = 1; //cell size along x
    const float delY = 1; //cell size along y
    const float delZ = 1; //cell size along z
    const float domainX = 600; //absolute domain x length
    const float domainY = 600; //absolute domain y length
    const float domainZ = 1000; //absolute domain z length
    const int nx = (int) ((domainX / delX) + 1);
    const int ny = (int) ((domainY / delY) + 1);
    const int nz = (int) ((domainZ / delZ) + 1);
}


namespace SatParam {

    const float3 e[3] = {make_float3(1,0,0), make_float3(0,1,0), make_float3(0,0,1)};
 
    const float3 aabbHalfLength = make_float3(GridParam::delX/2.0f, GridParam::delY/2.0f, GridParam::delZ/2.0f );

}


#endif //PARAMS_H
