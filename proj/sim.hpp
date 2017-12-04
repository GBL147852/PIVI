#ifndef SIM_HPP
#define SIM_HPP

//propriedades
extern double sim_radius;
extern double sim_gravity;
extern double sim_wind_max;

//variáveis estocásticas
extern double sim_wind_seed_u[3];
extern double sim_wind_seed_v[3];

//estado atual
extern double sim_pos[3];
extern double sim_vel[3];

//mais coisas do estado atual
extern double sim_time;
extern int sim_collide;
extern double sim_acc_wind[3];
extern double sim_acc_gravity[3];

//gera valores pras variáveis estocásticas
void sim_generate_wind_seed();
//realiza um passo da simulação, dado um delta time
void sim_step(double dt);
//simula até colidir, dado um delta time padrão pra cada passo
void sim_end(double dt);

#endif
