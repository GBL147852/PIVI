#include <stdio.h>
#include "sim.h"

void sim_step(double dt) {
	//aplicando arrasto na velocidade
	double drag_factor = 1-dt*sim_projectile_drag*sim_projectile_area*.5;
	if (drag_factor < 0) drag_factor = 0;
	sim_vel_x *= drag_factor;
	sim_vel_y *= drag_factor;
	sim_vel_z *= drag_factor;
	//aplicando aceleração (distância do ponto pro centro do planeta) na velocidade
	double gravity_factor = dt*sim_gravity/sim_radius;
	sim_vel_x -= sim_pos_x*gravity_factor;
	sim_vel_y -= sim_pos_y*gravity_factor;
	sim_vel_z -= sim_pos_z*gravity_factor;
	//aplicando velocidade na posição
	sim_pos_x += sim_vel_x*dt;
	sim_pos_y += sim_vel_y*dt;
	sim_pos_z += sim_vel_z*dt;
	//marca flag de colisão caso esteja dentro do raio do planeta
	sim_collide = sim_pos_x*sim_pos_x+sim_pos_y*sim_pos_y+sim_pos_z*sim_pos_z <= sim_radius*sim_radius;
}

void sim_end(double dt) {
	while (!sim_collide) {
		sim_step(dt);
	}
}
