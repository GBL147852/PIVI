#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "sim.hpp"
#include "perlin.hpp"

#define PERLIN_SCALE 0.01

//propriedades
double sim_radius;
double sim_gravity;
double sim_wind_max;

//variáveis estocásticas
double sim_wind_seed_u[3];
double sim_wind_seed_v[3];

//estado atual
double sim_pos[3];
double sim_vel[3];

//mais coisas do estado atual
double sim_time;
int sim_collide;
double sim_acc_wind[3];
double sim_acc_gravity[3];

void sim_generate_wind_seed() {
	srand48(clock());
	sim_wind_seed_u[0] = drand48()*256;
	sim_wind_seed_u[1] = drand48()*256;
	sim_wind_seed_u[2] = drand48()*256;
	sim_wind_seed_v[0] = drand48()*256;
	sim_wind_seed_v[1] = drand48()*256;
	sim_wind_seed_v[2] = drand48()*256;
}

void cross(double a[3],double b[3],double v[3]) {
	v[0] = a[1]*b[2]-a[2]*b[1];
	v[1] = a[2]*b[0]-a[0]*b[2];
	v[2] = a[0]*b[1]-a[1]*b[0];
}

void normalise(double v[3]) {
	double factor = 1.0/sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0] *= factor;
	v[1] *= factor;
	v[2] *= factor;
}

double sqr_radius() {
	return sim_pos[0]*sim_pos[0]+sim_pos[1]*sim_pos[1]+sim_pos[2]*sim_pos[2];
}

void sim_step(double dt) {
	sim_time += dt;
	//calcula vetores do projétil
	double vector_right[3];
	cross(sim_vel,sim_pos,vector_right);
	double vector_forward[3];
	cross(sim_pos,vector_right,vector_forward);
	normalise(vector_right);
	normalise(vector_forward);
	//gerando aceleração de vento (perlin noise)
	double wind_u = sim_wind_max*perlin_noise(
		sim_pos[0]*PERLIN_SCALE+sim_wind_seed_u[0],
		sim_pos[1]*PERLIN_SCALE+sim_wind_seed_u[1],
		sim_pos[2]*PERLIN_SCALE+sim_wind_seed_u[2]
	);
	double wind_v = sim_wind_max*perlin_noise(
		sim_pos[0]*PERLIN_SCALE+sim_wind_seed_v[0],
		sim_pos[1]*PERLIN_SCALE+sim_wind_seed_v[1],
		sim_pos[2]*PERLIN_SCALE+sim_wind_seed_v[2]
	);
	sim_acc_wind[0] = vector_forward[0]*wind_u+vector_right[0]*wind_v;
	sim_acc_wind[1] = vector_forward[1]*wind_u+vector_right[1]*wind_v;
	sim_acc_wind[2] = vector_forward[2]*wind_u+vector_right[2]*wind_v;
	//gerando aceleração de gravidade
	double current_sqr_radius = sqr_radius();
	double current_gravity = sim_radius*sim_radius*sim_gravity/current_sqr_radius;
	double current_radius = sqrt(current_sqr_radius);
	sim_acc_gravity[0] = -sim_pos[0]*current_gravity/current_radius;
	sim_acc_gravity[1] = -sim_pos[1]*current_gravity/current_radius;
	sim_acc_gravity[2] = -sim_pos[2]*current_gravity/current_radius;
	//aplicando aceleração e velocidade
	for (int a = 0; a < 3; a++) {
		sim_vel[a] += (sim_acc_wind[a]+sim_acc_gravity[a])*dt;
		sim_pos[a] += sim_vel[a]*dt;
	}
	//marca flag de colisão caso esteja dentro do raio do planeta
	sim_collide = sqr_radius() <= sim_radius*sim_radius;
}

void sim_end(double dt) {
	while (!sim_collide) {
		sim_step(dt);
	}
}
