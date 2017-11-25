#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"

std::string LoadFile(const char *path) {
	std::string str;
	std::ifstream stream(("data/shaders/"+(std::string(path))).c_str(),std::ios::in);
	if (stream.is_open()) {
		stream.seekg(0,std::ios::end);   
		str.reserve(stream.tellg());
		stream.seekg(0,std::ios::beg);
		str.assign((std::istreambuf_iterator<char>(stream)),std::istreambuf_iterator<char>());
		stream.close();
	}
	return str;
}

GLuint LoadShaders(const char *vertex_path,const char *fragment_path) {
	//cria os menino
	GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
	
	//carrega do arquivo
	std::string vertex_code = LoadFile(vertex_path);
	std::string fragment_code = LoadFile(fragment_path);
	
	GLint status = GL_FALSE;
	int info_log_length;
	
	//compila o vertex
	const char *vertex_code_c = vertex_code.c_str();
	glShaderSource(vertex_id,1,&vertex_code_c,NULL);
	glCompileShader(vertex_id);
	glGetShaderiv(vertex_id,GL_COMPILE_STATUS,&status);
	glGetShaderiv(vertex_id,GL_INFO_LOG_LENGTH,&info_log_length);
	if (info_log_length > 0) {
		std::vector<char> message(info_log_length+1);
		glGetShaderInfoLog(vertex_id,info_log_length,NULL,&message[0]);
		printf("%s\n",&message[0]);
	}
	
	//compila o fragment
	const char *fragment_code_c = fragment_code.c_str();
	glShaderSource(fragment_id,1,&fragment_code_c,NULL);
	glCompileShader(fragment_id);
	glGetShaderiv(fragment_id,GL_COMPILE_STATUS,&status);
	glGetShaderiv(fragment_id,GL_INFO_LOG_LENGTH,&info_log_length);
	if (info_log_length > 0) {
		std::vector<char> message(info_log_length+1);
		glGetShaderInfoLog(fragment_id,info_log_length,NULL,&message[0]);
		printf("%s\n",&message[0]);
	}
	
	//cria programinha e bota eles
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id,vertex_id);
	glAttachShader(program_id,fragment_id);
	glLinkProgram(program_id);
	
	//testa programinha
	glGetProgramiv(program_id,GL_LINK_STATUS,&status);
	glGetProgramiv(program_id,GL_INFO_LOG_LENGTH,&info_log_length);
	if (info_log_length > 0) {
		std::vector<char> message(info_log_length+1);
		glGetProgramInfoLog(program_id,info_log_length,NULL,&message[0]);
		printf("%s\n",&message[0]);
	}
	
	//tira tudo ta blz
	glDetachShader(program_id,vertex_id);
	glDetachShader(program_id,fragment_id);
	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
	
	return program_id;
}
