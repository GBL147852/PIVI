#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"

int main() {
	//inicia o guri
	if (!glfwInit()) {
		fprintf(stderr,"glfw n deu\n");
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	//oi janela
	window = glfwCreateWindow(960,540,"aaa",NULL,NULL);
	if (window == NULL) {
		fprintf(stderr,"janela n foi\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//oi glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr,"glew n foi\n");
		return -1;
	}
	
	//bota as tecla
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	
	//fundão azul
	glClearColor(0.0f,0.0f,0.4f,0.0f);
	
	//depth test sim pfv
	glEnable(GL_DEPTH_TEST);
	//de acordo com profundidade tb ne
	glDepthFunc(GL_LESS); 
	
	GLuint vertex_array_id;
	glGenVertexArrays(1,&vertex_array_id);
	glBindVertexArray(vertex_array_id);
	
	//carrega os shaders la
	GLuint shaders_program_id = LoadShaders("vert","frag_texture");
	
	//já marca o nome do parâmetro q vai passar pro shader
	GLuint matrix_id = glGetUniformLocation(shaders_program_id,"MVP");

	//matriz projection: 45 graus de visão, proporção 16:9, de 0.1 a 100 unidades
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),16.0f/9.0f,.1f,100.0f);
	//matriz view
	glm::mat4 view = glm::lookAt(
		glm::vec3(-4,3,3), //câmera em (4,3,3)
		glm::vec3(0,0,0), //olhando pro meio
		glm::vec3(0,1,0)  //com a cabeça pra cima
	);
	//matriz model: modelo tá normalzão, então identidade
	glm::mat4 model = glm::mat4(1.0f);
	//junta isso tudo
	glm::mat4 mvp = projection*view*model; // Remember, matrix multiplication is the other way around

	//carrega a textura
	GLuint texture = LoadTexture("angery");
	
	//faz o sample
	GLuint texture_id = glGetUniformLocation(shaders_program_id,"myTextureSampler");
	
	//carrega a mesh
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = LoadMesh("sphere",vertices,uvs,normals);
	
	GLuint vertex_buffer;
	glGenBuffers(1,&vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(glm::vec3),&vertices[0],GL_STATIC_DRAW);
	
	GLuint uv_buffer;
	glGenBuffers(1,&uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER,uv_buffer);
	glBufferData(GL_ARRAY_BUFFER,uvs.size()*sizeof(glm::vec2),&uvs[0],GL_STATIC_DRAW);
	
	while (!glfwWindowShouldClose(window)) {
		//limpa a tela
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//bota nosso shader lindo
		glUseProgram(shaders_program_id);
		
		//passa infos pro shader (matriz pro MVP, textura pro TEXTURE0)
		glUniformMatrix4fv(matrix_id,1,GL_FALSE,&mvp[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texture);
		glUniform1i(texture_id,0);
		
		//os vértices são o 0
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		glVertexAttribPointer(
			0,                  // atributo. tem q combinar com o shader
			3,                  // comprimento. xyz = 2
			GL_FLOAT,           // tipo
			GL_FALSE,           // tá normalizado?
			0,                  // stride
			(void*)0            // offset no array
		);
		
		//os uvs são o 1
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER,uv_buffer);
		glVertexAttribPointer(
			1,                                // atributo. tem q combinar com o shader
			2,                                // comprimento. uv = 2
			GL_FLOAT,                         // tipo
			GL_FALSE,                         // tá normalizado?
			0,                                // stride
			(void*)0                          // offset no array
		);
		
		//desenha como triângulos
		glDrawArrays(GL_TRIANGLES,0,vertices.size());
		
		//tira essas coisas
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//troca os buffers (mostra na tela as coisa)
		glfwSwapBuffers(window);
		
		//eventossss
		glfwPollEvents();
		if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}
	}
	
	//limpa tudo de buffer e sla oq
	glDeleteBuffers(1,&vertex_buffer);
	glDeleteBuffers(1,&uv_buffer);
	glDeleteProgram(shaders_program_id);
	glDeleteTextures(1,&texture);
	glDeleteVertexArrays(1,&vertex_array_id);
	
	//flw glfw
	glfwTerminate();
	
	return 0;
}
