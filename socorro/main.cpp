#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "rh.hpp"
#include "sim.hpp"

Mesh mesh_sphere;
Mesh mesh_missile;
Mesh mesh_trace;
Mesh mesh_trace_o;
Mesh mesh_trace_x;
Texture texture_earth;
Texture texture_missile;
Texture texture_trace;

float zoom = 3;

int simulation_speed = 100;
bool simulation_running = false;
bool simulation_ended = false;

double simulation_start_pos[3];
double simulation_start_vel[3];

std::vector<glm::mat4> simulation_trace;
std::vector<glm::vec3> simulation_trace_pos;
glm::mat4 simulation_trace_o;
glm::mat4 simulation_trace_x;
std::vector<glm::mat4> simulation_history;

glm::vec3 missile_pos;
glm::mat4 missile_mat;

bool press_space = false;
bool press_enter = false;
bool press_backspace = false;

// INPUT STATS
// Initial position : on +Z
glm::vec3 position( 0, 0, zoom ); 
// Initial horizontal angle : toward -Z
float horizontal_angle = 0.0f;
// Initial vertical angle : none
float vertical_angle = 0.0f;
// Initial Field of View
float fov = 60.0f;

float speed = 3.0f; // 3 units / second
float mouse_speed = 3.0f;
float vertical_limit = PI*0.49f;

bool first = true;

void simulation_start();
void calc_missile();

void start() {
	//fundão marelo
	glClearColor(236.0f/255.0f,230.0f/255.0f,202.0f/255.0f,0.0f);
	
	//nossos troço
	mesh_sphere = rh_create_mesh("sphere");
	mesh_missile = rh_create_mesh("missile");
	mesh_trace = rh_create_mesh("trace");
	mesh_trace_o = rh_create_mesh("trace_o");
	mesh_trace_x = rh_create_mesh("trace_x");
	texture_earth = rh_create_texture("earth");
	texture_missile = rh_create_texture("missile");
	texture_trace = rh_create_texture("trace");
	
	//configura simulação
	sim_radius = 6371000;
	sim_gravity = 9.807;
	sim_projectile_drag = 0;
	sim_projectile_area = 1;
	sim_wind_max = 200;
	sim_read_config("data/config.txt");
	calc_missile();
	
	simulation_start_pos[0] = 0;
	simulation_start_pos[1] = 0;
	simulation_start_pos[2] = sim_radius;
	simulation_start_vel[0] = 6000;
	simulation_start_vel[1] = 0;
	simulation_start_vel[2] = 1000;
}

void end() {
	rh_delete_mesh(mesh_sphere);
	rh_delete_mesh(mesh_missile);
	rh_delete_mesh(mesh_trace);
	rh_delete_mesh(mesh_trace_o);
	rh_delete_mesh(mesh_trace_x);
	rh_delete_texture(texture_earth);
	rh_delete_texture(texture_missile);
	rh_delete_texture(texture_trace);
}

glm::mat4 get_ground(double pos[3]) {
	glm::vec3 direction = glm::normalize(glm::vec3(pos[0],pos[1],pos[2]));
	return glm::translate(glm::mat4(1),direction)*rh_rot(glm::vec3(0,1,0),direction);
}

void calc_missile() {
	missile_pos = glm::vec3(sim_pos[0]/sim_radius,sim_pos[1]/sim_radius,sim_pos[2]/sim_radius);
	missile_mat = glm::inverse(glm::lookAt(
		glm::vec3(missile_pos.x,missile_pos.y,missile_pos.z),
		glm::vec3(missile_pos.x+sim_vel[0],missile_pos.y+sim_vel[1],missile_pos.z+sim_vel[2]),
		glm::vec3(0,-1,0)
	));
}

void simulation_start() {
	sim_generate_wind_seed();
	
	sim_pos[0] = simulation_start_pos[0];
	sim_pos[1] = simulation_start_pos[1];
	sim_pos[2] = simulation_start_pos[2];
	sim_vel[0] = simulation_start_vel[0];
	sim_vel[1] = simulation_start_vel[1];
	sim_vel[2] = simulation_start_vel[2];
	sim_collide = 0;
	
	simulation_running = true;
	simulation_ended = false;
	simulation_trace_pos.push_back(missile_pos);
}

void simulation_stop() {
	simulation_running = false;
	simulation_ended = false;
	simulation_trace.clear();
	simulation_trace_pos.clear();
}

void simulation_end() {
	simulation_running = false;
	simulation_ended = true;
	simulation_trace_x = get_ground(sim_pos);
	simulation_history.push_back(simulation_trace_x);
}

void simulation_clear() {
	simulation_history.clear();
}

bool update() {
	//já fecha se pressionar esc etc
	if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return false;
	}
	
	//cata tamanho da tela e reajusta se necessário. também cata o rato
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
	horizontal_angle -= mouse_speed * mouse_x;
	vertical_angle   += mouse_speed * mouse_y;
	
	// Go up
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		// position.z += deltaTime * speed;
		vertical_angle -= speed * deltaTime;
	}
	// Go Down
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		// position.z -= deltaTime * speed;
		vertical_angle += speed * deltaTime;
	}
	// Go right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		// position.x -= deltaTime * speed;
		horizontal_angle -= speed * deltaTime;
	}
	// Go left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		// position.x += deltaTime * speed;
		horizontal_angle += speed * deltaTime;
	}
	// Zoom in
	if(glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		zoom -= speed * deltaTime;
	}
	// Zoom out
	if(glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		zoom += speed * deltaTime;
	}
	
	// Spooky smooth
	if(vertical_angle < -vertical_limit) vertical_angle = -vertical_limit;
	if(vertical_angle > vertical_limit) vertical_angle = vertical_limit;
	
	if(zoom < 2) zoom = 2;
	if(zoom > 5) zoom = 5;
	
	position.x = cos(vertical_angle) * sin(horizontal_angle);
	position.y = sin(vertical_angle);
	position.z = cos(vertical_angle) * cos(horizontal_angle);
	
	// Camera matrix
	camera.view = glm::lookAt(
		position*zoom,		// A camera ta aqui
		glm::vec3(0,0,0),	// olhando pro centro
		glm::vec3(0,1,0)	// olhando pra cima
	);
	
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1)) {
		if (!simulation_running) {
			simulation_clear();
			simulation_start_pos[0] = position.x*sim_radius;
			simulation_start_pos[1] = position.y*sim_radius;
			simulation_start_pos[2] = position.z*sim_radius;
		}
	}
	
	//hmm
	if (glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (!press_space) {
			press_space = true;
			if (simulation_running || simulation_ended) {
				simulation_stop();
			} else {
				simulation_start();
			}
		}
	} else {
		press_space = false;
	}
	bool fastForward = false;
	if (glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS) {
		if (!press_enter) {
			press_enter = true;
			fastForward = true;
			if (simulation_ended) {
				simulation_stop();
			}
			if (!simulation_running) {
				simulation_start();
			}
		}
	} else {
		press_enter = false;
	}
	if (glfwGetKey(window,GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		if (!press_backspace) {
			press_backspace = true;
			if (simulation_ended) {
				simulation_stop();
			} else {
				simulation_clear();
			}
		}
	} else {
		press_backspace = false;
	}
	
	//ok bora simular
	bool calc_missile_done = false;
	if (simulation_running) {
		do {
			for (int a = 0; a < simulation_speed; a++) {
				sim_step(deltaTime);
				if (sim_collide) {
					if (!calc_missile_done) {
						calc_missile_done = true;
						calc_missile();
					}
					simulation_end();
					break;
				}
			}
			calc_missile_done = true;
			calc_missile();
			if (glm::distance(simulation_trace_pos.back(),missile_pos) > 0.05f) {
				simulation_trace.push_back(missile_mat);
				simulation_trace_pos.push_back(missile_pos);
			}
		} while (simulation_running && fastForward);
	}
	if (!calc_missile_done) {
		calc_missile_done = true;
		calc_missile();
	}
	simulation_trace_o = get_ground(simulation_start_pos);
	
	//desenha terra
	rh_draw(mesh_sphere,texture_earth,glm::mat4(1));
	
	//desenha míssil
	if (simulation_running) {
		glm::mat4 missile_model = missile_mat*glm::scale(glm::mat4(1),glm::vec3(.1f));
		rh_draw(mesh_missile,texture_missile,missile_model);
	}
	
	//desenha ponto de início
	rh_draw(mesh_trace_o,texture_trace,simulation_trace_o*glm::scale(glm::mat4(1),glm::vec3(.025f)));
	
	//desenha ponto de fim
	if (simulation_ended) {
		rh_draw(mesh_trace_x,texture_trace,simulation_trace_x*glm::scale(glm::mat4(1),glm::vec3(.025f)));
	}
	
	//desenha rastros
	for (int a = 0; a < simulation_trace.size(); a++) {
		rh_draw(mesh_trace,texture_trace,simulation_trace[a]*glm::scale(glm::mat4(1),glm::vec3(.05f,.05f,.025f)));
	}
	
	//desenha histórico de simulações (pontos finais)
	for (int a = 0; a < simulation_history.size(); a++) {
		rh_draw(mesh_trace_o,texture_trace,simulation_history[a]*glm::scale(glm::mat4(1),glm::vec3(.005f)));
	}
	
	return true;
}

int main() {
	if (!rh_start()) return -1;
	start();
	first = true;
	while (rh_loop()) {
		if (!update()) break;
		rh_post_loop();
		first = false;
	}
	end();
	rh_end();
	return 0;
}
