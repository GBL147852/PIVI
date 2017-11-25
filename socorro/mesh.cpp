#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "mesh.hpp"

bool LoadMesh(
	const char *path,
	std::vector<glm::vec3> &out_vertices,
	std::vector<glm::vec2> &out_uvs,
	std::vector<glm::vec3> &out_normals
){
	std::vector<unsigned int> vertex_indices,uv_indices,normal_indices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	
	FILE *file = fopen(("data/meshes/"+(std::string(path))+".obj").c_str(),"r");
	if (file == NULL) {
		printf("n deu pra carregar o modelo\n");
		getchar();
		return false;
	}
	
	while (1) {
		char line_header[128];
		if (fscanf(file,"%s",line_header) == EOF) break;
		if (strcmp(line_header,"v") == 0) {
			//vértice
			glm::vec3 vertex;
			fscanf(file,"%f %f %f\n",&vertex.x,&vertex.y,&vertex.z);
			temp_vertices.push_back(vertex);
		} else if (strcmp(line_header,"vt") == 0) {
			//uv
			glm::vec2 uv;
			fscanf(file,"%f %f\n",&uv.x,&uv.y);
			temp_uvs.push_back(uv);
		} else if (strcmp(line_header,"vn") == 0) {
			//normal
			glm::vec3 normal;
			fscanf(file,"%f %f %f\n",&normal.x,&normal.y,&normal.z);
			temp_normals.push_back(normal);
		} else if (strcmp(line_header,"f") == 0) {
			//triângulo
			unsigned int vertex_index[3],uv_index[3],normal_index[3];
			int matches = fscanf(file,"%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertex_index[0],&uv_index[0],&normal_index[0],
				&vertex_index[1],&uv_index[1],&normal_index[1],
				&vertex_index[2],&uv_index[2],&normal_index[2]
			);
			if (matches != 9) {
				printf("modelito ta errado??\n");
				fclose(file);
				return false;
			}
			vertex_indices.push_back(vertex_index[0]);
			vertex_indices.push_back(vertex_index[1]);
			vertex_indices.push_back(vertex_index[2]);
			uv_indices.push_back(uv_index[0]);
			uv_indices.push_back(uv_index[1]);
			uv_indices.push_back(uv_index[2]);
			normal_indices.push_back(normal_index[0]);
			normal_indices.push_back(normal_index[1]);
			normal_indices.push_back(normal_index[2]);
		} else {
			//qualquer outra coisa. termina de ler a linha
			char buffer[1000];
			fgets(buffer,1000,file);
		}
	}
	
	fclose(file);
	
	//agora pra cada vértice
	for (unsigned int i = 0; i < vertex_indices.size(); i++) {
		out_vertices.push_back(temp_vertices[vertex_indices[i]-1]);
		out_uvs.push_back(temp_uvs[uv_indices[i]-1]);
		out_normals.push_back(temp_normals[normal_indices[i]-1]);
	}
	
	//cabo
	return true;
}
