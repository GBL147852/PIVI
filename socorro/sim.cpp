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
double sim_projectile_drag;
double sim_projectile_area;
double sim_wind_max;

//variáveis estocásticas
double sim_wind_seed_u[3];
double sim_wind_seed_v[3];

//estado atual
double sim_pos[3];
double sim_vel[3];

//mais coisas do estado atual
int sim_collide;
double sim_acc_drag[3];
double sim_acc_wind[3];
double sim_acc_gravity[3];

void sim_read_config(const char *path) {
	FILE *file = fopen(path,"r");
	if (file == NULL) {
		printf("n deu pra carregar as config??\n");
		return;
	}
	while (1) {
		char line_header[128];
		if (fscanf(file,"%s",line_header) == EOF) break;
		if (strcmp(line_header,"radius") == 0) {
			fscanf(file,"%lf\n",&sim_radius);
		} else if (strcmp(line_header,"gravity") == 0) {
			fscanf(file,"%lf\n",&sim_gravity);
		} else if (strcmp(line_header,"wind_max") == 0) {
			fscanf(file,"%lf\n",&sim_wind_max);
		} else {
			char buffer[1000];
			fgets(buffer,1000,file);
		}
	}
	fclose(file);
}

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

void sim_step(double dt) {
	//calcula vetores do projétil
	double vector_right[3];
	cross(sim_vel,sim_pos,vector_right);
	double vector_forward[3];
	cross(sim_pos,vector_right,vector_forward);
	normalise(vector_right);
	normalise(vector_forward);
	//gerando aceleração de arrasto
	double drag_factor = sim_projectile_drag*sim_projectile_area*.5;
	sim_acc_drag[0] = -sim_vel[0]*drag_factor;
	sim_acc_drag[1] = -sim_vel[1]*drag_factor;
	sim_acc_drag[2] = -sim_vel[2]*drag_factor;
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
	double gravity_factor = sim_gravity/sim_radius;
	sim_acc_gravity[0] = -sim_pos[0]*gravity_factor;
	sim_acc_gravity[1] = -sim_pos[1]*gravity_factor;
	sim_acc_gravity[2] = -sim_pos[2]*gravity_factor;
	//aplicando aceleração e velocidade
	for (int a = 0; a < 3; a++) {
		sim_vel[a] += (sim_acc_drag[a]+sim_acc_wind[a]+sim_acc_gravity[a])*dt;
		sim_pos[a] += sim_vel[a]*dt;
	}
	//marca flag de colisão caso esteja dentro do raio do planeta
	sim_collide = sim_pos[0]*sim_pos[0]+sim_pos[1]*sim_pos[1]+sim_pos[2]*sim_pos[2] <= sim_radius*sim_radius;
}

void sim_end(double dt) {
	while (!sim_collide) {
		sim_step(dt);
	}
}
