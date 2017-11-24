#pragma once

double sim_pos_x,sim_pos_y,sim_pos_z;
double sim_vel_x,sim_vel_y,sim_vel_z;

double sim_radius;
double sim_gravity;
int sim_collide;

double sim_projectile_drag;
double sim_projectile_area;

void sim_step(double dt);
void sim_end(double dt);
