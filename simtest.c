#include <stdio.h>
#include "sim.h"

int main() {
	sim_radius = 6371000;
	sim_gravity = 9.807;
	sim_collide = 0;
	sim_pos_x = 0;
	sim_pos_y = sim_radius;
	sim_pos_z = 0;
	sim_vel_x = 5000;
	sim_vel_y = 500;
	sim_vel_z = 0;
	sim_projectile_drag = 0.06;
	sim_projectile_area = 1;
	int a = 0;
	while (!sim_collide) {
		sim_step(1.0/60.0);
		if (!(a--)) {
			printf("%f\t%f\n",sim_pos_x,sim_pos_y);
			a = 20;
		}
	}
}
