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

mesh light_mesh;

mesh board_mesh;

vector<vec3> colours;



vertex_fragment_shader point_shader;
vertex_geometry_fragment_shader point_depth_shader;
vertex_fragment_shader tex_passthrough;
vertex_geometry_fragment_shader line_shader;



int cam_factor = 24;

size_t shadowMapWidth = 1024;
size_t shadowMapHeight = 1024;

const size_t max_num_lights = 24;
vector<GLuint> depthCubemaps(max_num_lights, 0);
vector<GLuint> depthMapFBOs(max_num_lights, 0);
vector<vec3> lightPositions(max_num_lights, vec3(0, 0, 0));
vector<vec3> lightColours(max_num_lights, vec3(0, 0, 0));
vector<GLuint> lightEnabled(max_num_lights, 0);

GLuint offscreen_fbo = 0;
GLuint offscreen_colour_tex = 0;
GLuint offscreen_depth_tex = 0;



uv_camera main_camera;

GLint win_id = 0;
size_t win_x = 800, win_y = 600;

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


void draw_stuff(GLuint fbo_handle);
void use_buffers(GLuint frame_buffer);


//
//void write_cube_map_to_disk(size_t i, string filename)
//{
//	size_t size = shadowMapWidth * shadowMapHeight;
//	size_t num_bytes = size * 4;
//	vector<unsigned char> output_pixels(num_bytes);
//	vector<float> buffer(size);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
//	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &buffer[0]);
//
//
//
//	for (int i = 0; i < shadowMapHeight; i++)
//		for (int j = 0; j < shadowMapWidth; j++)
//		{
//			size_t imgIdx = 4 * ((i * shadowMapWidth) + j);
//			size_t bufIdx = ((shadowMapHeight - i - 1) * shadowMapWidth) + j;
//
//			// This is just to make a more visible image.  Scale so that
//			// the range (minVal, 1.0) maps to (0.0, 1.0).  This probably should
//			// be tweaked for different light configurations.
//			float minVal = 0.88f;
//			float scale = (buffer[bufIdx] - minVal) / (1.0f - minVal);
//			unsigned char val = (unsigned char)(scale * 255);
//			output_pixels[imgIdx] = val;
//			output_pixels[imgIdx + 1] = val;
//			output_pixels[imgIdx + 2] = val;
//			output_pixels[imgIdx + 3] = 0xff;
//		}
//
//
//
//	// Set up Targa TGA image data.
//	unsigned char  idlength = 0;
//	unsigned char  colourmaptype = 0;
//	unsigned char  datatypecode = 2;
//	unsigned short int colourmaporigin = 0;
//	unsigned short int colourmaplength = 0;
//	unsigned char  colourmapdepth = 0;
//	unsigned short int x_origin = 0;
//	unsigned short int y_origin = 0;
//
//	unsigned short int px = shadowMapWidth;
//	unsigned short int py = shadowMapHeight;
//	unsigned char  bitsperpixel = 32;
//	unsigned char  imagedescriptor = 0;
//	vector<char> idstring;
//
//
//
//	// Write Targa TGA file to disk.
//	ofstream out(filename, ios::binary);
//
//	if (!out.is_open())
//	{
//		cout << "Failed to open TGA file for writing: " << filename << endl;
//		return;
//	}
//
//	out.write(reinterpret_cast<char*>(&idlength), 1);
//	out.write(reinterpret_cast<char*>(&colourmaptype), 1);
//	out.write(reinterpret_cast<char*>(&datatypecode), 1);
//	out.write(reinterpret_cast<char*>(&colourmaporigin), 2);
//	out.write(reinterpret_cast<char*>(&colourmaplength), 2);
//	out.write(reinterpret_cast<char*>(&colourmapdepth), 1);
//	out.write(reinterpret_cast<char*>(&x_origin), 2);
//	out.write(reinterpret_cast<char*>(&y_origin), 2);
//	out.write(reinterpret_cast<char*>(&px), 2);
//	out.write(reinterpret_cast<char*>(&py), 2);
//	out.write(reinterpret_cast<char*>(&bitsperpixel), 1);
//	out.write(reinterpret_cast<char*>(&imagedescriptor), 1);
//
//	out.write(reinterpret_cast<char*>(&output_pixels[0]), num_bytes * sizeof(unsigned char));
//
//	out.close();
//
//
//}



void init_offscreen_fbo(void)
{
	if (offscreen_fbo != 0)
		glDeleteFramebuffers(1, &offscreen_fbo);

	if (offscreen_colour_tex != 0)
		glDeleteTextures(1, &offscreen_colour_tex);

	if (offscreen_depth_tex != 0)
		glDeleteTextures(1, &offscreen_depth_tex);


	glGenTextures(1, &offscreen_depth_tex);
	glBindTexture(GL_TEXTURE_2D, offscreen_depth_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glGenTextures(1, &offscreen_colour_tex);
	glBindTexture(GL_TEXTURE_2D, offscreen_colour_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &offscreen_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen_fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, offscreen_depth_tex, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, offscreen_colour_tex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

}


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


void draw_triangle_lines(GLuint program)
{
	GLuint components_per_vertex = 3;
	GLuint components_per_position = 3;

	GLuint axis_buffer;

	glGenBuffers(1, &axis_buffer);

	vector<GLfloat> flat_data;

	for (size_t t = 0; t < player_game_piece_meshes[0].tri_vec[0].size(); t++)
	{
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[0].x);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[0].y);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[0].z);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[1].x);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[1].y);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[1].z);

		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[1].x);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[1].y);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[1].z);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[2].x);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[2].y);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[2].z);

		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[2].x);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[2].y);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[2].z);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[0].x);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[0].y);
		flat_data.push_back(player_game_piece_meshes[0].tri_vec[0][t].vertex[0].z);
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



void take_screenshot2(size_t num_cams_wide, const char* filename)
{
	screenshot_mode = true;

	const size_t old_width = win_x;
	const size_t old_height = win_y;

	win_x = win_x * num_cams_wide;
	win_y = win_y * num_cams_wide;

	glViewport(0, 0, win_x, win_y);

	GLuint quad_vao = 0;

	glEnable(GL_DEPTH_TEST);


	init_offscreen_fbo();

	main_camera.calculate_camera_matrices(win_x, win_y);

	glBindFramebuffer(GL_FRAMEBUFFER, offscreen_fbo);


	//glUseProgram(shadow_map.get_program());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	draw_stuff(offscreen_fbo);
	use_buffers(offscreen_fbo);


	glGenVertexArrays(1, &quad_vao);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteVertexArrays(1, &quad_vao);



	vector<unsigned char> output_pixels(win_x * win_y * 3);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, win_x, win_y, GL_RGB, GL_UNSIGNED_BYTE, &output_pixels[0]);


	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Set up Targa TGA image data.
	unsigned char  idlength = 0;
	unsigned char  colourmaptype = 0;
	unsigned char  datatypecode = 2;
	unsigned short int colourmaporigin = 0;
	unsigned short int colourmaplength = 0;
	unsigned char  colourmapdepth = 0;
	unsigned short int x_origin = 0;
	unsigned short int y_origin = 0;

	unsigned short int px = win_x;
	unsigned short int py = win_y;
	unsigned char  bitsperpixel = 24;
	unsigned char  imagedescriptor = 0;
	vector<char> idstring;

	for (size_t i = 0; i < win_x; i++)
	{
		for (size_t j = 0; j < win_y; j++)
		{
			size_t index = 3 * (j * win_x + i);

			unsigned char temp_char;
			temp_char = output_pixels[index + 0];
			output_pixels[index + 0] = output_pixels[index + 2];
			output_pixels[index + 2] = temp_char;
		}
	}

	// Write Targa TGA file to disk.
	ofstream out(filename, ios::binary);

	if (!out.is_open())
	{
		cout << "Failed to open TGA file for writing: " << filename << endl;
		return;
	}

	out.write(reinterpret_cast<char*>(&idlength), 1);
	out.write(reinterpret_cast<char*>(&colourmaptype), 1);
	out.write(reinterpret_cast<char*>(&datatypecode), 1);
	out.write(reinterpret_cast<char*>(&colourmaporigin), 2);
	out.write(reinterpret_cast<char*>(&colourmaplength), 2);
	out.write(reinterpret_cast<char*>(&colourmapdepth), 1);
	out.write(reinterpret_cast<char*>(&x_origin), 2);
	out.write(reinterpret_cast<char*>(&y_origin), 2);
	out.write(reinterpret_cast<char*>(&px), 2);
	out.write(reinterpret_cast<char*>(&py), 2);
	out.write(reinterpret_cast<char*>(&bitsperpixel), 1);
	out.write(reinterpret_cast<char*>(&imagedescriptor), 1);

	out.write(reinterpret_cast<char*>(&output_pixels[0]), win_x * win_y * 3 * sizeof(unsigned char));

	out.close();

	win_x = old_width;
	win_y = old_height;
	main_camera.calculate_camera_matrices(win_x, win_y);

	screenshot_mode = false;

	init_offscreen_fbo();

}




void draw_stuff(GLuint fbo_handle)
{

	// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
	// https://community.khronos.org/t/best-solution-for-dealing-with-multiple-light-types/76401

	// todo: set light at negative eye
	vec3 left = cross(normalize(main_camera.eye), normalize(main_camera.up));
	vec3 lightPos = normalize(main_camera.eye) + normalize(main_camera.up) * 2.0f + left * 2.0f;
	lightPos = normalize(lightPos) * 10.0f;
	lightPositions[0] =  lightPos;

	vec3 lightPos2 = normalize(main_camera.eye) + normalize(main_camera.up) * 2.0f + left * 2.0f;
	lightPos2 = normalize(lightPos2) * 10.0f;
	lightPos2.x = -lightPos2.x;
	lightPos2.z = -lightPos2.z;
	lightPositions[1] = lightPos2;

	lightPositions[2] = vec3(0.0, 0.5, 2);
	lightPositions[3] = vec3(0.0, 0.5, 4);

	lightColours[0].x = 1.0;
	lightColours[0].y = 1.0;
	lightColours[0].z = 1.0;

	lightColours[1].x = 1.0;
	lightColours[1].y = 1.0;
	lightColours[1].z = 1.0;

	lightColours[2].x = 0.0;
	lightColours[2].y = 1.0;
	lightColours[2].z = 0.0;

	lightColours[3].x = 0.0;
	lightColours[3].y = 0.0;
	lightColours[3].z = 1.0;

	lightColours[3] = lightColours[0];
	lightPositions[3] = lightPos;


	lightEnabled[0] = 0;
	lightEnabled[1] = 0;
	lightEnabled[2] = 0;
	lightEnabled[3] = 1;











	// ------
	glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	for (size_t i = 0; i < max_num_lights; i++)
	{
		if (lightEnabled[i] == 0)
			continue;

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);

		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		main_camera.calculate_camera_matrices(shadowMapWidth, shadowMapHeight);
		glClear(GL_DEPTH_BUFFER_BIT);
		point_depth_shader.use_program();

		//// 0. create depth cubemap transformation matrices
		//// -----------------------------------------------
		float near_plane = 0.1;// main_camera.near_plane;
		float far_plane = 25.0;// main_camera.far_plane;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)shadowMapWidth / (float)shadowMapHeight, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;

		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		for (unsigned int j = 0; j < 6; j++)
		{
			string loc_string = "shadowMatrices[" + std::to_string(j) + "]";
			glUniformMatrix4fv(glGetUniformLocation(point_depth_shader.get_program(), loc_string.c_str()), 1, GL_FALSE, &shadowTransforms[j][0][0]);
		}

		glUniform1f(glGetUniformLocation(point_depth_shader.get_program(), "far_plane"), far_plane);
		glUniform3f(glGetUniformLocation(point_depth_shader.get_program(), "lightPos"), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		//	glUniform3f(glGetUniformLocation(point_depth_shader.get_program(), "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);

		mat4 model = mat4(1.0f);

		glUniformMatrix4fv(glGetUniformLocation(point_depth_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);

		for (size_t j = 0; j < player_game_piece_meshes.size(); j++)
			player_game_piece_meshes[j].draw(point_depth_shader.get_program(), shadowMapWidth, shadowMapHeight, "chr_knight.png", "chr_knight_specular.png");

		board_mesh.draw(point_shader.get_program(), win_x, win_y, "board.png", "board_specular.png");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}













		//// 2. render scene as normal 
		//// -------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	point_shader.use_program();

	glViewport(0, 0, win_x, win_y);

	main_camera.calculate_camera_matrices(win_x, win_y);

	for (size_t i = 0; i < max_num_lights; i++)
	{
		glActiveTexture(GL_TEXTURE4 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps[i]);
			
		string s = "depthMaps[" + to_string(i) + "]";
		glUniform1i(glGetUniformLocation(point_shader.get_program(), s.c_str()), 4 + i);
	}

	glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);



	glActiveTexture(GL_TEXTURE20);
	glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[0].get_colour_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"),20);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[0].get_specular_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 15);


	glm::mat4 projection = main_camera.projection_mat;// glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = main_camera.view_mat;// .GetViewMatrix();
	mat4 model = mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);

	for (size_t i = 0; i < max_num_lights; i++)
	{
		string s = "lightPositions[" + to_string(i) + "]";
		glUniform3f(glGetUniformLocation(point_shader.get_program(), s.c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
	}

	for (size_t i = 0; i < max_num_lights; i++)
	{
		string s = "lightColours[" + to_string(i) + "]";
		glUniform3f(glGetUniformLocation(point_shader.get_program(), s.c_str()), lightColours[i].x, lightColours[i].y, lightColours[i].z);
	}

	for (size_t i = 0; i < max_num_lights; i++)
	{
		string s = "lightEnabled[" + to_string(i) + "]";
		glUniform1i(glGetUniformLocation(point_shader.get_program(), s.c_str()), lightEnabled[i]);
	}

	glUniform3f(glGetUniformLocation(point_shader.get_program(), "viewPos"), main_camera.eye.x, main_camera.eye.y, main_camera.eye.z);
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "shadows"), 1);
	glUniform1f(glGetUniformLocation(point_shader.get_program(), "far_plane"), 25);


	for (size_t j = 0; j < player_game_piece_meshes.size(); j++)
		player_game_piece_meshes[j].draw(point_shader.get_program(), win_x, win_y, "chr_knight.png", "chr_knight_specular.png");


	glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);



	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, board_mesh.get_colour_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, board_mesh.get_specular_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 3);

	board_mesh.draw(point_shader.get_program(), win_x, win_y, "board.png", "board_specular.png");


	for (size_t j = 0; j < max_num_lights; j++)
	{
		if (lightEnabled[j] == 0)
			continue;

		glm::mat4 projection = main_camera.projection_mat;// glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = main_camera.view_mat;// .GetViewMatrix();
		mat4 model = mat4(1.0f);
		model[3] = vec4(lightPositions[j], 1.0f);

		glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);

		glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);
		glUniform3f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), lightColours[j].x, lightColours[j].y, lightColours[j].z);
 

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, light_mesh.get_colour_tex_handle());
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, board_mesh.get_specular_tex_handle());
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 3);

		light_mesh.draw(point_shader.get_program(), win_x, win_y, "3x3x3.png", "3x3x3.png");
	}




	glEnable(GL_BLEND); //Enable blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
	glUseProgram(line_shader.get_program());

	for (size_t j = 0; j < player_game_piece_meshes.size(); j++)
	{
		glDepthRange(0.025, 1.0);

		model = mat4(1.0f);// player_game_piece_meshes[i].model_mat;
		
		mat4 mvp = main_camera.projection_mat * main_camera.view_mat * model;

		glUniformMatrix4fv(glGetUniformLocation(line_shader.get_program(), "u_modelviewprojection_matrix"), 1, GL_FALSE, &mvp[0][0]);
		glUniform1i(glGetUniformLocation(line_shader.get_program(), "img_width"), win_x);
		glUniform1i(glGetUniformLocation(line_shader.get_program(), "img_height"), win_y);

		if (screenshot_mode)
			glUniform1i(glGetUniformLocation(line_shader.get_program(), "cam_factor"), cam_factor);
		else
			glUniform1i(glGetUniformLocation(line_shader.get_program(), "cam_factor"), 1);

		glUniform1f(glGetUniformLocation(line_shader.get_program(), "line_thickness"), 4.0f);

		draw_triangle_lines(line_shader.get_program());

		glDepthRange(0.0, 1.0);
	}




	glDisable(GL_BLEND);


	glFlush();


	//glBindFramebuffer(GL_FRAMEBUFFER, 0);



}



#endif

