//
// Created by shriram.krishna on 10/8/20.
//

#ifndef AABB_TRIANGLE_INTERSECTION_PARTICLEGENERATOR_H
#define AABB_TRIANGLE_INTERSECTION_PARTICLEGENERATOR_H

#include <stdio.h>
#include "grid_utils.h"

class ParticleGenerator {
    public:
        double mass_ = 0.2;
        double particle_radius_ = 0.1;

        ParticleGenerator(double mass, double particle_radius);

        void CreateParticlesByBbox(double x_min, double y_min, double z_min, double x_max,double y_max, double z_max,double radius_particle, double spacing);
        void CreateParticleSphere(double radius, double c_x, double c_y, double c_z, double radius_particle);
        map<int, FluidParticle>& GetParticles();



    private:

        map<int, FluidParticle> particleIdMap_;
};


#endif //AABB_TRIANGLE_INTERSECTION_PARTICLEGENERATOR_H
