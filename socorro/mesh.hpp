#ifndef MESH_H
#define MESH_H

bool LoadMesh(const char *path,
	std::vector<glm::vec3> &out_vertices,
	std::vector<glm::vec2> &out_uvs,
	std::vector<glm::vec3> &out_normals
);

#endif
