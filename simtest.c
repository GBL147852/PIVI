#include <stdio.h>
#include "sim.h"

int main() {
	sim_radius = 6371000;
	sim_gravity = 9.807;
	sim_projectile_drag = 0.06;
	sim_projectile_area = 1;
	sim_wind_max = 100;
	
	sim_generate_wind_seed();
	
	sim_pos[0] = 0;
	sim_pos[1] = sim_radius;
	sim_pos[2] = 0;
	sim_vel[0] = 3500;
	sim_vel[1] = 500;
	sim_vel[2] = 0;
	
	int a = 0;
	do {
		sim_step(1.0/60.0);
		if (!(a--)) {
			printf("%f\t%f\n",sim_pos[0],sim_pos[1]);
			a = 20;
		}
	} while (!sim_collide);
}
