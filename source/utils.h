#ifndef UTILS_H_INCLUDE
#define UTILS_H_INCLUDE
#include <iostream>
#include <math.h> 
#include "grid_utils.h"

//function prototypes  
double Dot(double vec_1[3], double vec_2[3]);  
void CrossProduct(double vec_1[3], double vec_2[3], double vec_Res[]);  
void SubtractVecs(double vec_1[3], double vec_2[3], double vec_Res[]);  
double FindMin(double p0,double p1,double p2);  
double FindMax(double p0,double p1,double p2);  
bool PerformOverlapTest( double vert0[3],double vert1[3],double vert2[3], double axis[3], double vec_h[3]);
bool PerformOverlapTest2(Triangle &tria, Vector &axis, vector<double> &vec_h);  
void CentroidAABB(double vec_AABBvertex[8][3], double vec_AABBcentroid[3]); 
void AddVecs(double vec_1[3], double vec_2[3], double vec_Res[]); 
double VecNorm(double vec[3]);
void ComputeBoxVertex(double vec_min[3], double vec_max[3], double vec_vertices[8][3]);
void ComputeHalfLength(double vec_min[3], double vec_max[3], double vec_result[3]);
bool PerformOverlapTest2_AABBnormals(Triangle &tria, vector<double> &vec_h);

#endif