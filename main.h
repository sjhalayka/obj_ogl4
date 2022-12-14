#ifndef MAIN_H
#define MAIN_H

#include "frustum.h"
#include "uv_camera.h"
#include "primitives.h"
#include "mesh.h"
#include "font_draw.h"
#include "logging_system.h"
#include "vertex_fragment_shader.h"
#include "vertex_geometry_fragment_shader.h"
#include "compute_shader.h"
#include "astar.h"
#include "arc_animation.h"

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

#include <algorithm>
using std::min;
using std::max;

#include <deque>
using std::deque;


void idle_func(void);
bool init_opengl(const int &width, const int &height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);




vector<vec3> board_highlight_colours;
vector<bool> board_highlight_enabled;


GLuint last_frame_glowmap_tex = 0;



// These need to be adjusted when players die
// that is, delete the item, then reduce the index of
// the remaining items by 1
vector<vec3> player_highlight_colours;
vector<bool> player_highlight_enabled;
 

size_t current_player = 0;
bool advance_current_player = false;


int grid[ROW][COL] =
{
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 }
};



vector<vec3> path_line_strip;




mt19937 mt_rand(0);// static_cast<unsigned int>(time(0)));

vector<mesh> player_game_piece_meshes; 
//vector<mesh> enemy_game_piece_meshes;
mesh light_mesh;
binned_mesh board_mesh;

deque<arc_animation> anim_deque;

vector<pair<int, int>> output_path;


enum possible_collision_locations { player_game_piece, game_board, background };

vec3 clicked_collision_location;
size_t clicked_collision_location_index = 0;
possible_collision_locations clicked_col_loc = background;
size_t clicked_cell_x, clicked_cell_y;


vec3 hover_collision_location;
size_t hover_collision_location_index = 0;
possible_collision_locations hover_col_loc = background;
size_t hover_cell_x, hover_cell_y;




vertex_fragment_shader point_shader;
vertex_geometry_fragment_shader point_depth_shader;
vertex_fragment_shader tex_passthrough;
vertex_geometry_fragment_shader line_shader;
vertex_geometry_fragment_shader circle_shader;

vertex_fragment_shader tex_reflectance;
vertex_fragment_shader ortho_text;
compute_shader glowmap_copier;


//vertex_fragment_shader proj_tex_shader;

//vertex_geometry_fragment_shader silhouette_shader;

float y_offset = 0;

int cam_factor = 4;

size_t shadowMapWidth = 2048;
size_t shadowMapHeight = 2048;

const size_t max_num_lights = 4;
vector<GLuint> depthCubemaps(max_num_lights, 0);
vector<GLuint> depthMapFBOs(max_num_lights, 0);
vector<vec3> lightPositions(max_num_lights, vec3(0, 0, 0));
vector<vec3> lightColours(max_num_lights, vec3(0, 0, 0));
vector<GLuint> lightEnabled(max_num_lights, 0);
vector<GLuint> lightShadowCaster(max_num_lights, 0);



GLuint offscreen_fbo = 0;
GLuint offscreen_colour_tex = 0;
GLuint offscreen_depth_tex = 0;



uv_camera main_camera;

GLint win_id = 0;
size_t win_x = 1920, win_y = 1080;

float u_spacer = 0.01f;
float v_spacer = 0.5f*u_spacer;
float w_spacer = 0.1f;

bool lmb_down = false;
bool mmb_down = false;
bool rmb_down = false;
int mouse_x = 0;
int mouse_y = 0;



bool screenshot_mode = false;


void draw_stuff(GLuint fbo_handle, bool upside_down, bool reflectance_only, bool solid_white, bool glowmap_only);
void use_buffers(GLuint frame_buffer, GLuint depth_tex_handle, GLuint colour_tex_handle);
void draw_scene(GLuint fbo_handle);

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

	main_camera.calculate_camera_matrices(win_x, win_y, true);

	init_offscreen_fbo();

	if (glIsTexture(last_frame_glowmap_tex))
		glDeleteTextures(1, &last_frame_glowmap_tex);

	glGenTextures(1, &last_frame_glowmap_tex);
	glBindTexture(GL_TEXTURE_2D, last_frame_glowmap_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glBindFramebuffer(GL_FRAMEBUFFER, offscreen_fbo);



	// Build up the glow buffer
	for(size_t i = 0; i < 4; i++)
	draw_scene(offscreen_fbo);


	glGenVertexArrays(1, &quad_vao);

	//	glDisable(GL_DEPTH_TEST);
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
	main_camera.calculate_camera_matrices(win_x, win_y, true);

	screenshot_mode = false;

	init_offscreen_fbo();

	if (glIsTexture(last_frame_glowmap_tex))
		glDeleteTextures(1, &last_frame_glowmap_tex);

	glGenTextures(1, &last_frame_glowmap_tex);
	glBindTexture(GL_TEXTURE_2D, last_frame_glowmap_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}



void draw_stuff(GLuint fbo_handle, bool upside_down, bool reflectance_only, bool solid_white, bool glowmap_only)
{

	// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
	// https://community.khronos.org/t/best-solution-for-dealing-with-multiple-light-types/76401

	//vec3 left = cross(normalize(main_camera.eye), normalize(main_camera.up));
	//vec3 lightPos = normalize(main_camera.eye) + normalize(main_camera.up) * 2.0f + left * 2.0f;
	////lightPos = normalize(lightPos) * 10.0f;
	//lightPositions[0] = normalize(lightPos) * 10.0f;

	//vec3 lightPos2 = normalize(main_camera.eye) + normalize(main_camera.up) * 2.0f + left * 2.0f;
	////lightPos2 = normalize(lightPos2) * 10.0f;
	//lightPos2.x = -lightPos2.x;
	//lightPos2.z = -lightPos2.z;
	//lightPositions[1] = lightPos2 * 10.0f;


	lightPositions[0] = vec3(-6, 6, 6);
	lightPositions[1] = vec3(6, 6, -6);
	lightPositions[2] = vec3(0, 4, 0);
	lightPositions[3] = vec3(-6, 6, -6);


	lightColours[0].r = 1;
	lightColours[0].g = 1;
	lightColours[0].b = 1;

	lightColours[1].r = 1;
	lightColours[1].g = 1;
	lightColours[1].b = 1;

	lightColours[2].r = 0.2;
	lightColours[2].g = 0.2;
	lightColours[2].b = 1;

	lightColours[3].r = 1;
	lightColours[3].g = 0.2;
	lightColours[3].b = 0.2;



	lightEnabled[0] = 1;
	lightEnabled[1] = 1;
	lightEnabled[2] = 1;
	lightEnabled[3] = 1;

	lightShadowCaster[0] = 0;
	lightShadowCaster[1] = 0;
	lightShadowCaster[2] = 1;
	lightShadowCaster[3] = 0;


	mat4 old_view_mat = main_camera.view_mat;


	if (upside_down)
	{
		glFrontFace(GL_CW);
		main_camera.view_mat = scale(old_view_mat, vec3(1, -1, 1));
	}
	else
	{
		glFrontFace(GL_CCW);
	}

	//if (upside_down)
	//	glCullFace(GL_FRONT);
	//else
	//	glCullFace(GL_BACK);


	//if (glowmap_only)
	//	glClearColor(1, 1, 1, 0);
	//else
	//	glClearColor(0, 0, 0, 0);




	if (false == reflectance_only && false == solid_white && false == glowmap_only)
	{
		for (size_t i = 0; i < max_num_lights; i++)
		{
			if (lightEnabled[i] == 0 || lightShadowCaster[i] == 0)
				continue;

			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);

			glViewport(0, 0, shadowMapWidth, shadowMapHeight);
			main_camera.calculate_camera_matrices(shadowMapWidth, shadowMapHeight, false);

			glClear(GL_DEPTH_BUFFER_BIT);
			point_depth_shader.use_program();

			//// 0. create depth cubemap transformation matrices
			//// -----------------------------------------------
			float near_plane = main_camera.near_plane;
			float far_plane = main_camera.far_plane;
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


			for (size_t j = 0; j < player_game_piece_meshes.size(); j++)
			{
				mat4 model = player_game_piece_meshes[j].model_mat;

				glUniformMatrix4fv(glGetUniformLocation(point_depth_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);

				player_game_piece_meshes[j].draw(point_depth_shader.get_program());// , shadowMapWidth, shadowMapHeight, "beholder.png", "beholder_specular.png");
			}


			mat4 model = board_mesh.model_mat;

			glUniformMatrix4fv(glGetUniformLocation(point_depth_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);

			for (size_t x = 0; x < board_mesh.num_cells_wide; x++)
			{
				for (size_t y = 0; y < board_mesh.num_cells_wide; y++)
				{
					board_mesh.draw(point_depth_shader.get_program(), x, y);// , win_x, win_y, "board_glow.png", "board.png", "board_specular.png");
				}
			}


			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}












	//// 2. render scene as normal 
	//// -------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	


	point_shader.use_program();

	if (reflectance_only)
	{
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "specular_only"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "specular_only"), 0);
	
		if (false == solid_white)
		{
			if (glowmap_only)
				glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowmap_only"), 1);
			else
				glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowmap_only"), 0);
		}
	}




	glViewport(0, 0, win_x, win_y);

	main_camera.calculate_camera_matrices(win_x, win_y, false);


	for (size_t i = 0; i < max_num_lights; i++)
	{
		glActiveTexture(GL_TEXTURE4 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps[i]);

		string s = "depthMaps[" + to_string(i) + "]";
		glUniform1i(glGetUniformLocation(point_shader.get_program(), s.c_str()), 4 + i);
	}

	glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);




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

	for (size_t i = 0; i < max_num_lights; i++)
	{
		string s = "lightShadowCaster[" + to_string(i) + "]";
		glUniform1i(glGetUniformLocation(point_shader.get_program(), s.c_str()), lightShadowCaster[i]);
	}


	glUniform1i(glGetUniformLocation(point_shader.get_program(), "shadows"), 1);
	glUniform1f(glGetUniformLocation(point_shader.get_program(), "far_plane"), main_camera.far_plane);



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	
	if (false == upside_down && false == reflectance_only && false == solid_white/* && true == glowmap_only*/)
	{
		glUseProgram(line_shader.get_program());

		glUniform3f(glGetUniformLocation(line_shader.get_program(), "camera_pos"), main_camera.eye.x, main_camera.eye.y, main_camera.eye.z);
		glUniform1i(glGetUniformLocation(line_shader.get_program(), "img_width"), win_x);
		glUniform1i(glGetUniformLocation(line_shader.get_program(), "img_height"), win_y);

		if (screenshot_mode)
			glUniform1i(glGetUniformLocation(line_shader.get_program(), "cam_factor"), cam_factor);
		else
			glUniform1i(glGetUniformLocation(line_shader.get_program(), "cam_factor"), 1);

		glUniform1f(glGetUniformLocation(line_shader.get_program(), "line_thickness"), 4.0f);


		glUseProgram(circle_shader.get_program());

		glUniform3f(glGetUniformLocation(circle_shader.get_program(), "camera_pos"), main_camera.eye.x, main_camera.eye.y, main_camera.eye.z);
		glUniform1i(glGetUniformLocation(circle_shader.get_program(), "img_width"), win_x);
		glUniform1i(glGetUniformLocation(circle_shader.get_program(), "img_height"), win_y);

		if (screenshot_mode)
			glUniform1i(glGetUniformLocation(circle_shader.get_program(), "cam_factor"), cam_factor);
		else
			glUniform1i(glGetUniformLocation(circle_shader.get_program(), "cam_factor"), 1);

		glUniform1f(glGetUniformLocation(circle_shader.get_program(), "line_thickness"), 4.0f);


		if (false == glowmap_only)
		{
			glDepthRange(0.1, 1.0);

			line_shader.use_program();

			mat4 model = board_mesh.model_mat;
			mat4 mvp = main_camera.projection_mat * main_camera.view_mat * model;
			glUniformMatrix4fv(glGetUniformLocation(line_shader.get_program(), "u_modelviewprojection_matrix"), 1, GL_FALSE, &mvp[0][0]);
			glUniform4f(glGetUniformLocation(line_shader.get_program(), "u_color"), 0.75, 0.75, 0.75, 1.0);
			board_mesh.draw_lines(line_shader.get_program());


			circle_shader.use_program();

			glUniformMatrix4fv(glGetUniformLocation(circle_shader.get_program(), "u_modelviewprojection_matrix"), 1, GL_FALSE, &mvp[0][0]);
			glUniform4f(glGetUniformLocation(circle_shader.get_program(), "u_color"), 0.75, 0.75, 0.75, 1.0);
			board_mesh.draw_circles(circle_shader.get_program());
		}




		for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
		{

			glDepthRange(0.01, 1.0);

			line_shader.use_program();

			mat4 model = player_game_piece_meshes[i].model_mat;
			mat4 mvp = main_camera.projection_mat * main_camera.view_mat * model;
			glUniformMatrix4fv(glGetUniformLocation(line_shader.get_program(), "u_modelviewprojection_matrix"), 1, GL_FALSE, &mvp[0][0]);

			if(i == current_player)
				glUniform4f(glGetUniformLocation(line_shader.get_program(), "u_color"), 0.5, 0.25, 0.0, 1.0);
			else
				glUniform4f(glGetUniformLocation(line_shader.get_program(), "u_color"), 0.0, 0.0, 0.0, 1.0);


			player_game_piece_meshes[i].draw_lines(line_shader.get_program());



			circle_shader.use_program();

			glUniform3f(glGetUniformLocation(circle_shader.get_program(), "camera_pos"), main_camera.eye.x, main_camera.eye.y, main_camera.eye.z);
			glUniform1i(glGetUniformLocation(circle_shader.get_program(), "img_width"), win_x);
			glUniform1i(glGetUniformLocation(circle_shader.get_program(), "img_height"), win_y);

			if (screenshot_mode)
				glUniform1i(glGetUniformLocation(circle_shader.get_program(), "cam_factor"), cam_factor);
			else
				glUniform1i(glGetUniformLocation(circle_shader.get_program(), "cam_factor"), 1);

			glUniform1f(glGetUniformLocation(circle_shader.get_program(), "line_thickness"), 4.0f);


			model = player_game_piece_meshes[i].model_mat;
			mvp = main_camera.projection_mat * main_camera.view_mat * model;
			glUniformMatrix4fv(glGetUniformLocation(circle_shader.get_program(), "u_modelviewprojection_matrix"), 1, GL_FALSE, &mvp[0][0]);

			if (i == current_player)
				glUniform4f(glGetUniformLocation(circle_shader.get_program(), "u_color"), 0.5, 0.25, 0.0, 1.0);
			else
				glUniform4f(glGetUniformLocation(circle_shader.get_program(), "u_color"), 0.0, 0.0, 0.0, 1.0);



			player_game_piece_meshes[i].draw_circles(circle_shader.get_program());


			//if(clicked_col_loc == player_game_piece && i == clicked_collision_location_index)
			//player_game_piece_meshes[i].draw_AABB(line_shader.get_program());
		}
	}

	glDepthRange(0.0, 1.0);
	






	point_shader.use_program();







	//GLuint cat_tex = 0;

	//std::vector<unsigned char> buffer, colour_data;
	//loadFile(buffer, "cat.png");
	//unsigned long w, h;
	//decodePNG(colour_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

	//glGenTextures(1, &cat_tex);
	//glBindTexture(GL_TEXTURE_2D, cat_tex);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour_data[0]);


	//vec3 projPos = vec3(0.0f, 5.0f, 0.0f);
	//vec3 projAt = vec3(0.0f, -5.0f, 0.0f);
	//vec3 projUp = vec3(1.0f, 0.0f, 0.0f);
	//mat4 projView = glm::lookAt(projPos, projAt, projUp);
	//mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.01f, 1000.0f);
	//mat4 bias = glm::translate(mat4(1.0f), vec3(0.5f));
	//bias = glm::scale(bias, vec3(0.5f));
	//mat4 projMat = bias * projProj * projView;

	//glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "ProjectorMatrix"), 1, GL_FALSE, &projMat[0][0]);

	glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"),0 );
	

	if (false == reflectance_only)
	{
		if (false == solid_white)
		{
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);
		}
		else
		{
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);
			glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1, 1);
		}

		for (size_t j = 0; j < player_game_piece_meshes.size(); j++)
		{
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, cat_tex);
			//glUniform1i(glGetUniformLocation(point_shader.get_program(), "projectorTexture"), 0);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[j].get_glow_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[j].get_colour_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[j].get_specular_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 2);


			mat4 model = player_game_piece_meshes[j].model_mat;

			if (upside_down)
			{
				//glCullFace(GL_FRONT);

				model = translate(model, vec3(0, -player_game_piece_meshes[j].get_y_extent(), 0));
				model = scale(model, vec3(1, -1, 1));
			}

			glm::mat4 projection = main_camera.projection_mat;
			glm::mat4 view = main_camera.view_mat;

			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);


			player_game_piece_meshes[j].draw(point_shader.get_program());// , win_x, win_y, "beholder.png", "beholder_specular.png");
		




		
			if ( player_highlight_enabled[j] && false == upside_down && false == glowmap_only)
			{
				glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);

				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), player_highlight_colours[j].r, player_highlight_colours[j].g, player_highlight_colours[j].b, 0.5);
				player_game_piece_meshes[j].draw(point_shader.get_program());// , win_x, win_y, "beholder.png", "beholder_specular.png");

				glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);
			}
		}
	}
	else
	{
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"), 0);

		glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);
		glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 0, 0, 0, 1);

		glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"), 0);// 1);

		for (size_t j = 0; j < player_game_piece_meshes.size(); j++)
		{
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, cat_tex);
			//glUniform1i(glGetUniformLocation(point_shader.get_program(), "projectorTexture"), 0);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[j].get_glow_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[j].get_colour_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, player_game_piece_meshes[j].get_specular_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 2);



			mat4 model = player_game_piece_meshes[j].model_mat;

			if (upside_down)
			{
				model = translate(model, vec3(0, -player_game_piece_meshes[j].get_y_extent(), 0));
				model = scale(model, vec3(1, -1, 1));
			}

			glm::mat4 projection = main_camera.projection_mat;
			glm::mat4 view = main_camera.view_mat;

			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);


			player_game_piece_meshes[j].draw(point_shader.get_program());// , win_x, win_y, "beholder.png", "beholder_specular.png");



			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);




			//if (player_highlight_enabled[j])
			//{
			//	//if (reflectance_only)
			//		//glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 0, 0, 0, 1);
			//	//else
			//	//{
			//		glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1.0, 0.5, 0, 0.5);
			//	//}


			//	player_game_piece_meshes[j].draw(point_shader.get_program(), win_x, win_y, "beholder.png", "beholder_specular.png");

			//}

			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);


		}

		glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"), 0);
	}

	


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, cat_tex);
	//glUniform1i(glGetUniformLocation(point_shader.get_program(), "projectorTexture"), 0);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, board_mesh.get_glow_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowTexture"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, board_mesh.get_colour_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, board_mesh.get_specular_tex_handle());
	glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 2);





	if (false == upside_down)
	{
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"), 1);

		for (size_t x = 0; x < board_mesh.num_cells_wide; x++)
		{
			for (size_t y = 0; y < board_mesh.num_cells_wide; y++)
			{
				mat4 model = board_mesh.model_mat;

				glm::mat4 projection = main_camera.projection_mat;
				glm::mat4 view = main_camera.view_mat;

				glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);

				board_mesh.draw(point_shader.get_program(), x, y);// , win_x, win_y, "board_glow.png", "board.png", "board_specular.png");

				glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);

				size_t index = y * board_mesh.num_cells_wide + x;


				if (board_highlight_enabled[index] && false == glowmap_only)
				{
					if (reflectance_only)
						glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 0, 0, 0, 1);
					else
					{
						glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), board_highlight_colours[index].r, board_highlight_colours[index].g, board_highlight_colours[index].b, 0.5);
					}

					board_mesh.draw(point_shader.get_program(), x, y);// , win_x, win_y, "board_glow.png", "board.png", "board_specular.png");
				}

				glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);








			}
		}
	}
	else
	{
		if (solid_white == true)
		{
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"), 0);
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);
			glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1, 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "do_proj_tex"), 1);
		}

		// https://prideout.net/clip-planes
		
//		glEnable(GL_CLIP_DISTANCE0);
//		glUniform1i(glGetUniformLocation(point_shader.get_program(), "clip_plane_enabled"), 1);

		map<float, size_t> y_extents;

		for (size_t x = 0; x < board_mesh.num_cells_wide; x++)
		{
			for (size_t y = 0; y < board_mesh.num_cells_wide; y++)
			{
				vector<neighbour_data> n;
				//board_mesh.get_all_neighbour_indices(x, y, n);

				neighbour_data nd;
				nd.x = x;
				nd.y = y;

				n.push_back(nd);

				for (size_t i = 0; i < n.size(); i++)
				{
					mat4 model = board_mesh.model_mat;

					float current_height = distance(board_mesh.get_y_min(), board_mesh.get_y_max(n[i].x, n[i].y));
					float normalized_height = (current_height / board_mesh.get_y_extent() - 0.5f)*2.0f;

					model = translate(model, vec3(0, -board_mesh.get_y_extent(n[i].x, n[i].y) * 2, 0));				
					model = scale(model, vec3(1, -1, 1));
					model = translate(model, vec3(0, -normalized_height*board_mesh.get_y_extent(), 0));
					float y_plane_min = board_mesh.get_y_plane_min(n[i].x, n[i].y);

					glUniform1i(glGetUniformLocation(point_shader.get_program(), "clip_plane_enabled"), 0);




					glm::mat4 projection = main_camera.projection_mat;
					glm::mat4 view = main_camera.view_mat;

					glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
					glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
					glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);

					board_mesh.draw(point_shader.get_program(), n[i].x, n[i].y);// , win_x, win_y, "board_glow.png", "board.png", "board_specular.png");


					//if (solid_white)
					//{
					//	glUniform3f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1);
					//}

					//else
					//{
					//	glUniform3f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1);
					//}

					//	glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);

					//if (hover_col_loc == game_board && hover_cell_x == n[i].x && hover_cell_y == n[i].y)
					//	board_mesh.draw(point_shader.get_program(), n[i].x, n[i].y, win_x, win_y, "board.png", "board_specular.png");

					//glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);



					//vec4 clip_plane(0, 1, 0, y_plane_min);
					//glUniform4f(glGetUniformLocation(point_shader.get_program(), "clip_plane"), clip_plane.x, clip_plane.y, clip_plane.z, clip_plane.w);
					//board_mesh.draw(point_shader.get_program(), n[i].x, n[i].y, win_x, win_y, "board.png", "board_specular.png");
				
					//clip_plane = vec4(0, -1, 0, -y_plane_min);
					//glUniform4f(glGetUniformLocation(point_shader.get_program(), "clip_plane"), clip_plane.x, clip_plane.y, clip_plane.z, clip_plane.w);
					//board_mesh.draw(point_shader.get_program(), n[i].x, n[i].y, win_x, win_y, "board.png", "board_specular.png");
				}
			}
		}

	//	glDisable(GL_CLIP_DISTANCE0);
		//glUniform1i(glGetUniformLocation(point_shader.get_program(), "clip_plane_enabled"), 0);

		glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 0);
	}


	glDisable(GL_BLEND);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_POLYGON_OFFSET_FILL);




















	
	// draw lights
	if (0)//false == glowmap_only)//false == reflectance_only)
	{
		for (size_t j = 0; j < max_num_lights; j++)
		{
			if (lightEnabled[j] == 0)
				continue;

			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);

			if(solid_white)
				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1, 1);
			
			if (reflectance_only)
				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 0, 0, 0, 1);

			if(solid_white == false && reflectance_only == false)
				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), lightColours[j].x, lightColours[j].y, lightColours[j].z, 1);

			glm::mat4 projection = main_camera.projection_mat;// glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = main_camera.view_mat;// .GetViewMatrix();
			mat4 model = mat4(1.0f);

			if (0)//upside_down)
			{
				//glCullFace(GL_FRONT);
				model[3] = vec4(lightPositions[j], 1.0f);
				model[3][1] = -model[3][1];
				model = translate(model, vec3(0, -board_mesh.get_y_extent() / 2, 0));
			}
			else
			{
				//glCullFace(GL_BACK);
				model[3] = vec4(lightPositions[j], 1.0f);
			}

			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, light_mesh.get_glow_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, light_mesh.get_colour_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, light_mesh.get_specular_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 2);


			light_mesh.draw(point_shader.get_program());// , win_x, win_y, "3x3x3.png", "3x3x3.png");
		}
	}
	

	
	if (/*false == glowmap_only &&*/ clicked_collision_location != vec3(0, 0, 0))
	{
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);

			if (solid_white)
				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1, 1);

			if (reflectance_only)
				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 0, 0, 0, 1);

			if (solid_white == false && reflectance_only == false)
				glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1, 1);

			glm::mat4 projection = main_camera.projection_mat;// glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = main_camera.view_mat;// .GetViewMatrix();
			mat4 model = mat4(1.0f);


				model[3] = vec4(clicked_collision_location, 1.0f);

			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, light_mesh.get_glow_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, light_mesh.get_colour_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, light_mesh.get_specular_tex_handle());
			glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 2);


			light_mesh.draw(point_shader.get_program());// , win_x, win_y, "3x3x3.png", "3x3x3.png");
	}
	


	if (/*false == glowmap_only &&*/ hover_collision_location != vec3(0, 0, 0))
	{
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "flat_draw"), 1);

		if (solid_white)
			glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1, 1, 1, 1);

		if (reflectance_only)
			glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 0, 0, 0, 1);

		if (solid_white == false && reflectance_only == false)
		{
			glUniform4f(glGetUniformLocation(point_shader.get_program(), "flat_colour"), 1.0, 0.5, 0, 1);
		}

		glm::mat4 projection = main_camera.projection_mat;// glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = main_camera.view_mat;// .GetViewMatrix();
		mat4 model = mat4(1.0f);


		model[3] = vec4(hover_collision_location, 1.0f);

		glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "model"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(point_shader.get_program(), "view"), 1, GL_FALSE, &view[0][0]);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, light_mesh.get_glow_tex_handle());
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "glowTexture"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, light_mesh.get_colour_tex_handle());
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "diffuseTexture"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, light_mesh.get_specular_tex_handle());
		glUniform1i(glGetUniformLocation(point_shader.get_program(), "specularTexture"), 2);


		light_mesh.draw(point_shader.get_program());// , win_x, win_y, "3x3x3.png", "3x3x3.png");
	}









	//glDeleteTextures(1, &cat_tex);


	if (upside_down)
		main_camera.view_mat = old_view_mat;






	

	glFlush();
}





void situate_player_mesh(size_t x_cell, size_t y_cell, size_t index, bool update_location = true)
{
	if (update_location)
	{
		vec3 centre = board_mesh.get_centre(x_cell, y_cell);
		centre.y = board_mesh.get_y_plane_min(x_cell, y_cell);
		centre.y += player_game_piece_meshes[index].get_y_extent() * 0.5;

		player_game_piece_meshes[index].model_mat = mat4(1.0f);
		player_game_piece_meshes[index].model_mat = translate(player_game_piece_meshes[index].model_mat, centre);
	}

	player_game_piece_meshes[index].cell_location.first = x_cell;
	player_game_piece_meshes[index].cell_location.second = y_cell;
}

void move_player_mesh(size_t x_cell, size_t y_cell, size_t index)
{
	vec3 centre = board_mesh.get_centre(x_cell, y_cell);
	centre.y = board_mesh.get_y_plane_min(x_cell, y_cell);
	centre.y += player_game_piece_meshes[index].get_y_extent() * 0.5;

	player_game_piece_meshes[index].model_mat = translate(player_game_piece_meshes[index].model_mat, centre);

	player_game_piece_meshes[index].cell_location.first = x_cell;
	player_game_piece_meshes[index].cell_location.second = y_cell;
}




void update_board_highlighting(void)
{
	pair<size_t, size_t> hover_start = player_game_piece_meshes[current_player].cell_location;
	size_t hover_start_x = hover_start.first;
	size_t hover_start_y = hover_start.second;

	size_t hover_end_x = hover_cell_x;
	size_t hover_end_y = hover_cell_y;


	Pair src = make_pair(hover_start_x, hover_start_y);

	// Destination is the left-most top-most corner
	Pair dest = make_pair(hover_end_x, hover_end_y);

	output_path.clear();

	path_line_strip.clear();

	int grid_temp[ROW][COL] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	for (size_t i = 0; i < ROW; i++)
	{
		for (size_t j = 0; j < COL; j++)
		{
			for (size_t k = 0; k < player_game_piece_meshes.size(); k++)
			{
				if (k == current_player)
				{
					grid_temp[player_game_piece_meshes[k].cell_location.first][player_game_piece_meshes[k].cell_location.second] = 1;
				}
				else
				{
					grid_temp[player_game_piece_meshes[k].cell_location.first][player_game_piece_meshes[k].cell_location.second] = 0;
				}
			}

			grid[i][j] = grid_temp[i][j];
		}
	}

	for (size_t i = 0; i < (ROW); i++)
	{
		for (size_t j = 0; j < (COL); j++)
		{
			size_t index = j * ROW + i;

			vec4 m = player_game_piece_meshes[current_player].model_mat[3];
			vec3 player_centre(m.x, m.y, m.z);

			vec3 start_centre = board_mesh.get_centre(i, j);
			start_centre.y = board_mesh.get_y_plane_min(i, j);

			// to do: change the parameter used here to the player's distance paramter
			// like, far reach for an archer or mage, close reach for
			// tanks
			if (distance(player_centre, start_centre) < 4.0 && aStarSearch(grid, pair<size_t, size_t>(player_game_piece_meshes[current_player].cell_location.first, player_game_piece_meshes[current_player].cell_location.second), pair<size_t, size_t>(i, j), output_path))
			{
				if (hover_cell_x == i && hover_cell_y == j)
				{
					// Disallow movement to and from the same cell
					if (false)//i == player_game_piece_meshes[current_player].cell_location.first && j == player_game_piece_meshes[current_player].cell_location.second)
					{
						board_highlight_enabled[index] = true;
						board_highlight_colours[index] = vec3(1, 0, 0);
					}
					else
					{
						board_highlight_enabled[index] = true;
						board_highlight_colours[index] = vec3(0, 1, 0);
					}
				}
				else
				{
					board_highlight_enabled[index] = true;
					board_highlight_colours[index] = vec3(0, 0, 1);
				}
			}
			else
			{
				if (hover_cell_x == i && hover_cell_y == j)
				{
					board_highlight_enabled[index] = true;
					board_highlight_colours[index] = vec3(1, 0, 0);
				}
				else
				{
					board_highlight_enabled[index] = false;
				}
			}
		}
	}



	if (false == aStarSearch(grid, src, dest, output_path))
	{
		cout << "did not find path" << endl;
	}


}





void get_hover_collision_location(size_t x, size_t y)
{
	// Only one can be picked at a time
	player_highlight_enabled.clear();
	player_highlight_enabled.resize(player_game_piece_meshes.size(), false);

	// Get intersection point closest to camera


	vec3 ray = screen_coords_to_world_coords(x, y, win_x, win_y);

	float t = 0;

	hover_col_loc = background;
	bool first_assignment = true;

	vec4 start = vec4(main_camera.eye, 1.0);
	vec4 direction = vec4(ray, 0.0);
	direction = normalize(direction);

	// bounding box is axis aligned for the board mesh
	// and assuming that board_mesh's model matrix equals mat4(1.0f)
	if (true == board_mesh.intersect_AABB(start, direction))
	{
		vec3 closest_intersection_point;

		for (size_t cell_x = 0; cell_x < board_mesh.num_cells_wide; cell_x++)
		{
			for (size_t cell_y = 0; cell_y < board_mesh.num_cells_wide; cell_y++)
			{
				size_t index = cell_y * board_mesh.num_cells_wide + cell_x;

				if (true == board_mesh.intersect_triangles(start, direction, closest_intersection_point, cell_x, cell_y))
				{
					closest_intersection_point = board_mesh.model_mat * vec4(closest_intersection_point, 1);

					hover_collision_location = closest_intersection_point;
					hover_col_loc = game_board;
					hover_cell_x = cell_x;
					hover_cell_y = cell_y;
					first_assignment = false;

					board_highlight_colours[index] = vec3(1, 0.5, 0);
					board_highlight_enabled[index] = true;
				}
				else
				{
					//board_highlight_colours[index] = vec3(1, 0.5, 0);
					board_highlight_enabled[index] = false;
				}
			}
		}
	}











	for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
	{
		mat4 inv = inverse(player_game_piece_meshes[i].model_mat);
		vec4 start = inv * vec4(main_camera.eye, 1.0);
		vec4 direction = inv * vec4(ray, 0.0);
		direction = normalize(direction);

		if (true == player_game_piece_meshes[i].intersect_AABB(start, direction))
		{
			vec3 closest_intersection_point;

			if (true == player_game_piece_meshes[i].intersect_triangles(start, direction, closest_intersection_point, false))
			{
				closest_intersection_point = player_game_piece_meshes[i].model_mat * vec4(closest_intersection_point, 1);




				if (first_assignment)
				{
					hover_collision_location = closest_intersection_point;

					hover_col_loc = player_game_piece;
					hover_collision_location_index = i;

					first_assignment = false;

					player_highlight_colours[i] = vec3(1, 0.5, 0);
					player_highlight_enabled[i] = true;

				}
				else
				{
					vec3 c0 = vec3(main_camera.eye) - closest_intersection_point;
					vec3 c1 = vec3(main_camera.eye) - hover_collision_location;

					if (length(c0) < length(c1))
					{
						hover_collision_location = closest_intersection_point;

						hover_col_loc = player_game_piece;
						hover_collision_location_index = i;

						// ... so that only one player is highlighted
						player_highlight_enabled.clear();
						player_highlight_enabled.resize(player_highlight_colours.size(), false);

						player_highlight_colours[i] = vec3(1, 0.5, 0);
						player_highlight_enabled[i] = true;
					}
				}
			}
		}
	}

	// Nothing was clicked on, so the background is set
	if (first_assignment)
	{
		hover_collision_location = vec3(0, 0, 0);
		hover_col_loc = background;
	}

	size_t number_players_highlighted = 0;

	for (size_t j = 0; j < player_highlight_enabled.size(); j++)
	{
		if (player_highlight_enabled[j])
		{
			number_players_highlighted++;
		}
	}

	if (number_players_highlighted > 0)
	{
		//cout << "num highlighted " << number_players_highlighted << endl;

		board_highlight_enabled.clear();
		board_highlight_enabled.resize(board_mesh.num_cells_wide * board_mesh.num_cells_wide, false);
	}
}






void get_collision_location(size_t x, size_t y)
{
	// Get intersection point closest to camera

	vec3 ray = screen_coords_to_world_coords(x, y, win_x, win_y);

	float t = 0;

	clicked_col_loc = background;
	bool first_assignment = true;

	vec4 start = vec4(main_camera.eye, 1.0);
	vec4 direction = vec4(ray, 0.0);
	direction = normalize(direction);

	// bounding box is axis aligned for the board mesh
	// bounding box is centred on xyz -- the mesh model matrix is the identity matrix
	// (assumes that board_mesh's model matrix equals mat4(1.0f))
	if (true == board_mesh.intersect_AABB(start, direction))
	{
		vec3 closest_intersection_point;

		for (size_t cell_x = 0; cell_x < board_mesh.num_cells_wide; cell_x++)
		{
			for (size_t cell_y = 0; cell_y < board_mesh.num_cells_wide; cell_y++)
			{
				if (true == board_mesh.intersect_triangles(start, direction, closest_intersection_point, cell_x, cell_y))
				{
					closest_intersection_point = board_mesh.model_mat * vec4(closest_intersection_point, 1);

					clicked_collision_location = closest_intersection_point;
					clicked_col_loc = game_board;
					clicked_cell_x = cell_x;
					clicked_cell_y = cell_y;
					first_assignment = false;
				}
			}
		}
	}


	for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
	{
		// Not necessarily axis-aligned (characters can rotate)
		mat4 inv = inverse(player_game_piece_meshes[i].model_mat);
		vec4 start = inv * vec4(main_camera.eye, 1.0);
		vec4 direction = inv * vec4(ray, 0.0);
		direction = normalize(direction);

		if (true == player_game_piece_meshes[i].intersect_AABB(start, direction))
		{
			vec3 closest_intersection_point;

			if (true == player_game_piece_meshes[i].intersect_triangles(start, direction, closest_intersection_point, false))
			{
				closest_intersection_point = player_game_piece_meshes[i].model_mat * vec4(closest_intersection_point, 1);

				if (first_assignment)
				{
					clicked_collision_location = closest_intersection_point;

					clicked_col_loc = player_game_piece;
					clicked_collision_location_index = i;

					first_assignment = false;
				}
				else
				{
					vec3 c0 = vec3(main_camera.eye) - closest_intersection_point;
					vec3 c1 = vec3(main_camera.eye) - clicked_collision_location;

					if (length(c0) < length(c1))
					{
						clicked_collision_location = closest_intersection_point;

						clicked_col_loc = player_game_piece;
						clicked_collision_location_index = i;
					}
				}
			}
		}
	}


	// Nothing was clicked on, so the background is set
	if (first_assignment)
	{
		clicked_collision_location = vec3(0, 0, 0);
		clicked_col_loc = background;
	}
}




void save_float_tex_to_disk(size_t win_x, size_t win_y, vector<float> &output_pixels, const char *file_name)
{
	size_t size = win_x * win_y;
	size_t num_bytes = size * 4;
	vector<unsigned char> output_pixels_bytes(num_bytes);

	for (int x = 0; x < win_x; x++)
	{
		for (int y = 0; y < win_y; y++)
		{
			size_t imgIdx = 4 * ((y * win_x) + x);
			size_t bufIdx = 4 * ((y * win_x) + x);

			output_pixels_bytes[imgIdx + 0] = output_pixels[imgIdx + 2] * 255.0f;
			output_pixels_bytes[imgIdx + 1] = output_pixels[imgIdx + 1] * 255.0f;
			output_pixels_bytes[imgIdx + 2] = output_pixels[imgIdx + 0] * 255.0f;
			output_pixels_bytes[imgIdx + 3] = output_pixels[imgIdx + 3] * 255.0f;
		}
	}

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
	unsigned char  bitsperpixel = 32;
	unsigned char  imagedescriptor = 0;
	vector<char> idstring;



	// Write Targa TGA file to disk.
	ofstream out(file_name, ios::binary);

	if (!out.is_open())
	{
		cout << "Failed to open TGA file for writing: " << file_name << endl;
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

	out.write(reinterpret_cast<char*>(&output_pixels_bytes[0]), num_bytes * sizeof(unsigned char));

	out.close();

}




#endif

