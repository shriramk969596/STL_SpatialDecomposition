#ifndef GRID_UTILS_H_INCLUDE
#define GRID_UTILS_H_INCLUDE

#include <iostream>
#include <fstream> 
#include <map>
#include<math.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <CL/cl.h> 
#include <CL/cl_platform.h>

using namespace std;

///////////////////////////////////////Point/////////////////////////////////////////
class Point{

public:

	//Constructor
    Point();
	Point(double x, double y, double z);


	void SetCoord(double x, double y, double z);
	void PrintCoord();
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	vector<double> GetPointVector();
	void SetX(double x);
	void SetY(double y);
	void SetZ(double z);
	void MovePoint(double dx, double dy, double dz);

protected:

	double x_coord, y_coord, z_coord;

};

///////////////////////////////////////Vector////////////////////////////////////////////

class Vector : public Point {

	public:

		Vector();
		Vector(double x, double y, double z);
		Vector(Point vectorHead);

		// void Norm();
		static double Norm(Vector vec);
		// void Vector::Add(Vector vec);
		static Vector Add(Vector vec1, Vector vec2);
		// void Vector::Subtract(Vector vec);
		static Vector Subtract(Vector vec1, Vector vec2);
		static Vector Normalize(Vector vec1);
		static Vector Scale(Vector vec1,  double scaling);
		static Vector CrossProduct(Vector vec1, Vector vec2);
		static double DotProduct(Vector vec1, Vector vec2);
		static double Distance(Vector vec1, Vector vec2);
		static Vector Reflection(Vector vec1, Vector surfaceNormal);

        Vector& operator*(const double &a){

            x_coord = this->x_coord *a;
            y_coord = this->y_coord *a;
            z_coord = this->z_coord *a;

            return *this;
        }

};



inline Vector operator+(const Vector v1, const Vector v2) {
    Vector tmp = Vector(v1.GetX() + v2.GetX(), v1.GetY() + v2.GetY(), v1.GetZ() + v2.GetZ());
    return tmp;
}

inline Vector operator-(const Vector v1, const Vector v2) {
    Vector tmp = Vector(v1.GetX() - v2.GetX(), v1.GetY() - v2.GetY(), v1.GetZ() - v2.GetZ());
    return tmp;
}

inline Vector operator*(const Vector v, float t) {
    Vector tmp =  Vector(t * v.GetX(), t * v.GetY(), t * v.GetZ());
    return tmp;
}

inline Vector operator*(float t, const Vector v) {
    Vector tmp = Vector(t * v.GetX(), t * v.GetY(), t * v.GetZ());
    return tmp;
}

inline Vector operator-(const Vector v1) {
    Vector tmp = Vector(-v1.GetX(), -v1.GetY(), -v1.GetZ());
    return tmp;
}



///////////////////////////////////////Bbox////////////////////////////////////////////
class Bbox{

public:

    //Constructor
    Bbox();
    Bbox(Point Min, Point Max);
    Bbox(vector<int> cellID,Point gridOrigin, double stepX, double stepY, double stepZ);
    Bbox(Point v0, Point v1, Point v2, Point v3, Point v4, Point v5, Point v6, Point v7);

    vector<Point> GetVertices();
    double GetDelX();
    double GetDelY();
    double GetDelZ();
    Point GetCentroid();
    Point GetMin();
    Point GetMax();
    Bbox ExtendBox(double scaling);
    bool PointInsideBbox(Bbox& box, Point point);
    Bbox ExtendBox(Bbox box_reference, double scaling);
    bool IsOverlappingBbox(Bbox box1, Bbox box2);
    vector<double> ComputeHalfLength(Bbox &box1);
    void Create_box_stl(std::string map_filename, Point min, Point max);

private:

    Point min, max;

};

/////////////////////////////////////////////////////////////////////////////////////////
struct compare{

    //Overloading operator for std::Map
    bool operator() (const Point &lhs,const Point &rhs) {

        if(lhs.GetX() != rhs.GetX()) return lhs.GetX() < rhs.GetX();
        if(lhs.GetY() != rhs.GetY()) return lhs.GetY() < rhs.GetY();
        return lhs.GetZ() < rhs.GetZ();

        //return std::make_tuple(lhs.getx(), lhs.gety(), lhs.getz()) < std::make_tuple(rhs.getx(), rhs.gety(), rhs.getz())

    }

};

struct vec_compare{

    //Overloading operator for std::Map
    bool operator() (const vector<int> &lhs,const vector<int> &rhs) {

        if(lhs[0] != rhs[0]) return lhs[0] < rhs[0];
        if(lhs[1] != rhs[1]) return lhs[1] < rhs[1];
        return lhs[2] < rhs[2];

    }

};

///////////////////////////////////////Triangle////////////////////////////////////////////
class Triangle{

public:

    //Constructor
    Triangle();
    Triangle(Point v0, Point v1, Point v2);
    Triangle(Point v0, Point v1, Point v2, int id);

    Point GetCentroid() const;
    int Hash(int i, int j, int k);
    int GetId() const;
    Bbox BoundingVolume(Triangle tria) const;
    Point Get_V0(Triangle tria) const;
    Point Get_V1(Triangle tria) const;
    Point Get_V2(Triangle tria) const;
    Vector GetNormal(const Triangle tria) const;
    Vector GetEdge(const Triangle tria, int edgeNumber) const;
    vector<int> GetParentCellByCentroid(Point gridOrigin, double dx, double dy, double dz, Triangle tria) const;
    Triangle Translate(Triangle tria1, Vector direction) const;
    Vector Get_V0_Vector(Triangle tria) const;
    Vector Get_V1_Vector(Triangle tria) const;
    Vector Get_V2_Vector(Triangle tria) const;
    void CreateTriaSTL(std::string map_filename, const Triangle &tria, Vector facet_normal) const;
    pair<float,bool> RayIntersectsTriangle(Vector rayOrigin, Vector rayVector, const Triangle& inTriangle, Vector& outIntersectionPoint) const;

private:

    Point v0_, v1_, v2_, centroid_;
    int id_ = 000;

};

///////////////////////////////////////FluidParticle///////////////////////////////////

class FluidParticle : public Point{

	public:

		//Constructor
		FluidParticle();
		FluidParticle(double pos_x, double pos_y, double pos_z, int particleId, double smoothingRadius = 1.5, double mass = 0.2);

		int GetId();
		int GetCellId();
		double GetSmoothingRadius();
		void MoveParticle(Point moveAlongVector);
		void SetCellId(int cellId);
		vector<int> GetCellIdByPosition(double dx, double dy, double dz, Point gridOrigin);
		Vector GetVelocity();
        void SetVelocity(double vel_x, double vel_y, double vel_z);
        Vector GetOmega();
        void SetOmega(Vector omega);
        Vector Get_v_half_prev();
        void Set_v_half_prev(Vector v_half_prev);
        Vector GetPosition();
        Vector Set_omega_half_prev(Vector omega_half_prev);
        Vector Get_omega_half_prev();
        double GetMass();
        void WriteCSVlocs(std::string map_filename, double time) const;
        pair<bool, double> Collides(FluidParticle f);
        Vector ResolvePenetrationOnCollision(FluidParticle &f);
        pair<double, double> GetReducedRadius(double d_o, FluidParticle f);
        pair<Vector, Vector> ContactPlaneNormal(Vector C2);
        Vector CurrentContactPoint(Vector n, double d_o, double d1, Vector C1_o, Vector C2_o);
        pair<double, double> u_N(Vector C2_o, Vector C1_o, double d_o);
        pair<double, Vector> u_N_tria(FluidParticle f, Triangle tria);
        Vector v12_perpendicular(Vector v2_theta, Vector omega2_theta, Vector v1_theta, Vector omega1_theta, double d_1, double d_2,
            Vector n_o);
        Vector u_T(Vector v2_half, Vector omega2_half, Vector v1_half, Vector omega1_half, double d_2, Vector n_o, double d_1, double dt);
        Vector Get_u_T_prev();
        void Set_u_T_prev(Vector u_T_prev);
        void Add_du_T(Vector du_t);


        void SetForce(const Vector& F);
        void AddForce(const Vector& F);
        Vector GetForce();

        void SetTorque(const Vector& T);
        void AddTorque(const Vector& T);
        Vector GetTorque();
        double GetInertia();




private:

		int cellId_;
		int particleId_;
		double smoothingRadius_;
		double mass_;
		double vx_ = 0;
		double vy_ = 0;
		double vz_ = 0;
		Vector u_T_prev_;
		Vector v_half_prev_;
		Vector a_ = Vector(0 , 0, 0);
		Vector omega_ = Vector(0, 0 ,0);
        Vector omega_half_prev_;
		Vector F_ = Vector(0, 0, 0);
		Vector T_ = Vector(0 ,0 ,0);


};

///////////////////////////////////////TriangleMesh////////////////////////////////////////////

class TriangleMesh{

public:

	//Constructor
	TriangleMesh();

	Triangle GetTriaById(int id);
	const map<int, Triangle>& GetMesh();
	void Append(Triangle &tria, int id);

private:

	map<int, Triangle> mesh_Tria_;

};


///////////////////////////////////////Grid////////////////////////////////////////////
class Grid{

public:

	//Constructor
    Grid();
	Grid(double delX, double delY, double delZ, int l, int m, int n, Point origin = Point(0,0,0));

	double GetXstep();
	double GetYstep();
	double GetZstep();
    void MoveGrid();
	int GetCellCount();
	int GetVertexCount();
	void GenerateCellMinMaxID();
	void GenerateVTK();
	vector<Point> GenerateCellCentroids();
	Point GetGridOrigin();
	const vector<vector<Point> > GenereateGridConnectivity();
    vector<vector<int> > GetMinMaxIdVector();
	map<int, vector<Point>> GetCellIdMap();
	vector<vector<int>> GetNeighbours(vector<int> cellId);
    void InsertToCellTriaMap(vector<int> cellId, const Triangle tria);
    map<vector<int>, vector<Triangle> , vec_compare>& GetCellTriaMap();


private:

	double dx, dy, dz;
	int num_l, num_m, num_n, cellCount, vertexCount;
	vector<vector<int> > cellMinMaxVertexIDs;
	map<Point, int, compare> vertexIdMap;
	map<int, vector<Point>> cellIdMap;
	vector<Point> cellCentroids;
	int write_vtk = 0;
	Point grid_Origin;
	map<vector<int>, vector<Triangle> , vec_compare> cellTriaMap;

};



#endif
