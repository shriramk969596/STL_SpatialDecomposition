//
//Shriram(29.10.20)
//
#ifndef TRIANGLE4_H
#define TRIANGLE4_H

#include <cuda.h>
#include <cuda_runtime.h>
#include <vector_types.h>





struct Triangle4{

        //Vertices defined anti-clockwise
        float3 v0;
        float3 v1;
        float3 v2;

        //Normal
        float3 n; 

};



#endif //TRIANGLE4_H