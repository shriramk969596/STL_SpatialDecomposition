//
// Created by shriram.krishna on 10/8/20.
//

#include "ParticleGenerator.h"

using namespace std;

ParticleGenerator::ParticleGenerator(double mass, double particle_radius) {
    this->mass_ = mass;
    this->particle_radius_ = particle_radius;
}

void ParticleGenerator::CreateParticleSphere(double radius, double c_x, double c_y, double c_z, double radius_particle) {

}

void ParticleGenerator::CreateParticlesByBbox(double x_min, double y_min, double z_min, double x_max, double y_max, double z_max, double radius_particle, double spacing) {

    double x_length = x_max-x_min;
    double y_length = y_max-y_min;
    double z_length = z_max-z_min;

    int id_counter = 0;
    for(int k = 0; k < (int) (z_length/(2*radius_particle)); k++) {
        for(int j = 0; j < (int)(y_length/(2*radius_particle)); j++) {
            for(int i = 0; i < (int) (x_length/(2*radius_particle)); i++) {

//                cout << i << " " << j << " " << k << endl;
//                cout << (x_min + i*(2*radius_particle) + spacing) << " " << (y_min + j*(2*radius_particle) + spacing) << " " << (z_min + k*(2*radius_particle) + spacing) <<"**" << endl;

                FluidParticle tmp = FluidParticle(x_min + ((double) i *(2*radius_particle) + spacing),
                                                  y_min + ((double) j *(2*radius_particle) + spacing),
                                                  z_min + ((double) k *(2*radius_particle) + spacing),
                                                  id_counter, this->particle_radius_, this->mass_);

                tmp.SetVelocity(20, 20, 20);

                particleIdMap_.insert({id_counter, tmp});
                id_counter++;
            }
        }
    }

}

map<int, FluidParticle>& ParticleGenerator::GetParticles() {
    return particleIdMap_;
}
