//
// Created by shriram.krishna on 9/28/20.
//

#include "dem_core.h"

dem_core::dem_core(map<int, FluidParticle>& particleIdMap, map<vector<int>, vector<Triangle> ,
                   vec_compare>& cellTriaMap, Grid &grid, double K_N, double K_T) : particleIdMap_(particleIdMap), cellTriaMap_(cellTriaMap) , grid_(grid) {

    particleIdMap_ =  particleIdMap;
    cellTriaMap_ = cellTriaMap;
    grid_ = grid;
    K_N_= K_N;
    K_T_ = K_T;

}

//Pass 2 particles and return interaction force. Add this force to global force vector of particle.
void dem_core::ParticleParticleInteraction(FluidParticle &f1, FluidParticle &f2, double dt) {
    pair<int, double> status_collision = f1.Collides(f2);

    if(status_collision.first == true) {

         pair<double, double> d1_d2 = f1.GetReducedRadius(status_collision.second, f1);
         Vector C2_o = Vector(f2.GetX(), f2.GetY(), f2.GetZ());
         pair<Vector, Vector> n_C1_o = f1.ContactPlaneNormal(C2_o);
         Vector n = n_C1_o.first;
         Vector C1_o = n_C1_o.second;
         Vector C_o =  f1.CurrentContactPoint(n, status_collision.second, d1_d2.first, C1_o, C2_o);
         pair<double, double> u_N_Epsilon = f1.u_N(C2_o, C1_o, status_collision.second);
         Vector F_N = this->F_N(u_N_Epsilon.first, n);
//         Vector u_T = f1.u_T() //TODO: Implement u_T in grid utils
//         Vector F_T = this->F_T(u_T);
         Vector F_T = Vector(0, 0, 0);
         Vector F = F_N + F_T;
         Vector T1 = Vector::CrossProduct((d1_d2.first * (-n)), F);
         Vector T2 = Vector::CrossProduct((d1_d2.second * n), F);

         //Updating force vector of particles f1 and f2 for current time step
         f1.AddForce(F);
         f2.AddForce(-F);
         f1.AddTorque(T1);
         f2.AddTorque(T2);

    }

}

void dem_core::ParticleTriaInteraction(FluidParticle &f1, Triangle &tria, double dt) {

    Vector intersectionPoint;

    Vector triaNormal = tria.GetNormal(tria);
    triaNormal = Vector::Normalize(triaNormal);

    Vector unitRay;
    unitRay = triaNormal;
    if(Vector::DotProduct(f1.Get_v_half_prev(),triaNormal) < 0) {
         unitRay = -triaNormal;
    }

    pair<float,bool> intersects = tria.RayIntersectsTriangle(f1.GetPosition(), unitRay, tria, intersectionPoint);

    double u_N = 0;
    if(intersects.second) {
        double distToTria = intersects.first;
        if( abs(distToTria) < f1.GetSmoothingRadius()) {
            double u_N = f1.GetSmoothingRadius() - abs(distToTria);
            Vector F_N = this->F_N(u_N, unitRay);

            //TODO: Change this to compute tangential force instead of casting Normal force along reflection of velocity vector
//            Vector unitReflectedVelocity = Vector::Normalize(Vector::Reflection(f1.Get_v_half_prev(),unitRay));
//            double forceNorm = Vector::Norm(F_N);
//            Vector F_N_reflected = forceNorm * unitReflectedVelocity;
            Vector T1 = Vector::CrossProduct(f1.GetSmoothingRadius() * (-unitRay), unitRay);

            f1.AddForce(-F_N);
            f1.AddTorque(T1);
        }
    }

}

//TODO : Inline this
Vector dem_core::F_N(double u_N, Vector n) {
    return ((this->K_N_) * u_N * n);
}

//TODO : Inline this
Vector dem_core::F_T(Vector u_T) {
    //TODO : Internal Friction Angle
    return ((this->K_T_) * u_T);
}

void dem_core::VerletInit(double dt) {
    //Use forward Euler to get v_0.5 to start verlet integrator
    for(auto &pair_particleIdMap : this->particleIdMap_) {
        Vector v_half_init = pair_particleIdMap.second.GetVelocity() + this->GRAVITY_ * (dt/2);
        pair_particleIdMap.second.Set_v_half_prev(v_half_init);
    }
}

pair<Vector, Vector> dem_core::VerletIntegrator(Vector v_half_prev, Vector pos, Vector acc, double dt) {
    Vector v_half_next =  v_half_prev + (acc * dt);
    Vector pos_next = pos + (v_half_next * dt);
    return make_pair(v_half_next,pos_next);
}

void dem_core::ForceReset(){

    for(auto particleIdPair : this->particleIdMap_) {
        particleIdPair.second.SetForce(Vector(0,0,0));
    }

}

void dem_core::StartSim(int n_steps, double dt) {

    this->VerletInit(dt);

    this->WriteStepOutput("OUTPUT", 0, dt);
    for(int i = 1; i <= n_steps; i++) {

        cout << "TIME STEP:" << i << "***************************" << endl;

        cout << "STARTING INTERACTION LOOP FOR STEP : " << i <<endl;
        for(auto &particleIdPair : this->particleIdMap_) {
//            cout << particleIdPair.second.GetSmoothingRadius() << " " << particleIdPair.second.GetMass() << endl;
            Vector F_gravity = particleIdPair.second.GetMass() * this->GRAVITY_;
            particleIdPair.second.AddForce(F_gravity);

            map<int, FluidParticle>::iterator itr = this->particleIdMap_.find(particleIdPair.first);
            advance(itr, 1);

            //Particle particle
            for(itr; itr != this->particleIdMap_.end(); itr++) {
                this->ParticleParticleInteraction(particleIdPair.second, itr->second, dt);
            }

            //Particle Tria
            vector<int> testedTriaIds;
            vector<int> cellId = particleIdPair.second.GetCellIdByPosition(this->grid_.GetXstep(), this->grid_.GetYstep(), this->grid_.GetZstep(), this->grid_.GetGridOrigin());
            vector<vector<int>> testCells = this->grid_.GetNeighbours(cellId);

            testCells.push_back(cellId);

            for(auto cellId : testCells) {
                for(auto tria : this->cellTriaMap_[cellId]) {
                    if(find(testedTriaIds.begin(), testedTriaIds.end(), tria.GetId()) != testedTriaIds.end()) {
                        continue;
                    }
                    this->ParticleTriaInteraction(particleIdPair.second,tria,dt);
                    testedTriaIds.push_back(tria.GetId());
                }
            }

        }

        cout << "STARTING TIME INTEGRATION FOR STEP : " << i << endl;
        for(auto &particleIdPair : this->particleIdMap_) {

            pair<Vector, Vector> v_half_pos_next;
            v_half_pos_next = this->VerletIntegrator(particleIdPair.second.Get_v_half_prev(),particleIdPair.second.GetPosition(), particleIdPair.second.GetForce()*(1/particleIdPair.second.GetMass()),dt);

            particleIdPair.second.Set_v_half_prev(v_half_pos_next.first);
            particleIdPair.second.SetX(v_half_pos_next.second.GetX());
            particleIdPair.second.SetY(v_half_pos_next.second.GetY());
            particleIdPair.second.SetZ(v_half_pos_next.second.GetZ());

            this->ConsoleOutput(particleIdPair.second);

        }

        cout << "TIME STEP:" << i << "***************************" << endl;

        this->WriteStepOutput("OUTPUT", i, dt);
        this->ForceReset();
    }
}

void dem_core::WriteStepOutput(std::string map_filename, int step, double dt) {

    std::ofstream file;
    std::ostringstream fl;
    fl << "./../../../OUTPUTS/PARTICLE_POSITIONS/ParticleLocs_" << map_filename << "_" << step  << ".csv";
    boost::filesystem::path file_ex(fl.str());

    file.open(fl.str(),std::ios_base::app);

    file << "x"  << "," << "y" << "," << "z" << "," << time << std::endl;
    for(auto particleIdPair : this->particleIdMap_) {
        file << particleIdPair.second.GetX()  << "," << particleIdPair.second.GetY() << "," << particleIdPair.second.GetZ() << "," << step*dt << std::endl;
    }

    file.close();
}

void dem_core::ConsoleOutput(FluidParticle &f) {
    cout << "--------------------" << f.GetId() << endl;
    cout << "V half prev : ";
    f.Get_v_half_prev().PrintCoord();

    cout << "Total Force on particle : ";
    f.GetForce().PrintCoord();

    cout << "New position : ";
    f.GetPosition().PrintCoord();
    cout << "--------------------" << f.GetId() << endl;
}


