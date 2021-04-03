#include "grid_utils.h"

///////////////////////////////////////Point////////////////////////////////////////////

//Constructor
Point::Point() {

}

Point::Point(double x, double y, double z) {
	x_coord = x;
	y_coord = y;
	z_coord = z;
}

//set coordinates of point taking x, y and z coodinate values
void Point::SetCoord(double x, double y, double z) {
	x_coord = x;
	y_coord = y;
	z_coord = z;
}

//cout point coordinates
void Point::PrintCoord() {
	
	cout << "(" << x_coord << " " << y_coord << " " << z_coord << ")" << endl;
}

//returns the x coordinate of point
double Point::GetX() const{
	return x_coord;
}

//returns the y coordinate of point
double Point::GetY() const{
	return y_coord;
}

//returns the z coordinate of point
double Point::GetZ() const{
	return z_coord;
}

//sets x coordinate of point
void Point::SetX(double x) {
	x_coord = x;
}

//sets y coordinate of point
void Point::SetY(double y) {
	y_coord = y;
}

//sets z coordinate of point
void Point::SetZ(double z) {
	z_coord = z;
}

//returns point coordinate as a vector
vector<double> Point::GetPointVector() {

	vector<double> point;

	point.push_back(x_coord);
	point.push_back(y_coord);
	point.push_back(z_coord);

	return point;

}

//moves "this" instance of point in x, y and z direction by dx. dy and dz respectively
void Point::MovePoint(double dx, double dy, double dz) {

	x_coord += dx;
	y_coord += dy;
	z_coord += dz; 

}


///////////////////////////////////////FluidParticle////////////////////////////////////////////

//Constructor
FluidParticle::FluidParticle() {

}

//pos_ = position/location of the fluid particle
FluidParticle::FluidParticle(double pos_x, double pos_y, double pos_z, int particleId, double smoothingRadius, double mass) {

		smoothingRadius_ = smoothingRadius;
		x_coord = pos_x;
		y_coord = pos_y;
		z_coord = pos_z;
		particleId_ = particleId;
		mass_ = mass;

}

//returns particleId_ of "this" particle
int FluidParticle::GetId() {
	return particleId_;
}

//return cellId_ of "this" particle
int FluidParticle::GetCellId() {
	return cellId_;
}

vector<int> FluidParticle::GetCellIdByPosition(double dx, double dy, double dz, Point gridOrigin) {
	return {(int) ((x_coord - gridOrigin.GetX())/dx), (int) ((y_coord- gridOrigin.GetY())/dy), (int) ((z_coord - gridOrigin.GetZ())/dz)};
}

//returns smoothingRadius_ of "this" particle
double FluidParticle::GetSmoothingRadius() {

	return smoothingRadius_;

}

//sets cellId_ for "this" particle
void FluidParticle::SetCellId(int cellId) {
	cellId_ = cellId;
}

void FluidParticle::SetVelocity(double vel_x, double vel_y, double vel_z) {

	vx_ = vel_x;
	vy_ = vel_y;
	vz_ = vel_z;

}

Vector FluidParticle::GetVelocity() {
	return Vector(vx_, vy_, vz_);
}

Vector FluidParticle::GetOmega() {
    return this->omega_;
}

void FluidParticle::SetOmega(Vector omega) {
    this->omega_ = omega;
}

Vector FluidParticle::Get_v_half_prev() {
    return this->v_half_prev_;
}

void FluidParticle::Set_v_half_prev(Vector v_half_prev) {
    this->v_half_prev_ = v_half_prev;
}

Vector FluidParticle::Set_omega_half_prev(Vector omega_half_prev) {
    this->omega_half_prev_ = omega_half_prev;
}

Vector FluidParticle::Get_omega_half_prev() {
    return this->omega_half_prev_;
}

Vector FluidParticle::GetPosition() {
    return Vector(x_coord, y_coord, z_coord);
}

double FluidParticle::GetMass() {
    return this->mass_;
}

void FluidParticle::WriteCSVlocs(std::string map_filename, double time) const {
	
	std::ofstream file;

	boost::filesystem::path file_ex(map_filename);

	// file.open(map_filename,std::ios::out|std::ios::app);
	file.open(map_filename,std::ios_base::app);
	file << x_coord  << "," << y_coord << "," << z_coord << "," << time << std::endl;
	file.close();

}

pair<bool, double> FluidParticle::Collides(FluidParticle f) {
    double distance = sqrt((x_coord - f.GetX())*(x_coord - f.GetX())+(y_coord - f.GetY())*(y_coord - f.GetY())+(z_coord - f.GetZ())*(z_coord - f.GetZ()));
    double d_o = smoothingRadius_ + f.GetSmoothingRadius();
    if(distance <= smoothingRadius_ + f.GetSmoothingRadius()) {
        return(make_pair(1,  d_o));
    }
    return(make_pair(0, d_o));
}

pair<double, double> FluidParticle::GetReducedRadius(double d_o, FluidParticle f) {
    double d1 = (smoothingRadius_ + ((d_o - smoothingRadius_ - f.GetSmoothingRadius())/2.0f));
    return(make_pair(d1, d_o - d1));
}

pair<Vector, Vector> FluidParticle::ContactPlaneNormal(Vector C2_o) {
    Vector C1_o = Vector(x_coord, y_coord, z_coord);
    Vector  n = Vector::Subtract(C2_o, C1_o);
    return make_pair((Vector::Normalize(n)), C1_o);
}

Vector FluidParticle::CurrentContactPoint(Vector n, double d_o, double d1, Vector C1_o, Vector C2_o) {
    Vector C_o = C1_o + (d1 - (d_o - Vector::Norm(C2_o - C1_o))/2.0f)*n;
}

pair<double, double> FluidParticle::u_N(Vector C2_o, Vector C1_o, double d_o) {
    double u_N = Vector::Norm(C2_o - C1_o) - d_o;
    double epsilon_N = u_N/d_o;
    return make_pair(u_N, epsilon_N);
}

pair<double, Vector> FluidParticle::u_N_tria(FluidParticle f1, Triangle tria) {
    Vector intersectionPoint;

    Vector triaNormal = tria.GetNormal(tria);
    triaNormal = Vector::Normalize(triaNormal);

    Vector unitRay;
    unitRay = triaNormal;
    if(Vector::DotProduct(f1.GetVelocity(),triaNormal) < 0) {
        unitRay = -triaNormal;
    }

    pair<float,bool> intersects = tria.RayIntersectsTriangle(f1.GetPosition(), unitRay, tria, intersectionPoint);

    double u_N = 0;
    if(intersects.second) {
        double distToTria = intersects.first;
        if( abs(distToTria) < f1.GetSmoothingRadius()) {
            double u_N = f1.GetSmoothingRadius() - abs(distToTria);
        }
    }

    return make_pair(u_N, unitRay);
}

//TODO: Implement u_T in grid utils
Vector FluidParticle::u_T(Vector v2_half_prev, Vector omega2_half_prev, Vector v1_half_prev, Vector omega1_half_prev, double d_2, Vector n_o, double d_1, double dt) {

    Vector v12 = (v2_half_prev + Vector::CrossProduct(omega2_half_prev, (-d_2 * n_o))) - (v1_half_prev + Vector::CrossProduct(omega1_half_prev, (-d_1 * n_o)));
    Vector v12_perpendicular = v12 - (Vector::DotProduct(n_o, v12) * n_o);
    Vector du_T = -dt * v12_perpendicular;
    return du_T;

}

Vector FluidParticle::Get_u_T_prev() {
    return this->u_T_prev_;
}
void FluidParticle::Set_u_T_prev(Vector u_T_prev) {
    this->u_T_prev_ = u_T_prev;
}

void FluidParticle::Add_du_T(Vector du_t) {
    this->u_T_prev_ = this->u_T_prev_ + du_t;
}

Vector FluidParticle::v12_perpendicular(Vector v2_theta, Vector omega2_theta, Vector v1_theta, Vector omega1_theta, double d_1, double d_2, Vector n_o) {
    Vector v12 = (v2_theta + Vector::CrossProduct(omega2_theta, (-d_2 * n_o))) - (v1_theta + Vector::CrossProduct(omega1_theta, (d_1 * n_o)));
    return (v12 - ((Vector::DotProduct(n_o, v12)) * n_o));
}

void FluidParticle::SetForce(const Vector& F) {
    this->F_ = F;
}
void FluidParticle::AddForce(const Vector& F) {
//    cout << F.GetX() << " " << F.GetY() << " " << F.GetZ() << endl;
    this->F_ = this->F_ + F;
}
Vector FluidParticle::GetForce() {
    return this->F_;
}

void FluidParticle::SetTorque(const Vector& T) {
    this->T_ = T;
}
void FluidParticle::AddTorque(const Vector& T) {
    this->T_ = this->T_ + T;

}
Vector FluidParticle::GetTorque() {
    return this->T_;
}

double FluidParticle::GetInertia() {
    return ((2/5)*this->mass_*(this->smoothingRadius_)*(this->smoothingRadius_));
}
///////////////////////////////////////Vector////////////////////////////////////////////

//Constructor
Vector::Vector() {

}

Vector::Vector(double x, double y, double z) {

	x_coord = x;
	y_coord = y;
	z_coord = z;

}

Vector::Vector(Point vectorHead) {

	x_coord = vectorHead.GetX();
	y_coord = vectorHead.GetY();
	z_coord = vectorHead.GetZ();

}

// void Vector::Norm() {

// 	double norm = sqrt((this->GetX()*this->GetX()+this->GetY()->this->GetY()+this->GetY()*this->GetY()));

// }

//returns eucleadian norm of vec
double Vector::Norm(Vector vec) {

	return sqrt(( (vec.GetX()*vec.GetX()) + (vec.GetY()*vec.GetY()) + (vec.GetZ()*vec.GetZ()) ));

}

//returns a vector object which is the sum of vec1 and vec2
Vector Vector::Add(Vector vec1, Vector vec2) {

	return Vector(vec1.GetX()+vec2.GetX(), vec1.GetY()+vec2.GetY(), vec1.GetZ()+vec2.GetZ());

}

// void Vector::Add(Vector vec) {

// 	this.SetX() += vec.GetX();
// 	this.SetY() += vec.GetY();
// 	this.SetZ() += vec.GetZ();

// }

//returns a vector object which is the difference of vec1 and vec2
Vector Vector::Subtract(Vector vec1, Vector vec2) {

	return Vector(vec1.GetX()-vec2.GetX(), vec1.GetY()-vec2.GetY(), vec1.GetZ()-vec2.GetZ());

}

// void Vector::Subtract(Vector vec) {

	// 	this.SetX() -= vec.GetX();
	// 	this.SetY() -= vec.GetY();
	// 	this.SetZ() -= vec.GetZ();

// }



double Vector::Distance(Vector vec1, Vector vec2) {
 
	return sqrt(((vec2.GetX()-vec1.GetX()) * (vec2.GetX()-vec1.GetX()))  +  
			((vec2.GetY()-vec1.GetY()) * (vec2.GetY()-vec1.GetY()))  +   
			((vec2.GetZ()-vec1.GetZ()) * (vec2.GetZ()-vec1.GetZ())));
	
}
//returns a normalized vector.
Vector Vector::Normalize(Vector vec1) {

	double norm = Vector::Norm(vec1);
	return Vector((vec1.GetX()) / (double)norm, (vec1.GetY()) / (double)norm, (vec1.GetZ()) / (double)norm );

}

Vector Vector::Scale(Vector vec1,  double scaling) {

	vec1.SetX( (vec1.GetX()) * scaling);
	vec1.SetY( (vec1.GetY()) * scaling);
	vec1.SetZ( (vec1.GetZ()) * scaling);

	return vec1;

}

Vector Vector::CrossProduct(Vector vec1, Vector vec2) {

	vector<double> vec_Res;

	vec_Res.push_back(vec1.GetY() * vec2.GetZ() - vec1.GetZ() * vec2.GetY());   

    vec_Res.push_back(vec1.GetZ() * vec2.GetX() - vec1.GetX() * vec2.GetZ());   

    vec_Res.push_back(vec1.GetX() * vec2.GetY() - vec1.GetY() * vec2.GetX());   

	return Vector(vec_Res[0], vec_Res[1], vec_Res[2]);

}

double Vector::DotProduct(Vector vec1, Vector vec2) {

	return(vec1.GetX()*vec2.GetX()+vec1.GetY()*vec2.GetY()+vec1.GetZ()*vec2.GetZ());  

}

//Surface normal to be used depends on the direction of approach of particle.(The correct surface normal is one which produces -ve dot product with vec1)
Vector Vector::Reflection(Vector vec1, Vector surfaceNormal) {

	Vector::Normalize(surfaceNormal);
	
	if( Vector::DotProduct(vec1, surfaceNormal) > 0) {
		Vector::Scale(surfaceNormal, -1);
	}

	double scale = Vector::DotProduct(vec1, surfaceNormal);
	Vector tmp_Normal = surfaceNormal;
	Vector tmp_ScaledNormal = Vector::Scale(tmp_Normal, scale);
	tmp_ScaledNormal = Vector::Scale(tmp_ScaledNormal, -2);

	return Vector::Add(vec1, tmp_ScaledNormal);

}

///////////////////////////////////////Triangle////////////////////////////////////////////

	// 	v0 /\
	// 	  /  \  
        //    E0 /    \ E2
	//      /      \
	//  v1 /________\ v2
	//         E1

//Constructor
Triangle::Triangle() {

}

Triangle::Triangle(Point v0, Point v1, Point v2) {

	v0_ = v0;
	v1_ = v1;
	v2_ = v2;

}

Triangle::Triangle(Point v0, Point v1, Point v2, int id) {

	v0_ = v0;
	v1_ = v1;
	v2_ = v2;
	id_ = id;
	//Normal

}

Point Triangle::GetCentroid() const {

	double x = (v0_.GetX() + v1_.GetX() + v2_.GetX()) / 3;
	double y = (v0_.GetY() + v1_.GetY() + v2_.GetY()) / 3;
	double z = (v0_.GetZ() + v1_.GetZ() + v2_.GetZ()) / 3;
	
	return Point(x,y,z);
}

Point Triangle::Get_V0(Triangle tria) const {
	return v0_;
}

Point Triangle::Get_V1(Triangle tria) const {
	return v1_;
} 

Point Triangle::Get_V2(Triangle tria) const {
	return v2_;
} 

Vector Triangle::Get_V0_Vector(Triangle tria) const {
	return Vector(v0_.GetX(),v0_.GetY(),v0_.GetZ());
}

Vector Triangle::Get_V1_Vector(Triangle tria) const {
	return Vector(v1_.GetX(),v1_.GetY(),v1_.GetZ());
}

Vector Triangle::Get_V2_Vector(Triangle tria) const {
	return Vector(v2_.GetX(),v2_.GetY(),v2_.GetZ());
}

Vector Triangle::GetNormal(const Triangle tria) const {

	Vector tria_V0 = Vector(tria.Get_V0(tria).GetX(), tria.Get_V0(tria).GetY(), tria.Get_V0(tria).GetZ());
	Vector tria_V1 = Vector(tria.Get_V1(tria).GetX(), tria.Get_V1(tria).GetY(), tria.Get_V1(tria).GetZ());
	Vector tria_V2 = Vector(tria.Get_V2(tria).GetX(), tria.Get_V2(tria).GetY(), tria.Get_V2(tria).GetZ());

	Vector edge_0 = Vector::Subtract(tria_V1,tria_V0);
	Vector edge_1 = Vector::Subtract(tria_V2,tria_V1);
	Vector edge_2 = Vector::Subtract(tria_V0,tria_V2);

	return Vector::CrossProduct(edge_0, edge_1);

}

Bbox Triangle::BoundingVolume(Triangle tria) const {

	double x_min = std::min(std::min(tria.v0_.GetX(), tria.v1_.GetX()), tria.v2_.GetX());
	double y_min = std::min(std::min(tria.v0_.GetY(), tria.v1_.GetY()), tria.v2_.GetY());  
	double z_min = std::min(std::min(tria.v0_.GetZ(), tria.v1_.GetZ()), tria.v2_.GetZ());  

	double x_max = std::max(std::max(tria.v0_.GetX(), tria.v1_.GetX()), tria.v2_.GetX());
	double y_max = std::max(std::max(tria.v0_.GetY(), tria.v1_.GetY()), tria.v2_.GetY());
	double z_max = std::max(std::max(tria.v0_.GetZ(), tria.v1_.GetZ()), tria.v2_.GetZ());

	return Bbox(Point(x_min, y_min, z_min), Point(x_max, y_max, z_max));

}

int Triangle::GetId() const {
		return this->id_;
}

vector<int> Triangle::GetParentCellByCentroid(Point gridOrigin, double dx, double dy, double dz, Triangle tria) const {

	Vector vec_translate = Vector(gridOrigin.GetX(), gridOrigin.GetY(), gridOrigin.GetZ());
	Triangle tria_translated = tria.Translate(tria, vec_translate);
	Point triaCentroid = tria_translated.GetCentroid();
	return {(int) ((triaCentroid.GetX())/dx), (int) ((triaCentroid.GetY())/dy), (int) ((triaCentroid.GetZ())/dz)};

}

// edgeNumber = 0 ---> return E0 
Vector Triangle::GetEdge(const Triangle tria, int edgeNumber) const {

	Vector tria_V0 = Vector(tria.Get_V0(tria).GetX(), tria.Get_V0(tria).GetY(), tria.Get_V0(tria).GetZ());
	Vector tria_V1 = Vector(tria.Get_V1(tria).GetX(), tria.Get_V1(tria).GetY(), tria.Get_V1(tria).GetZ());
	Vector tria_V2 = Vector(tria.Get_V2(tria).GetX(), tria.Get_V2(tria).GetY(), tria.Get_V2(tria).GetZ());

	switch(edgeNumber) {

  		case 0:

    		return Vector::Subtract(tria_V1,tria_V0);

    	case 1:

    		return Vector::Subtract(tria_V2,tria_V1);

		case 2:

			return Vector::Subtract(tria_V0,tria_V2);

		default:

			return Vector::Subtract(tria_V1,tria_V0);
	}

}

Triangle Triangle::Translate(Triangle tria1, Vector direction) const {

	Vector v0_new = Vector::Subtract(Vector(tria1.Get_V0(tria1).GetX(),tria1.Get_V0(tria1).GetY(),tria1.Get_V0(tria1).GetZ()),direction);
	Vector v1_new = Vector::Subtract(Vector(tria1.Get_V1(tria1).GetX(),tria1.Get_V1(tria1).GetY(),tria1.Get_V1(tria1).GetZ()),direction);
	Vector v2_new = Vector::Subtract(Vector(tria1.Get_V2(tria1).GetX(),tria1.Get_V2(tria1).GetY(),tria1.Get_V2(tria1).GetZ()),direction);

	return Triangle(v0_new,v1_new,v2_new);

}

//NOTE: rayVector = Direction; vector should always be a unit vector
pair<float,bool> Triangle::RayIntersectsTriangle(Vector rayOrigin, Vector rayVector,const Triangle& inTriangle, Vector& outIntersectionPoint) const {

	const float EPSILON = 0.0000001;

    Vector v0 = inTriangle.Get_V0_Vector(inTriangle);
    Vector v1 = inTriangle.Get_V1_Vector(inTriangle);  
    Vector v2 = inTriangle.Get_V2_Vector(inTriangle);

    Vector edge1, edge2, h, s, q;
    float a,f,u,v;

    edge1 = Vector::Subtract(v1,v0);
    edge2 = Vector::Subtract(v2,v0);

    h = Vector::CrossProduct(rayVector,edge2);
    a = Vector::DotProduct(edge1,h);

    if (a > -EPSILON && a < EPSILON) {
        return make_pair(-1,false);    // This ray is parallel to this triangle.
	}
	
    f = 1.0/a;
    s = Vector::Subtract(rayOrigin,v0);
    u = f * Vector::DotProduct(s,h);

    if (u < 0.0 || u > 1.0) {
        return make_pair(-1,false);
	}

    q = Vector::CrossProduct(s,edge1);
    v = f * Vector::DotProduct(rayVector,q);

    if (v < 0.0 || u + v > 1.0) {
        return make_pair(-1,false);
	}

    // Compute t to find out where the intersection point is on the line.
    float t = f * Vector::DotProduct(edge2,q);
    if (t > EPSILON) // ray intersection
    {
        outIntersectionPoint = Vector::Add(rayOrigin, Vector::Scale(rayVector,t));
        return make_pair(t,true);
    }
    else // This means that there is a line intersection but not a ray intersection.
        return make_pair(-1,false);
}

void Triangle::CreateTriaSTL(std::string map_filename, const Triangle &tria, Vector facet_normal) const {
	
	std::ofstream file;

	boost::filesystem::path file_ex(map_filename);

	// if(boost::filesystem::exists(map_filename))
	// {
	//     boost::filesystem::remove(map_filename);
	// }

	// file.open(map_filename,std::ios::out|std::ios::app);
	file.open(map_filename,std::ios_base::app);
	file << "solid" << std::endl;

	file << " facet normal " << facet_normal.GetX() << " " << facet_normal.GetY() << " " << facet_normal.GetZ() << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << tria.Get_V0(tria).GetX() << " " << tria.Get_V0(tria).GetY() << " " << tria.Get_V0(tria).GetZ() << std::endl;
	file << "     vertex " << tria.Get_V1(tria).GetX() << " " << tria.Get_V1(tria).GetY() << " " << tria.Get_V1(tria).GetZ() << std::endl;
	file << "     vertex " << tria.Get_V2(tria).GetX() << " " << tria.Get_V2(tria).GetY() << " " << tria.Get_V2(tria).GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << "endsolid" << std::endl;
	file.close();

}

///////////////////////////////////////Bbox////////////////////////////////////////////

		//--------------------------------------------------------
        //
        //           6-------7  
        //          /|      /|
        //         4-+-----5 |
        //         | |     | |   y
        //         | 2-----+-3   | z
        //         |/      |/    |/
        //         0-------1     +--x
        //
        //         0   =>   0, 0, 0
        //         1   =>   0, 0, 1
        //         2   =>   0, 0, 1
        //         3   =>   1, 0, 1
        //         4   =>   0, 1, 0
        //         5   =>   1, 1, 0
        //         6   =>   0, 1, 1
        //         7   =>   1, 1, 1
        //
        //--------------------------------------------------------

//Constructor
Bbox::Bbox() {

}

Bbox::Bbox(Point Min, Point Max) {
	min = Min;
	max = Max;
}

Bbox::Bbox(Point v0, Point v1, Point v2, Point v3, Point v4, Point v5, Point v6, Point v7) {
	min = v0;
	max = v7;
}

Bbox::Bbox(vector<int> cellID,Point gridOrigin, double stepX, double stepY, double stepZ) {

	min = Point((cellID[0]*stepX)+gridOrigin.GetX(), (cellID[1]*stepY)+gridOrigin.GetY(), (cellID[2]*stepZ)+gridOrigin.GetZ());
	max = Point( ( (cellID[0]+1) * stepX) + gridOrigin.GetX(), ( (cellID[1]+1) * stepY) + gridOrigin.GetY(), ( (cellID[2]+1) * stepZ) + gridOrigin.GetZ());

}

//retruns a vector containing all vertices of bounding box in the required order for VTK
vector<Point> Bbox::GetVertices() {

	Point v0 = Point(min.GetX(), min.GetY(), min.GetZ());
	Point v1 = Point(max.GetX(), min.GetY(), min.GetZ()); 
	Point v2 = Point(min.GetX(), min.GetY(), max.GetZ()); 
	Point v3 = Point(max.GetX(), min.GetY(), max.GetZ()); 
	Point v4 = Point(min.GetX(), max.GetY(), min.GetZ()); 
	Point v5 = Point(max.GetX(), max.GetY(), min.GetZ()); 
	Point v6 = Point(min.GetX(), max.GetY(), max.GetZ()); 
	Point v7 = Point(max.GetX(), max.GetY(), max.GetZ());  

	vector<Point> vertices = {v0,v1,v2,v3,v4,v5,v6,v7};

	return vertices;

}

//returns a point object which represents the centroid of "this" bounding box 
Point Bbox::GetCentroid() {
	return Point((min.GetX() + max.GetX())/2, (min.GetY() + max.GetY())/2, (min.GetZ() + max.GetZ())/2);
}

//returns half length of boundng box along x axis. Used by GetCentroid()
double Bbox::GetDelX() {
	double delX = max.GetX() - min.GetX();
	return delX;
}

//returns half length of boundng box along y axis. Used by GetCentroid()
double Bbox::GetDelY() {
	double delY = max.GetY() - min.GetY();
	return delY;
}

//returns half length of boundng box along z axis. Used by GetCentroid()
double Bbox::GetDelZ() {
	double delZ = max.GetZ() - min.GetZ();
	return delZ;
}

//returns minimum bound of box
Point Bbox::GetMin() {
	return min;
}

//returns maximum bound of box
Point Bbox::GetMax() {
	return max;
}

//extends "this" bounding box by a vector whose magnitude is scaling value
Bbox Bbox::ExtendBox(Bbox box_reference, double scaling) {
	
	Vector vec_centroid = Vector(box_reference.GetCentroid()); // Defines a vector from origin to centroid.
	Vector vec_originToMinVertex = box_reference.GetMin();
	Vector vec_centroidToMinVertex =  Vector::Subtract(vec_originToMinVertex, vec_centroid); //returns a vector directed from centroid to vertex

	Point tmp_ExtendedMin = Vector::Add(vec_originToMinVertex, Vector::Scale(Vector::Normalize(vec_centroidToMinVertex), scaling));

	Vector vec_originToMaxVertex = box_reference.GetMax();
	Vector vec_centroidToMaxVertex = Vector::Subtract(vec_originToMaxVertex, vec_centroid);
	Point tmp_ExtendedMax = Vector::Add(vec_originToMaxVertex, Vector::Scale(Vector::Normalize(vec_centroidToMaxVertex), scaling));

	return Bbox(tmp_ExtendedMin, tmp_ExtendedMax);

}

//checks if a point lies inside the box
bool Bbox::PointInsideBbox(Bbox& box, Point point) {

	if(point.GetX() >= box.GetMin().GetX() && point.GetY() >= box.GetMin().GetY() && point.GetZ() >= box.GetMin().GetZ() &&
	point.GetX() <= box.GetMax().GetX() && point.GetY() <= box.GetMax().GetY() && point.GetZ() <= box.GetMax().GetZ() ) 
	{
		return true;
	} else {
		return false;
	}

}

//check if 2 AABB's overlap
bool Bbox::IsOverlappingBbox(Bbox box1, Bbox box2) {

	return(box1.GetMax().GetX() > box2.GetMin().GetX() &&
	box1.GetMin().GetX() < box2.GetMax().GetX() &&
	box1.GetMax().GetY() > box2.GetMin().GetY() &&
	box1.GetMin().GetY() < box2.GetMax().GetY() &&
	box1.GetMax().GetZ() > box2.GetMin().GetZ() &&
	box1.GetMin().GetZ() < box2.GetMax().GetZ() );

}

//Compute halflength vector of AABB
vector<double> Bbox::ComputeHalfLength(Bbox &box1) {
    vector<double> halfLength = {abs((box1.GetMax().GetX() - box1.GetMin().GetX())/(double)2)};
	halfLength.push_back(abs((box1.GetMax().GetX() - box1.GetMin().GetX())/(double)2));
	halfLength.push_back((abs(box1.GetMax().GetZ() - box1.GetMin().GetZ())/(double)2));

	return halfLength;
}

void Bbox::Create_box_stl(std::string map_filename, Point min, Point max) {
	std::ofstream file;

	boost::filesystem::path file_ex(map_filename);

	// file.open(map_filename,std::ios::out|std::ios::app);
	file.open(map_filename,std::ios_base::app);
	file << "solid" << std::endl;

	file << " facet normal " << -1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << -1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << " facet normal " << 1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;


	file << " facet normal " << 0.0 << " " << -1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << -1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << " facet normal " << 0.0 << " " << 1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << 1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "   endloop" << std::endl;

	file << " endfacet" << std::endl;

	file << " facet normal " << 0.0 << " " << 0.0 << " " << -1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << 0.0 << " " << -1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << min.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << " facet normal " << 0.0 << " " << 0.0 << " " << 1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << min.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << 0.0 << " " << 1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << max.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << min.GetX() << " " << max.GetY() << " " << max.GetZ() << std::endl;
	file << "     vertex " << max.GetX() << " " << min.GetY() << " " << max.GetZ() << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;


	file << "endsolid" << std::endl;
	file.close();
}

///////////////////////////////////TriangleMesh////////////////////////////////////////

//Constructor
TriangleMesh::TriangleMesh() {

}

void TriangleMesh::Append(Triangle &tria, int id) {
	mesh_Tria_.insert({id, tria});
}

Triangle TriangleMesh::GetTriaById(int id) {
	return mesh_Tria_[id];
}

const map<int, Triangle>& TriangleMesh::GetMesh() {
	return mesh_Tria_;
}

///////////////////////////////////////Grid////////////////////////////////////////////

//Contains necessary methods to generate grid data for an unstructured grid and to write the corresponding VTK file to visualize in paraview.
//Constructor
Grid::Grid() {

}

Grid::Grid(double delX, double delY, double delZ, int l, int m, int n, Point origin) {
	dx = delX;
	dy = delY; 
	dz = delZ;
	num_l = l; //X
	num_m = m; //Y
	num_n = n; //Z
	cellCount = ((l-1)*(m-1)*(n-1));
	vertexCount = num_l*num_m*num_n;
	grid_Origin = origin;
	
}

double Grid::GetXstep() {
	return dx;
}

double Grid::GetYstep() {
	return dy;
}

double Grid::GetZstep() {
	return dz;
}

//The grid origin represents the minimum bound of the first cell in the grid.
Point Grid::GetGridOrigin() {
	return grid_Origin;
}

//used to relocate the grid origin to a new location.
void Grid::MoveGrid() {

}

//returns total numer of cells in the grid
int Grid::GetCellCount() {
	return cellCount;
}

//returns total number of vertices in the grid without repetition
int Grid::GetVertexCount() {
	return vertexCount;
}

//returns a vector containing integer Id's using which the grid can be reconstructed. 
// {0,0,0,1,1,1} would represent a single cell in the grid -> the values when multiplied
// with gris size dx, dy and dz give the coordinates of the minimum and maximum bound of the cell.
void Grid::GenerateCellMinMaxID() {
	
	vector<int> tmp_vec;

	for(int i = 0; i < num_n-1; i++) {

		for(int j = 0; j < num_m-1; j++) {

			for(int k = 0; k < num_l-1; k++) {

				cellMinMaxVertexIDs.push_back({k,j,i,k+1,j+1,i+1});
				
			}
		}
	}


}

//returns cell minmax id vector
vector<vector<int> > Grid::GetMinMaxIdVector() {
	return cellMinMaxVertexIDs;
}

//generates the connectivity data required for VTK.
//vec_Connectivity - vector of vectors where each sub vetor is a collection of 8 points
//belonging to a cell. The vertex ordering of each cell is in compliance with VTK file standards
//cellIdMap - standard map containing point objects as keys and integers as the value. The values of
//the map represent point Id's used to generate connectivity in GenerateVTK().
const vector<vector<Point> > Grid::GenereateGridConnectivity() {
     
	int value = 0;
	vector<int> minMax;
	Bbox tmp_box;
	vector<Point> tmp_box_vertices; 
	pair <map<Point, int>::iterator, bool> ptr;

	if(cellMinMaxVertexIDs.empty()) {

		GenerateCellMinMaxID();

	}

	vector<vector<Point>> vec_Connectivity; 

	cout << cellCount << " CellCount" << endl;
	
	for(int i = 0; i < cellCount; i++) {

		minMax = cellMinMaxVertexIDs[i];
		tmp_box = Bbox(Point((minMax[0]*dx) + grid_Origin.GetX(), (minMax[1]*dy) + grid_Origin.GetY(), (minMax[2]*dz) + grid_Origin.GetZ()), Point((minMax[3]*dx) + grid_Origin.GetX(), (minMax[4]*dy) + grid_Origin.GetY(), (minMax[5]*dz) + grid_Origin.GetZ())); 

		tmp_box_vertices = tmp_box.GetVertices();

		for(int j = 0; j < 8; j++) {

			if((vertexIdMap.insert(std::make_pair(tmp_box_vertices[j],value))).second) {

					value++;

			}
			
		}

		cellIdMap.insert({i,tmp_box_vertices});

		vec_Connectivity.push_back(tmp_box_vertices);

	}

	return vec_Connectivity;

}

map<int, vector<Point>> Grid::GetCellIdMap() {

	return cellIdMap;

}

vector<Point> Grid::GenerateCellCentroids() {

	vector<Point> cellCentroids;
	
	if(cellMinMaxVertexIDs.size() == 0) {

		GenerateCellMinMaxID();

	}
	
	Point min;
	Point max;

	for(int i = 0; i < cellCount; i++) {

			min = Point(cellMinMaxVertexIDs[i][0]*dx,cellMinMaxVertexIDs[i][1]*dy,cellMinMaxVertexIDs[i][2]*dz);
			max = Point(cellMinMaxVertexIDs[i][3]*dx,cellMinMaxVertexIDs[i][4]*dy,cellMinMaxVertexIDs[i][5]*dz);

			Bbox tmp_Bbox = Bbox(min, max);
			cellCentroids.push_back(tmp_Bbox.GetCentroid());

	}

	return cellCentroids;

}

void Grid::GenerateVTK() {

	vector<vector<Point>> con = GenereateGridConnectivity();

	ofstream vtkFile;
	vtkFile.open("./../../OUTPUTS/Grid.vtk");

	cout << "Writing VTK for Grid dx = " << dx << " dy = " << dy << "dz = " << dz << endl;
	cout << "Grid count" << num_l << "X"<< num_m << "X"<< num_n << endl;

	vtkFile << "# vtk DataFile Version 2.0" << endl;
	vtkFile << "Grid_AABB_Tria" << endl;
	vtkFile << "ASCII" << endl;
	vtkFile << "DATASET UNSTRUCTURED_GRID" << endl;
	vtkFile << "POINTS " << vertexCount <<" Float64" << endl; // num_l*num_m*num_n = Total number of vertices in the grid

	unique_ptr<Point[]> pointOrdering(new Point[vertexCount]());

	for(auto const& pair: vertexIdMap) {
		pointOrdering[pair.second] = pair.first;
	}

	for(int i = 0; i < vertexCount; i++) {
		vtkFile << pointOrdering[i].GetX() << " " << pointOrdering[i].GetY() << " " << pointOrdering[i].GetZ() << endl;
	}

	vtkFile << "\nCELLS" << " " << cellCount << " " << cellCount * 9 <<  endl; //((num_l-1)*(num_m-1)*(num_n-1)) total number of cells
	
	for(auto const& pair: cellIdMap) {

		vtkFile << "8 ";

		for(int i = 0; i < 8; i++) {
			vtkFile << vertexIdMap[pair.second[i]] << " ";
		}
		vtkFile << endl;
	}

	vtkFile << "\nCELL_TYPES" << " " << cellCount <<  endl;

	for(int i = 0; i < cellCount; i++) {
		vtkFile<< "11" << endl;
	}

	vtkFile << "POINT_DATA" << " " << vertexCount <<  endl;
	vtkFile << "SCALARS" << " " << "scalars" << " " << "Float64" << " " << "1" << endl;
	vtkFile << "LOOKUP_TABLE" << " " << "default" <<  endl;

	for(int i = 0; i < vertexCount; i++) {
		vtkFile<< "2" << endl;
	}
	vtkFile << endl;
	vtkFile.close();
}

/*


					 +   +   +  
                    +   +   +
                   +   +   +

					 +   +   +  
                    +  (+)   +      (+) ---> cellId
                   +   +   +  

                     +   +   +  
                    +   +   +
          rootCell(+)  +   +     


*/

void Grid::InsertToCellTriaMap(vector<int> cellId, const Triangle tria) {

	cellTriaMap[cellId].push_back(tria);

}

map<vector<int>, vector<Triangle>, vec_compare >& Grid::GetCellTriaMap() {

	return cellTriaMap;

}

vector<vector<int>> Grid::GetNeighbours(vector<int> cellId) {

	vector<vector<int>> neighbours;
	vector<int> rootCell = {cellId[0]-1, cellId[1]-1, cellId[2]-1};
	vector<int> validXids;

	vector<int> tmp;

	if(cellId[0]-1 < 0) {

		validXids.push_back(cellId[0]);
		validXids.push_back(cellId[0]+1);

	}else if(cellId[0]+1 > num_l-2) {

		validXids.push_back(cellId[0]);
		validXids.push_back(cellId[0]-1);
		
	} else {

		validXids.push_back(cellId[0]);
		validXids.push_back(cellId[0]-1);
		validXids.push_back(cellId[0]+1);

	}
	
	for(auto id: validXids) {

		for(int i = rootCell[1]; i <= rootCell[1]+2; i++) {

			if(i < 0 || i > num_m-2) {
				continue;
			}

			for(int j = rootCell[2]; j <= rootCell[2]+2; j++) {

				if(j < 0 || j > num_n-2) {
					continue;
				}

				tmp = {id, i, j};	

				if(tmp[0] == cellId[0] && tmp[1] == cellId[1] && tmp[2] == cellId[2]) {

					continue;

				}
				neighbours.push_back(tmp);
				
			}	

		}
	}

	return neighbours;
}