#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "texture.hpp"

GLuint LoadBMP(const char *path) {
	//isso vai ser tipo c aparentemente
	unsigned char header[54];
	unsigned int data_pos;
	unsigned int image_size;
	unsigned int width,height;
	unsigned char *data;
	
	//abre o menino
	FILE *file = fopen(path,"rb");
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

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint LoadDDS(const char *path) {
	unsigned char filecode[4];
	unsigned char header[124];
	unsigned int width,height,linear_size,mip_map_count,four_cc;
	unsigned char* data;
	
	//abre o menino
	FILE *file = fopen(path,"rb");
	if (!file) {
		printf("%s n deu\n",path);
		return 0;
	}
	
	//cata o código
	fread(filecode,1,4,file);
	if (filecode[0] != 'D' || filecode[1] != 'D' || filecode[2] != 'S' || filecode[3] != ' ') {
		fclose(file);
		return 0;
	}
	
	//cata as coisa
	fread(&header,124,1,file);
	height = *(unsigned int*)&(header[8]);
	width = *(unsigned int*)&(header[12]);
	linear_size = *(unsigned int*)&(header[16]);
	mip_map_count = *(unsigned int*)&(header[24]);
	four_cc = *(unsigned int*)&(header[80]);
	
	//carrega data (com espaço extra pros mipmaps)
	unsigned int data_length = mip_map_count > 1 ? linear_size * 2 : linear_size;
	data = new unsigned char[data_length];
	fread(data,1,data_length,file);
	fclose(file);
	
	unsigned int components = four_cc == FOURCC_DXT1 ? 3 : 4;
	unsigned int format;
	switch (four_cc) {
		case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
		case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
		case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
		default: delete[] data; return 0;
	}
	
	//agora sim a parte legal
	GLuint texture_id;
	glGenTextures(1,&texture_id);
	glBindTexture(GL_TEXTURE_2D,texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
	unsigned int blockSize = format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;
	unsigned int offset = 0;
	
	//bota tudo com os mipmap
	for (unsigned int level = 0; level < mip_map_count && (width || height); level++) {
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D,level,format,width,height,0,size,data+offset);
		offset += size;
		width /= 2;
		height /= 2;
		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}
	
	//eh isto
	delete[] data;
	return texture_id;
}
