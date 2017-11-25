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
}

void end() {
	rh_delete_mesh(mesh_sphere);
	rh_delete_mesh(mesh_missile);
	rh_delete_texture(texture_angery);
	rh_delete_texture(texture_smooth);
}

void update() {
	rh_get_input();
}

void render() {
	rh_draw(mesh_sphere,texture_angery,glm::scale(glm::mat4(1),glm::vec3(1,1,1)));
	rh_draw(mesh_missile,texture_smooth,glm::translate(glm::mat4(1),glm::vec3(-1.5f,0,0)));
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
