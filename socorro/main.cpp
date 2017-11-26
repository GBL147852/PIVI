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
Texture texture_smooth;
Texture texture_earth;

float zoom = 3;

// INPUT STATS
// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, zoom ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float fov = 60.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 3.0f;
float verticalLimit = PI*0.49f;

int first = 1;

void start() {
	//fundão azul
	glClearColor(236.0f/255.0f,230.0f/255.0f,202.0f/255.0f,0.0f);
	
	//nossos troço
	mesh_sphere = rh_create_mesh("sphere");
	mesh_missile = rh_create_mesh("missile");
	texture_smooth = rh_create_texture("smooth");
	texture_earth = rh_create_texture("earth");
	
	//inicia simulação
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
	rh_delete_texture(texture_smooth);
	rh_delete_texture(texture_earth);
}

bool update() {
	if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return false;
	}
	
	if (!sim_collide) {
		for (int a = 0; a < 100; a++) sim_step(deltaTime);
	}
	int width,height;
	glfwGetWindowSize(window,&width,&height);
	double mouse_x,mouse_y;
	if (first) {
		mouse_x = 0;
		mouse_y = 0;
	} else {
		glfwGetCursorPos(window,&mouse_x,&mouse_y);
		mouse_x = (mouse_x-(width/2))/height;
		mouse_y = (mouse_y-(height/2))/height;
	}
	glfwSetCursorPos(window,width/2,height/2);
	camera.projection = glm::perspective(
		glm::radians(fov),
		(float)width/height,
		.1f,100.0f
	);
	
	// Compute new orientation
	horizontalAngle -= mouseSpeed * mouse_x;
	verticalAngle   += mouseSpeed * mouse_y;
	
	// Go up
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		// position.z += deltaTime * speed;
		verticalAngle -= speed * deltaTime;
	}
	// Go Down
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		// position.z -= deltaTime * speed;
		verticalAngle += speed * deltaTime;
	}
	// Go right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		// position.x -= deltaTime * speed;
		horizontalAngle -= speed * deltaTime;
	}
	// Go left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		// position.x += deltaTime * speed;
		horizontalAngle += speed * deltaTime;
	}
	// Zoom in
	if(glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		zoom -= speed * deltaTime;
	}
	// Zoom out
	if(glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		zoom += speed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
		//
	}
	
	// Spooky smooth
	if(verticalAngle < -verticalLimit) verticalAngle = -verticalLimit;
	if(verticalAngle > verticalLimit) verticalAngle = verticalLimit;
	
	if(zoom < 2) zoom = 2;
	if(zoom > 5) zoom = 5;
	
	position.x = zoom * cos(verticalAngle) * sin(horizontalAngle);
	position.y = zoom * sin(verticalAngle);
	position.z = zoom * cos(verticalAngle) * cos(horizontalAngle);
	
	// Camera matrix
	camera.view = glm::lookAt(
		position,           // A camera ta aqui
		glm::vec3(0,0,0), 	// olhando pro centro
		glm::vec3(0,1,0)    // olhando pra cima
	);
	
	return true;
}

void render() {
	rh_draw(mesh_sphere,texture_earth,glm::mat4(1));
	glm::mat4 missile_model = glm::scale(glm::mat4(1),glm::vec3(.1f));
	double x = sim_pos[0]/sim_radius;
	double y = sim_pos[1]/sim_radius;
	double z = sim_pos[2]/sim_radius;
	missile_model = glm::inverse(glm::lookAt(
		glm::vec3(x,y,z),
		glm::vec3(x+sim_vel[0],y+sim_vel[1],z+sim_vel[2]),
		glm::vec3(0,-1,0)
	))*missile_model;
	rh_draw(mesh_missile,texture_smooth,missile_model);
}

int main() {
	if (!rh_start()) return -1;
	start();
	first = 1;
	while (rh_loop()) {
		if (!update()) break;
		rh_pre_render();
		render();
		rh_post_render();
		first = 0;
	}
	end();
	rh_end();
	return 0;
}
