#pragma once

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

//gera valores pras variáveis estocásticas
void sim_generate_wind_seed();
//realiza um passo da simulação, dado um delta time
void sim_step(double dt);
//simula até colidir, dado um delta time padrão pra cada passo
void sim_end(double dt);
