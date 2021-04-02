#ifndef STL_UTILS_H_INCLUDE
#define STL_UTILS_H_INCLUDE
#include <iostream>
#include <fstream> 
#include <boost/filesystem.hpp>
#include <CL/cl.h> 
#include <CL/cl_platform.h>  

void Create_box_stl(std::string map_filename, double domain_min[3], double domain_max[3]);
void Create_tria_stl(std::string map_filename, double v0[3], double v1[3], double v2[3], double facet_normal[3]);

#endif