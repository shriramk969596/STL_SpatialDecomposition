//
// Created by shriram.krishna on 9/17/20.
//

#ifndef DEM_CORE_H
#define DEM_CORE_H

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include<math.h>
#include <vector>
#include <boost/filesystem.hpp>
#include "grid_utils.h"


class dem_core{

public:

    //constructor
    dem_core(map<int, FluidParticle>& particleIdMap, map<vector<int>, vector<Triangle>, vec_compare> &cellTriaMap,
             Grid &grid, double K_N, double K_T);

    void VerletIntegrator(Vector pos, Vector acc, double dt);
    void ParticleParticleInteraction(FluidParticle &f1, FluidParticle &f2, double d2);
    void ParticleTriaInteraction(FluidParticle &f1, Triangle &tria, double dt);
    Vector F_N(double u_N, Vector n);
    Vector F_T(Vector u_T);
    void VerletInit(double dt);
    pair<Vector, Vector> VerletIntegrator(Vector v_half_prev, Vector pos, Vector acc, double dt);
    void StartSim(int n_steps, double dt);
    void ForceReset();
    void WriteStepOutput(std::string map_filename, int step, double dt);
    void ConsoleOutput(FluidParticle &f);



    double K_N_;
    double K_T_;
    const Vector GRAVITY_ = Vector(0,-9.81,0);


private:

    map<int, FluidParticle>& particleIdMap_;
    map<vector<int>, vector<Triangle>, vec_compare>& cellTriaMap_;
    Grid& grid_;


};

#endif //DEM_CORE_H
