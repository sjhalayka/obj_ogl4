#ifndef MAIN_H
#define MAIN_H

#include "frustum.h"
#include "uv_camera.h"
#include "primitives.h"
#include "mesh.h"
#include "vertex_fragment_shader.h"
#include "vertex_geometry_fragment_shader.h"

#include <cstdlib>
#include "GL/glew.h"
#include "GL/glut.h"
#pragma comment(lib, "glew32")

#include <chrono>

#include <ctime>
using std::time;

#include <random>
using std::mt19937;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;
using std::istringstream;


void idle_func(void);
bool init_opengl(const int &width, const int &height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);


mt19937 mt_rand(0);// static_cast<unsigned int>(time(0)));

vector<mesh> player_game_piece_meshes; 

vector<vec3> colours;





vertex_fragment_shader shadow_map;
vertex_fragment_shader tex_passthrough;
vertex_geometry_fragment_shader line_shader;



// The depth buffer texture
GLuint depthTex = 0;

size_t shadowMapWidth = 8192;
size_t shadowMapHeight = 8192;

GLuint shadowFBO = 0, pass1Index = 0, pass2Index = 0;


GLuint offscreen_fbo = 0;
GLuint offscreen_colour_tex = 0;
GLuint offscreen_depth_tex = 0;



uv_camera main_camera;

GLint win_id = 0;
size_t win_x = 800, win_y = 600;
size_t cam_factor = 4;



float u_spacer = 0.01f;
float v_spacer = 0.5f*u_spacer;
float w_spacer = 0.1f;

bool lmb_down = false;
bool mmb_down = false;
bool rmb_down = false;
int mouse_x = 0;
int mouse_y = 0;

vec3 ray;
vec3 collision_location;


bool screenshot_mode = false;



void take_screenshot(size_t num_cams_wide, const char* filename);


vec3 get_pseudorandom_unit_direction(void)
{
	float x = static_cast<float>(mt_rand()) / static_cast<float>(static_cast<long unsigned int>(-1));
	float y = static_cast<float>(mt_rand()) / static_cast<float>(static_cast<long unsigned int>(-1));
	float z = static_cast<float>(mt_rand()) / static_cast<float>(static_cast<long unsigned int>(-1));

	x *= 2.0f;
	x -= 1.0f;
	y *= 2.0f;
	y -= 1.0f;
	z *= 2.0f;
	z -= 1.0f;

	vec3 dir(x, y, z);
	dir = normalize(dir);

	return dir;
}

vec3 screen_coords_to_world_coords(const int x, const int y, const int screen_width, const int screen_height)
{
	const float half_screen_width = screen_width / 2.0f;
	const float half_screen_height = screen_height / 2.0f;

	mat4 inv_mat = inverse(main_camera.projection_mat * main_camera.view_mat);

	vec4 n((x - half_screen_width) / half_screen_width, -1 * (y - half_screen_height) / half_screen_height, -1, 1.0);
	vec4 f((x - half_screen_width) / half_screen_width, -1 * (y - half_screen_height) / half_screen_height, 1, 1.0);

	vec4 near_result = inv_mat * n;
	vec4 far_result = inv_mat * f;

	near_result /= near_result.w;
	far_result /= far_result.w;

	vec3 dir = vec3(far_result - near_result);

	vec3 ret_dir(dir.x, dir.y, dir.z);
	ret_dir = normalize(ret_dir);

	return ret_dir;
}


// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool line_sphere_intersect(const vec3 orig, const vec3 dir, const vec3 center, const float radius, float& t)
{
	float t0, t1; // solutions for t if the ray intersects 

	vec3 L = center - orig;
	float tca = dot(L, dir);

	if (tca < 0)
		return false;
	
	float d2 = dot(L, L) - tca * tca;

	float radius2 = radius * radius;

	if (d2 > radius2) 
		return false;

	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) 
	{
		t0 = t1; // if t0 is negative, let's use t1 instead 
		
		if (t0 < 0)
			return false; // both t0 and t1 are negative 
	}

	t = t0;

	return true;
}

//
//void draw_triangle_outlines(GLuint program)
//{
//	GLuint components_per_vertex = 3;
//	GLuint components_per_position = 3;
//
//	GLuint axis_buffer;
//
//	glGenBuffers(1, &axis_buffer);
//
//	vector<GLfloat> flat_data;
//
//	for (size_t t = 0; t < player_game_piece_meshes[0].triangles.size(); t++)
//	{
//		flat_data.push_back(triangles[t].vertex[0].x);
//		flat_data.push_back(triangles[t].vertex[0].y);
//		flat_data.push_back(triangles[t].vertex[0].z);
//		flat_data.push_back(triangles[t].vertex[1].x);
//		flat_data.push_back(triangles[t].vertex[1].y);
//		flat_data.push_back(triangles[t].vertex[1].z);
//
//		flat_data.push_back(triangles[t].vertex[1].x);
//		flat_data.push_back(triangles[t].vertex[1].y);
//		flat_data.push_back(triangles[t].vertex[1].z);
//		flat_data.push_back(triangles[t].vertex[2].x);
//		flat_data.push_back(triangles[t].vertex[2].y);
//		flat_data.push_back(triangles[t].vertex[2].z);
//
//		flat_data.push_back(triangles[t].vertex[2].x);
//		flat_data.push_back(triangles[t].vertex[2].y);
//		flat_data.push_back(triangles[t].vertex[2].z);
//		flat_data.push_back(triangles[t].vertex[0].x);
//		flat_data.push_back(triangles[t].vertex[0].y);
//		flat_data.push_back(triangles[t].vertex[0].z);
//	}
//
//
//
//	GLuint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;
//
//	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
//	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_DYNAMIC_DRAW);
//
//	glEnableVertexAttribArray(glGetAttribLocation(program, "position"));
//	glVertexAttribPointer(glGetAttribLocation(program, "position"),
//		components_per_position,
//		GL_FLOAT,
//		GL_FALSE,
//		components_per_vertex * sizeof(GLfloat),
//		NULL);
//
//	glDrawArrays(GL_LINES, 0, num_vertices);
//
//	glDeleteBuffers(1, &axis_buffer);
//}


void draw_axis(GLuint program)
{
	GLuint components_per_vertex = 3;
	GLuint components_per_position = 3;

	GLuint axis_buffer;

	glGenBuffers(1, &axis_buffer);

	vector<GLfloat> flat_data;

	for (size_t t = 0; t < player_game_piece_meshes[0].triangles.size(); t++)
	{
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[0].x);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[0].y);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[0].z);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[1].x);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[1].y);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[1].z);

		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[1].x);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[1].y);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[1].z);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[2].x);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[2].y);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[2].z);

		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[2].x);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[2].y);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[2].z);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[0].x);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[0].y);
		flat_data.push_back(player_game_piece_meshes[0].triangles[t].vertex[0].z);
	}

	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(1);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(0);

	//flat_data.push_back(0);
	//flat_data.push_back(1);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//	
	//flat_data.push_back(1);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(0);
	//flat_data.push_back(0);

	GLuint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(program, "position"));
	glVertexAttribPointer(glGetAttribLocation(program, "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_LINES, 0, num_vertices);

	glDeleteBuffers(1, &axis_buffer);
}





#endif

