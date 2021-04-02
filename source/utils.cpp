#include "utils.h"
using namespace std;

//Compute Dot product of 2 vectors  
double Dot(double vec_1[3], double vec_2[3]) {   

    double product = vec_1[0]*vec_2[0]+vec_1[1]*vec_2[1]+vec_1[2]*vec_2[2];  

    return product;   

}  

//Subtract two vectors  
void SubtractVecs(double vec_1[3], double vec_2[3], double vec_Res[3]) {  

    vec_Res[0] = vec_1[0] - vec_2[0];   

    vec_Res[1] = vec_1[1] - vec_2[1];    

    vec_Res[2] = vec_1[2] - vec_2[2];   

}    

//Add two vectors  
void AddVecs(double vec_1[3], double vec_2[3], double vec_Res[]) {  

    vec_Res[0] = vec_1[0] + vec_2[0];   

    vec_Res[1] = vec_1[1] + vec_2[1];    

    vec_Res[2] = vec_1[2] + vec_2[2];   

}  

//Compute cross product of 2 vectors 
void CrossProduct(double vec_1[3], double vec_2[3], double vec_Res[3]) {   

    vec_Res[0] = vec_1[1] * vec_2[2] - vec_1[2] * vec_2[1];   

    vec_Res[1] = vec_1[2] * vec_2[0] - vec_1[0] * vec_2[2];   

    vec_Res[2] = vec_1[0] * vec_2[1] - vec_1[1] * vec_2[0];   

}  

//Perform SAT test 
bool PerformOverlapTest( double vert0[3],double vert1[3],double vert2[3], double axis[3], double vec_h[3]) {  

    if(VecNorm(axis) == 0) {

        return true;                // Check for overlap not ncessary if cross product results in 0 vector

    }
    
    double p0 = Dot(vert0,axis);  

    double p1 = Dot(vert1,axis);  

    double p2 = Dot(vert2,axis);  

    double max = FindMax(p0,p1,p2);  

    double min = FindMin(p0,p1,p2);  

    // Computing radius of AABB projected on testing axis  
    double r = vec_h[0]*abs(axis[0])+vec_h[1]*abs(axis[1])+vec_h[2]*abs(axis[2]);

    // Check overlap
    if(min > r || max < -r) {
                                        // Min and Max projected vertex of traingle lie outside 
        return false;                   // projected half length of AABB            
                                           
    }                                   

    return true; 

}  


bool PerformOverlapTest2(Triangle &tria, Vector &axis, vector<double> &vec_h) {  

    if(Vector::Norm(axis) == 0) {

        return true;                // Check for overlap not ncessary if cross product results in 0 vector

    }
    
    double p0 = Vector::DotProduct(tria.Get_V0_Vector(tria),axis);  

    double p1 = Vector::DotProduct(tria.Get_V1_Vector(tria),axis);  

    double p2 = Vector::DotProduct(tria.Get_V2_Vector(tria),axis);  

    double max = FindMax(p0,p1,p2);  

    double min = FindMin(p0,p1,p2);  

    // Computing radius of AABB projected on testing axis  
    double r = vec_h[0]*abs(axis.GetX())+vec_h[1]*abs(axis.GetY())+vec_h[2]*abs(axis.GetZ());

    // Check overlap
    if(min > r || max < -r) {
                                        // Min and Max projected vertex of traingle lie outside 
        return false;                   // projected half length of AABB            
                                           
    }                                   

    return true; 

}

bool PerformOverlapTest2_AABBnormals(Triangle &tria, vector<double> &vec_h) {
    
    vector<vector<double> > vertices = {{tria.Get_V0(tria).GetX(), tria.Get_V0(tria).GetY(), tria.Get_V0(tria).GetZ()}, 
                                       {tria.Get_V1(tria).GetX(), tria.Get_V1(tria).GetY(), tria.Get_V1(tria).GetZ() }, 
                                       {tria.Get_V2(tria).GetX(), tria.Get_V2(tria).GetY(), tria.Get_V2(tria).GetZ() } };

    for(int i=0; i < 3; i++) {

        double max = FindMax(vertices[0][i],vertices[1][i],vertices[2][i]);
        double min = FindMin(vertices[0][i],vertices[1][i],vertices[2][i]);

        if(min > vec_h[i] || max < -vec_h[i]) {
             // Min and Max projected vertex of traingle lie outside
            return false;
        } 

    }

    return true;

}




// Compute the minimum of projected vertices  
double FindMin(double p0,double p1,double p2) {  

    return std::min(std::min(p0, p1), p2);  

}    

//Compute the maximum of projected vertices  
double FindMax(double p0,double p1,double p2) {  

    return std::max(std::max(p0, p1), p2);  

}  

//Computes centroid of AABB 
void CentroidAABB(double vec_AABBvertex[8][3], double vec_AABBcentroid[3]) { 

    for(int i=0; i < 8; i++ ) { 

        AddVecs(vec_AABBcentroid,vec_AABBvertex[i],vec_AABBcentroid); 

    } 


    for(int i=0; i < 3; i++) { 

        vec_AABBcentroid[i] = vec_AABBcentroid[i]/8; 

    } 

} 


//Compute vector norm 
double VecNorm(double vec[3]) {
    double norm = sqrt((vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]));
    return norm; 

} 

//Retrieve AABB vertices from min max data 
void ComputeBoxVertex(double vec_min[3], double vec_max[3], double vec_vertices[8][3]) {
   
        vec_vertices[0][0] = vec_min[0];
        vec_vertices[0][1] = vec_min[1];
        vec_vertices[0][2] = vec_min[2];

        vec_vertices[1][0] = vec_max[0];
        vec_vertices[1][1] = vec_min[1];
        vec_vertices[1][2] = vec_min[2];

        vec_vertices[2][0] = vec_max[0];
        vec_vertices[2][1] = vec_max[1];
        vec_vertices[2][2] = vec_min[2];

        vec_vertices[3][0] = vec_min[0];
        vec_vertices[3][1] = vec_max[1];
        vec_vertices[3][2] = vec_min[2];

        vec_vertices[4][0] = vec_min[0];
        vec_vertices[4][1] = vec_min[1];
        vec_vertices[4][2] = vec_max[2];

        vec_vertices[5][0] = vec_max[0];
        vec_vertices[5][1] = vec_min[1];
        vec_vertices[5][2] = vec_max[2];

        vec_vertices[6][0] = vec_max[0];
        vec_vertices[6][1] = vec_max[1];
        vec_vertices[6][2] = vec_max[2];


        vec_vertices[7][0] = vec_min[0];
        vec_vertices[7][1] = vec_max[1],
        vec_vertices[7][2] = vec_max[2];

}


//Compute halflength vector of AABB
void ComputeHalfLength(double vec_min[3], double vec_max[3], double vec_result[3]) {

    vec_result[0] = abs((vec_max[0] - vec_min[0])/2);
    vec_result[1] = abs((vec_max[1] - vec_min[1])/2);
    vec_result[2] = abs((vec_max[2] - vec_min[2])/2);

}


