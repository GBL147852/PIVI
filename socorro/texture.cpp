#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "texture.hpp"

GLuint LoadTexture(const char *path) {
	//isso vai ser tipo c aparentemente
	unsigned char header[54];
	unsigned int data_pos;
	unsigned int image_size;
	unsigned int width,height;
	unsigned char *data;
	
	//abre o menino
	FILE *file = fopen(("data/textures/"+(std::string(path))+".bmp").c_str(),"rb");
	if (!file) {
		printf("%s n deu\n",path);
		return 0;
	}
	
	//pega o header
	if (fread(header,1,54,file) != 54) {
		printf("o bmp ta errado\n");
		fclose(file);
		return 0;
	}
	
	//tem q ter o início BM
	if (header[0] != 'B' || header[1] != 'M') {
		printf("o bmp ta errado\n");
		fclose(file);
		return 0;
	}
	
	//tem q ser 24bpp
	if (*(int*)&(header[0x1E]) != 0 || *(int*)&(header[0x1C]) != 24) {
		printf("o bmp ta errado\n");
		fclose(file);
		return 0;
	}
	
	//cata o resto q precisa
	data_pos = *(int*)&(header[0x0A]);
	image_size = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	
	//arruma isso tb
	if (image_size == 0) image_size = width*height*3;
	if (data_pos == 0) data_pos = 54;
	
	//monta o buffer e lê
	data = new unsigned char[image_size];
	fread(data,1,image_size,file);
	fclose(file);
	
	//agora sim a parte legal
	GLuint texture_id;
	glGenTextures(1,&texture_id);
	glBindTexture(GL_TEXTURE_2D,texture_id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,data);
	delete[] data;
	
	//nearest é feio
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	
	//trilinear é mais top
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	//monta mip map pro trilinear
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//eh isto
	return texture_id;
}
