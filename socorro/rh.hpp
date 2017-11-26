#ifndef RH_HPP
#define RH_HPP

#include <GLFW/glfw3.h>
extern GLFWwindow* window;

#define PI 3.14159265359

typedef struct {
	GLuint vertex,uv,normal;
	unsigned int count;
} Mesh;

typedef struct {
	GLuint id;
} Texture;

typedef struct {
	glm::mat4 projection;
	glm::mat4 view;
} Camera;
extern Camera camera;

extern float totalTime;
extern float deltaTime;

Mesh rh_create_mesh(const char *path);
void rh_delete_mesh(Mesh mesh);

Texture rh_create_texture(const char *path);
void rh_delete_texture(Texture texture);

bool rh_start();
bool rh_loop();
void rh_post_loop();
void rh_get_input();
void rh_end();

void rh_draw(Mesh mesh,Texture texture,glm::mat4 model);

quat rh_rot_quat(vec3 start,vec3 dest);
glm::mat4 rh_rot(vec3 start,vec3 dest);

#endif
