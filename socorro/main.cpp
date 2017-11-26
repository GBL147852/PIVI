#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "rh.hpp"
#include "sim.hpp"

Mesh mesh_sphere;
Mesh mesh_missile;
Texture texture_angery;
Texture texture_smooth;

void start() {
	//fundão azul
	glClearColor(0.0f,0.0f,0.4f,0.0f);
	
	//câmera
	camera.projection = glm::perspective(
		glm::radians(45.0f),
		16.0f/9.0f,
		.1f,100.0f);
	camera.view = glm::lookAt(
		glm::vec3(-3,1,3), //posição
		glm::vec3(0,3,0), //pra onde tá olhando
		glm::vec3(0,1,0)  //de cabeça pra cima
	);
	
	//nossos troço
	mesh_sphere = rh_create_mesh("sphere");
	mesh_missile = rh_create_mesh("missile");
	texture_angery = rh_create_texture("angery");
	texture_smooth = rh_create_texture("smooth");
	
	sim_radius = 6371000;
	sim_gravity = 9.807;
	sim_projectile_drag = 0.01;
	sim_projectile_area = 1;
	sim_wind_max = 100;
	
	sim_generate_wind_seed();
	
	sim_pos[0] = 0;
	sim_pos[1] = sim_radius;
	sim_pos[2] = 0;
	sim_vel[0] = 15000;
	sim_vel[1] = 500;
	sim_vel[2] = 0;
	
	sim_collide = 0;
}

void end() {
	rh_delete_mesh(mesh_sphere);
	rh_delete_mesh(mesh_missile);
	rh_delete_texture(texture_angery);
	rh_delete_texture(texture_smooth);
}

void update() {
	if (!sim_collide) {
		for (int a = 0; a < 100; a++) sim_step(deltaTime);
	}
	rh_get_input();
}

void render() {
	rh_draw(mesh_sphere,texture_angery,glm::mat4(1));
	glm::mat4 missile_model = glm::scale(glm::mat4(1),glm::vec3(.2f));
	double x = sim_pos[0]/sim_radius;
	double y = sim_pos[1]/sim_radius;
	double z = sim_pos[2]/sim_radius;
	missile_model = glm::inverse(glm::lookAt(
		glm::vec3(x,y,z),
		glm::vec3(x+sim_vel[0],y+sim_vel[1],z+sim_vel[2]),
		glm::vec3(0,-1,0)
	))*missile_model;
	//missile_model = glm::translate(glm::mat4(1),glm::vec3(x,y,z))*missile_model;
	rh_draw(mesh_missile,texture_smooth,missile_model);
	printf("%f\t%f\n",x,y);
}

int main() {
	if (!rh_start()) return -1;
	start();
	while (rh_loop()) {
		update();
		rh_pre_render();
		render();
		rh_post_render();
	}
	end();
	rh_end();
	return 0;
}
