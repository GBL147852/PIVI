#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "rh.hpp"

GLFWwindow* window;
Camera camera;
float totalTime,deltaTime;

GLuint vertex_array_id;
GLuint shaders_program_id;

GLuint matrix_mvp_id;
GLuint matrix_mv_id;

double firstTime,lastTime;

Mesh rh_create_mesh(const char *path) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = LoadMesh(path,vertices,uvs,normals);
	
	Mesh mesh;
	mesh.count = vertices.size();
	
	glGenBuffers(1,&mesh.vertex);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.vertex);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(glm::vec3),&vertices[0],GL_STATIC_DRAW);
	
	glGenBuffers(1,&mesh.uv);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.uv);
	glBufferData(GL_ARRAY_BUFFER,uvs.size()*sizeof(glm::vec2),&uvs[0],GL_STATIC_DRAW);
	
	glGenBuffers(1,&mesh.normal);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.normal);
	glBufferData(GL_ARRAY_BUFFER,normals.size()*sizeof(glm::vec3),&normals[0],GL_STATIC_DRAW);
	
	return mesh;
}

void rh_delete_mesh(Mesh mesh) {
	glDeleteBuffers(1,&mesh.vertex);
	glDeleteBuffers(1,&mesh.uv);
	glDeleteBuffers(1,&mesh.normal);
}

Texture rh_create_texture(const char *path) {
	Texture texture;
	texture.id = LoadTexture(path);
	return texture;
}

void rh_delete_texture(Texture texture) {
	glDeleteTextures(1,&texture.id);
}

bool rh_start() {
	//inicia o guri
	if (!glfwInit()) {
		fprintf(stderr,"glfw n deu\n");
		return false;
	}
	
	//configurações bla
	glfwWindowHint(GLFW_SAMPLES,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	//oi janela
	window = glfwCreateWindow(1280,720,"aaa",NULL,NULL);
	if (window == NULL) {
		fprintf(stderr,"janela n foi\n");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	//oi glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr,"glew n foi\n");
		glfwTerminate();
		return false;
	}
	
	//bota as tecla
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	//bota o rato
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	
	//fundão
	glClearColor(0,0,0,0);
	
	//depth test sim pfv
	glEnable(GL_DEPTH_TEST);
	//de acordo com profundidade tb ne
	glDepthFunc(GL_LESS);
	
	glGenVertexArrays(1,&vertex_array_id);
	glBindVertexArray(vertex_array_id);
	
	//carrega os shaders la
	shaders_program_id = LoadShaders("vert","frag");
	glUseProgram(shaders_program_id);
	
	//bota textura no shader
	GLuint texture_id = glGetUniformLocation(shaders_program_id,"tex");
	glUniform1i(texture_id,0);
	glActiveTexture(GL_TEXTURE0);
	
	//inicializa valores de câmera
	matrix_mvp_id = glGetUniformLocation(shaders_program_id,"mvp");
	matrix_mv_id = glGetUniformLocation(shaders_program_id,"mv");
	// camera.projection = glm::perspective(glm::radians(45.0f),16.0f/9.0f,.1f,100.0f);
	// camera.view = glm::mat4(1);+
	
	//arruma o time tb
	lastTime = firstTime = glfwGetTime();
	totalTime = 0;
	deltaTime = 0.0001;
	firstTime -= deltaTime;
	
	//ae
	return true;
}

bool rh_loop() {
	if (glfwWindowShouldClose(window)) return false;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

void rh_post_loop() {
	//troca os buffers e cata evento
	glfwSwapBuffers(window);
	glfwPollEvents();
	
	//arruma o time dnv
	double now = glfwGetTime();
	totalTime = now-firstTime;
	deltaTime = now-lastTime;
	lastTime = now;
}

void rh_end() {
	glDeleteProgram(shaders_program_id);
	glDeleteVertexArrays(1,&vertex_array_id);
	
	//flw glfw
	glfwTerminate();
}

void rh_draw(Mesh mesh,Texture texture,glm::mat4 model) {
	//passa infos pro shader (matriz pro MVP, textura pro TEXTURE0)
	glm::mat4 mv = camera.view*model;
	glm::mat4 mvp = camera.projection*mv;
	//mv = glm::transpose(glm::inverse(mv));
	glUniformMatrix4fv(matrix_mvp_id,1,GL_FALSE,&mvp[0][0]);
	glUniformMatrix4fv(matrix_mv_id,1,GL_FALSE,&mv[0][0]);
	glBindTexture(GL_TEXTURE_2D,texture.id);
	
	//os vértices são o 0
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.vertex);
	glVertexAttribPointer(
		0,			//atributo. tem q combinar com o shader
		3,			//comprimento. xyz = 2
		GL_FLOAT,	//tipo
		GL_FALSE,	//tá normalizado?
		0,			//stride
		(void*)0	//offset no array
	);
	
	//os uvs são o 1
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.uv);
	glVertexAttribPointer(
		1,			//atributo. tem q combinar com o shader
		2,			//comprimento. uv = 2
		GL_FLOAT,	//tipo
		GL_FALSE,	//tá normalizado?
		0,			//stride
		(void*)0	//offset no array
	);
	
	//os uvs são o 1
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.normal);
	glVertexAttribPointer(
		2,			//atributo. tem q combinar com o shader
		3,			//comprimento. xyz = 3
		GL_FLOAT,	//tipo
		GL_FALSE,	//tá normalizado?
		0,			//stride
		(void*)0	//offset no array
	);
	
	//desenha como triângulos
	glDrawArrays(GL_TRIANGLES,0,mesh.count);
	
	//tira essas coisas
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

quat rh_rot_quat(vec3 start,vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);
	float cosTheta = dot(start,dest);
	vec3 rotationAxis;
	if (cosTheta < -1 + 0.001f) {
		rotationAxis = cross(vec3(0,0,1),start);
		if (glm::length(rotationAxis) < 0.01) {
			rotationAxis = cross(vec3(1,0,0),start);
		}
		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f),rotationAxis);
	}
	rotationAxis = cross(start,dest);
	float s = sqrt((1+cosTheta)*2);
	float invs = 1/s;
	return quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

glm::mat4 rh_rot(vec3 start,vec3 dest) {
	return glm::mat4_cast(rh_rot_quat(start,dest));
}
