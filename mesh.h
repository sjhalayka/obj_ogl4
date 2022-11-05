#pragma once



#include "picopng.h"

#include "primitives.h"
#include "uv_camera.h"

#define OGT_VOX_IMPLEMENTATION
#include "ogt_vox.h"


#include <filesystem>
using std::filesystem::current_path;
using std::filesystem::path;
using std::filesystem::directory_entry;
using std::filesystem::recursive_directory_iterator;
using std::filesystem::directory_iterator;

#include <cstdlib>
#include "GL/glew.h"
#include "GL/glut.h"

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>
using std::swap;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <ios>
using std::ios_base;
using std::ios;

#include <set>
using std::set;

#include <map>
using std::map;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <regex>
using std::regex;
using std::sregex_token_iterator;

#include <sstream>
using std::istringstream;

#include <limits>
using std::numeric_limits;

#include <cstring> // for memcpy()

#include <random>
using std::mt19937;
using std::uniform_real_distribution;

class mesh
{
public:

	GLuint colour_tex = 0;
	GLuint specular_tex = 0;

	~mesh(void)
	{
		if (colour_tex != 0)
			glDeleteTextures(1, &colour_tex);

		if (specular_tex != 0)
			glDeleteTextures(1, &specular_tex);
	}

	vector<unsigned char> colour_data;
	vector<unsigned char> specular_data;

	size_t colour_data_x = 0, colour_data_y = 0;
	size_t specular_data_x = 0, specular_data_y = 0;


	GLuint get_colour_tex_handle(void)
	{
		return colour_tex;
	}

	GLuint get_specular_tex_handle(void)
	{
		return specular_tex;
	}


	vector<vector<triangle>> tri_vec;

	vector<vector<float>> opengl_vertex_data;

	mat4 model_mat;

	vec3 min_location, max_location;


	float get_y_extent(void)
	{
		float y_min = numeric_limits<float>::max();
		float y_max = numeric_limits<float>::min();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y < y_min)
						y_min = tri_vec[t][i].vertex[j].y;

					if (tri_vec[t][i].vertex[j].y > y_max)
						y_max = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return fabsf(y_min - y_max);
	}


	void centre_mesh_on_xyz(void)
	{
		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = numeric_limits<float>::min();
		float y_max = numeric_limits<float>::min();
		float z_max = numeric_limits<float>::min();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].x < x_min)
						x_min = tri_vec[t][i].vertex[j].x;

					if (tri_vec[t][i].vertex[j].x > x_max)
						x_max = tri_vec[t][i].vertex[j].x;

					if (tri_vec[t][i].vertex[j].y < y_min)
						y_min = tri_vec[t][i].vertex[j].y;

					if (tri_vec[t][i].vertex[j].y > y_max)
						y_max = tri_vec[t][i].vertex[j].y;

					if (tri_vec[t][i].vertex[j].z < z_min)
						z_min = tri_vec[t][i].vertex[j].z;

					if (tri_vec[t][i].vertex[j].z > z_max)
						z_max = tri_vec[t][i].vertex[j].z;
				}
			}
		}

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					tri_vec[t][i].vertex[j].x += -(x_max + x_min) / 2.0f;
					tri_vec[t][i].vertex[j].y += -(y_max + y_min) / 2.0f;
					tri_vec[t][i].vertex[j].z += -(z_max + z_min) / 2.0f;
				}
			}
		}
	}

	void std_strtok(const string& s, const string& regex_s, vector<string>& tokens)
	{
		tokens.clear();

		regex r(regex_s);

		sregex_token_iterator iter(s.begin(), s.end(), r, -1);
		sregex_token_iterator end;

		while (iter != end)
			tokens.push_back(*iter++);
	}

	vector<string> get_masked_paths(string filemask)
	{
		vector<string> ret;

		// Get current working directory
		const path p{ current_path() };

		// Recursively enumerate files and sub-directories
		for (const directory_entry dir_entry : directory_iterator{ p })
		{
			if (dir_entry.path().extension() == ".obj" && dir_entry.is_regular_file())
			{
				string s = dir_entry.path().string();

				for (size_t i = 0; i < s.size(); i++)
					if (s[i] == '/')
						s[i] = '\\';

				vector<string> tokens;
				std_strtok(s, "[\\\\]", tokens);

				string filename = tokens[tokens.size() - 1];
				string truncated_filename = filename;
				truncated_filename.resize(filename.size() - 4);

				string truncated_filename_without_extension = truncated_filename;

				truncated_filename_without_extension.resize(filemask.size());

				if (truncated_filename_without_extension == filemask)
				{
					ret.push_back(filename);
				}
			}
		}

		return ret;
	}

	bool read_quads_from_obj_array(bool cull_back_faces, string filename_base)
	{
		tri_vec.clear();

		vector<string> filenames = get_masked_paths(filename_base);

		if (filenames.size() == 0)
			return false;

		tri_vec.resize(filenames.size());

		for (size_t i = 0; i < filenames.size(); i++)
		{
			if (false == read_quads_from_wavefront_obj_file(cull_back_faces, filenames[i], i))
				return false;
		}

		centre_mesh_on_xyz();


		init_opengl_data();

		return true;
	}

	void process_line(bool cull_back_faces, string line, 
		vector<triangle> &triangles, 
		vector<vertex_3> &verts,
		vector<uv_coord> &tex_coords,
		vector<vertex_3> &norms,
		map<vertex_3, size_t> &face_normal_counts)
	{
		vector<string> tokens;

		std_strtok(line, "[ \t]", tokens);

		vertex_3 vtemp;

		if (tokens[0] == "vn")
		{
			if (tokens.size() != 4)
				return;

			vertex_3 vn;

			istringstream iss(tokens[1]);
			iss >> vn.x;

			iss.clear();
			iss.str(tokens[2]);
			iss >> vn.y;

			iss.clear();
			iss.str(tokens[3]);
			iss >> vn.z;

			norms.push_back(vn);
		}
		else if (tokens[0] == "vt")
		{
			if (tokens.size() != 3)
				return;

			uv_coord c;

			istringstream iss(tokens[1]);
			iss >> c.u;

			iss.clear();
			iss.str(tokens[2]);
			iss >> c.v;

			tex_coords.push_back(c);
		}
		else if (tokens[0] == "v")
		{
			if (tokens.size() != 4)
				return;

			vertex_3 v;

			istringstream iss(tokens[1]);
			iss >> v.x;

			iss.clear();
			iss.str(tokens[2]);
			iss >> v.y;

			iss.clear();
			iss.str(tokens[3]);
			iss >> v.z;

			verts.push_back(v);
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 5)
				return;

			//v / vt / vn

			quad q;
			//triangle t;

			vector<string> face_tokens;


			std_strtok(tokens[1], "[/]", face_tokens);

			if (face_tokens.size() != 3)
				return;

			istringstream iss;
			size_t v_index = 0;
			size_t vt_index = 0;
			size_t vn_index = 0;

			iss.clear();
			iss.str(face_tokens[0]);
			iss >> v_index;
			v_index--;

			iss.clear();
			iss.str(face_tokens[1]);
			iss >> vt_index;
			vt_index--;

			iss.clear();
			iss.str(face_tokens[2]);
			iss >> vn_index;
			vn_index--;

			q.vertex[0].x = verts[v_index].x;
			q.vertex[0].y = verts[v_index].y;
			q.vertex[0].z = verts[v_index].z;
			q.vertex[0].u = tex_coords[vt_index].u;
			q.vertex[0].v = tex_coords[vt_index].v;
			q.vertex[0].nx = norms[vn_index].x;
			q.vertex[0].ny = norms[vn_index].y;
			q.vertex[0].nz = norms[vn_index].z;

			vtemp = vertex_3(norms[vn_index].x, norms[vn_index].y, norms[vn_index].z);
			face_normal_counts[vtemp]++;










			std_strtok(tokens[2], "[/]", face_tokens);

			if (face_tokens.size() != 3)
				return;

			iss.clear();
			iss.str(face_tokens[0]);
			iss >> v_index;
			v_index--;

			iss.clear();
			iss.str(face_tokens[1]);
			iss >> vt_index;
			vt_index--;

			iss.clear();
			iss.str(face_tokens[2]);
			iss >> vn_index;
			vn_index--;

			q.vertex[1].x = verts[v_index].x;
			q.vertex[1].y = verts[v_index].y;
			q.vertex[1].z = verts[v_index].z;
			q.vertex[1].u = tex_coords[vt_index].u;
			q.vertex[1].v = tex_coords[vt_index].v;
			q.vertex[1].nx = norms[vn_index].x;
			q.vertex[1].ny = norms[vn_index].y;
			q.vertex[1].nz = norms[vn_index].z;

			vtemp = vertex_3(norms[vn_index].x, norms[vn_index].y, norms[vn_index].z);
			face_normal_counts[vtemp]++;




			std_strtok(tokens[3], "[/]", face_tokens);

			if (face_tokens.size() != 3)
				return;


			iss.clear();
			iss.str(face_tokens[0]);
			iss >> v_index;
			v_index--;

			iss.clear();
			iss.str(face_tokens[1]);
			iss >> vt_index;
			vt_index--;

			iss.clear();
			iss.str(face_tokens[2]);
			iss >> vn_index;
			vn_index--;

			q.vertex[2].x = verts[v_index].x;
			q.vertex[2].y = verts[v_index].y;
			q.vertex[2].z = verts[v_index].z;
			q.vertex[2].u = tex_coords[vt_index].u;
			q.vertex[2].v = tex_coords[vt_index].v;
			q.vertex[2].nx = norms[vn_index].x;
			q.vertex[2].ny = norms[vn_index].y;
			q.vertex[2].nz = norms[vn_index].z;

			vtemp = vertex_3(norms[vn_index].x, norms[vn_index].y, norms[vn_index].z);
			face_normal_counts[vtemp]++;




			std_strtok(tokens[4], "[/]", face_tokens);

			if (face_tokens.size() != 3)
				return;


			iss.clear();
			iss.str(face_tokens[0]);
			iss >> v_index;
			v_index--;

			iss.clear();
			iss.str(face_tokens[1]);
			iss >> vt_index;
			vt_index--;

			iss.clear();
			iss.str(face_tokens[2]);
			iss >> vn_index;
			vn_index--;

			q.vertex[3].x = verts[v_index].x;
			q.vertex[3].y = verts[v_index].y;
			q.vertex[3].z = verts[v_index].z;
			q.vertex[3].u = tex_coords[vt_index].u;
			q.vertex[3].v = tex_coords[vt_index].v;
			q.vertex[3].nx = norms[vn_index].x;
			q.vertex[3].ny = norms[vn_index].y;
			q.vertex[3].nz = norms[vn_index].z;

			vtemp = vertex_3(norms[vn_index].x, norms[vn_index].y, norms[vn_index].z);
			face_normal_counts[vtemp]++;

			if (cull_back_faces)
			{
				if (q.vertex[3].nx < 0 || q.vertex[3].nz > 0 || q.vertex[3].ny == 1)
				{
					triangle t;
					t.vertex[0] = q.vertex[0];
					t.vertex[1] = q.vertex[1];
					t.vertex[2] = q.vertex[2];
					triangles.push_back(t);

					t.vertex[0] = q.vertex[0];
					t.vertex[1] = q.vertex[2];
					t.vertex[2] = q.vertex[3];
					triangles.push_back(t);
				}
			}
			else
			{
				triangle t;
				t.vertex[0] = q.vertex[0];
				t.vertex[1] = q.vertex[1];
				t.vertex[2] = q.vertex[2];
				triangles.push_back(t);

				t.vertex[0] = q.vertex[0];
				t.vertex[1] = q.vertex[2];
				t.vertex[2] = q.vertex[3];
				triangles.push_back(t);
			}
		}
	}


	bool read_quads_from_wavefront_obj_file(bool cull_back_faces, string file_name, size_t index)
	{
		tri_vec[index].clear();

		vector<triangle> triangles;

		vector<vertex_3> verts;
		vector<uv_coord> tex_coords;
		vector<vertex_3> norms;

		string line;

		map<vertex_3, size_t> face_normal_counts;

		ifstream infile(file_name, ifstream::ate | ifstream::binary);

		if (infile.fail())
		{
			cout << "Could not open file" << endl;
			return false;
		}

		size_t file_size = infile.tellg();

		infile.close();

		if (file_size == 0)
			return false;

		infile.open(file_name, ifstream::binary);

		if (infile.fail())
		{
			cout << "Could not re-open file" << endl;
			return false;
		}

		string s(file_size, ' ');

		infile.read(&s[0], file_size);
		infile.close();

		// Make sure that the last line ends in a newline
		if (s[s.size() - 1] != '\n')
		{
			s += '\n';
			file_size++;
		}

		string temp_token;

		// For each character in the file
		for (size_t i = 0; i < file_size; i++)
		{
			if (s[i] == '\r')
			{
				// Skip over Windows carriage return
				continue;
			}
			else if (s[i] == '\n')
			{
				process_line(cull_back_faces, temp_token, triangles, verts, tex_coords, norms, face_normal_counts);

				temp_token.clear();
			}
			else
			{
				temp_token += s[i];
			}
		}

		tri_vec[index] = triangles;

		return true;
	}





	void draw(GLint render_shader_program,
		int win_x,
		int win_y,
		string texture_filename, string specular_texture_filename);

	vec3 geodesic_dir;
	vec3 geodesic_left;
	vec3 geodesic_tangent;
	float displacement;

	void init_model_matrix(vec3 dir, vec3 left, vec3 tangent)
	{
		geodesic_dir = dir;
		geodesic_left = left;
		geodesic_tangent = tangent;

		set_transform();
	}

	void set_transform(void)
	{
		const vec3 n_forward = normalize(geodesic_tangent);
		const vec3 n_up = normalize(geodesic_dir);
		const vec3 n_left = normalize(geodesic_left);

		model_mat[0] = normalize(vec4(n_left, 0.0f));
		model_mat[1] = normalize(vec4(n_forward, 0.0f));
		model_mat[2] = normalize(vec4(n_up, 0.0f));
		model_mat[3] = vec4(n_up * displacement, 1.0f);
	}

	bool read_quads_from_vox_file(string file_name, bool cull_faces)
	{
		tri_vec.clear();
		opengl_vertex_data.clear();

		tri_vec.resize(1);
		opengl_vertex_data.resize(1);

		ifstream infile(file_name, ifstream::ate | ifstream::binary);

		if (infile.fail())
		{
			//cout << "Could not open file " << file_name << endl;
			return false;
		}

		size_t file_size = infile.tellg();

		infile.close();

		if (file_size == 0)
			return false;

		infile.open(file_name, ifstream::binary);

		if (infile.fail())
		{
			//cout << "Could not re-open file " << file_name << endl;
			return false;
		}

		vector<unsigned char> v(file_size, 0);

		infile.read(reinterpret_cast<char*>(&v[0]), file_size);
		infile.close();

		const ogt_vox_scene* scene = ogt_vox_read_scene(&v[0], file_size);

		cout << scene->num_models << endl;



		for (size_t x = 0; x < scene->models[0]->size_x; x++)
		{
			for (size_t y = 0; y < scene->models[0]->size_y; y++)
			{
				for (size_t z = 0; z < scene->models[0]->size_z; z++)
				{
					float scale = 0.1;

					const size_t voxel_index = x + (y * scene->models[0]->size_x) + (z * scene->models[0]->size_x * scene->models[0]->size_y);
					const uint8_t colour_index = scene->models[0]->voxel_data[voxel_index];

					if (colour_index == 0)
						continue;

					const float uv_index = float(colour_index - 1) / 254.0f;

					vertex_3 translate(x * scale, y * scale, z * scale);

					quad q0, q1, q2, q3, q4, q5;

					// Top face (y = 1.0f)
					q0.vertex[0] = vertex_3(scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q0.vertex[1] = vertex_3(-scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q0.vertex[2] = vertex_3(-scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q0.vertex[3] = vertex_3(scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q0.vertex[0].ny = 1;
					q0.vertex[1].ny = 1;
					q0.vertex[2].ny = 1;
					q0.vertex[3].ny = 1;
					q0.vertex[0].u = uv_index;
					q0.vertex[1].u = uv_index;
					q0.vertex[2].u = uv_index;
					q0.vertex[3].u = uv_index;



					// Bottom face (y = -scale*0.5f)
					q1.vertex[0] = vertex_3(scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q1.vertex[1] = vertex_3(-scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q1.vertex[2] = vertex_3(-scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q1.vertex[3] = vertex_3(scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q1.vertex[0].ny = -1;
					q1.vertex[1].ny = -1;
					q1.vertex[2].ny = -1;
					q1.vertex[3].ny = -1;
					q1.vertex[0].u = uv_index;
					q1.vertex[1].u = uv_index;
					q1.vertex[2].u = uv_index;
					q1.vertex[3].u = uv_index;



					// Front face  (z = scale*0.5f)
					q2.vertex[0] = vertex_3(scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[1] = vertex_3(-scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[2] = vertex_3(-scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[3] = vertex_3(scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[0].nz = 1;
					q2.vertex[1].nz = 1;
					q2.vertex[2].nz = 1;
					q2.vertex[3].nz = 1;
					q2.vertex[0].u = uv_index;
					q2.vertex[1].u = uv_index;
					q2.vertex[2].u = uv_index;
					q2.vertex[3].u = uv_index;

					// Back face (z = -scale*0.5f)
					q3.vertex[0] = vertex_3(scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[1] = vertex_3(-scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[2] = vertex_3(-scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[3] = vertex_3(scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[0].nz = -1;
					q3.vertex[1].nz = -1;
					q3.vertex[2].nz = -1;
					q3.vertex[3].nz = -1;
					q3.vertex[0].u = uv_index;
					q3.vertex[1].u = uv_index;
					q3.vertex[2].u = uv_index;
					q3.vertex[3].u = uv_index;

					// Right face (x = scale*0.5f)
					q4.vertex[0] = vertex_3(scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q4.vertex[1] = vertex_3(scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q4.vertex[2] = vertex_3(scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q4.vertex[3] = vertex_3(scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q4.vertex[0].nx = 1;
					q4.vertex[1].nx = 1;
					q4.vertex[2].nx = 1;
					q4.vertex[3].nx = 1;
					q4.vertex[0].u = uv_index;
					q4.vertex[1].u = uv_index;
					q4.vertex[2].u = uv_index;
					q4.vertex[3].u = uv_index;


					// Left face (x = -scale*0.5f)
					q5.vertex[0] = vertex_3(-scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q5.vertex[1] = vertex_3(-scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q5.vertex[2] = vertex_3(-scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q5.vertex[3] = vertex_3(-scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q5.vertex[0].nx = -1;
					q5.vertex[1].nx = -1;
					q5.vertex[2].nx = -1;
					q5.vertex[3].nx = -1;
					q5.vertex[0].u = uv_index;
					q5.vertex[1].u = uv_index;
					q5.vertex[2].u = uv_index;
					q5.vertex[3].u = uv_index;




					triangle t;
					size_t neighbour_index = 0;

					neighbour_index = x + (y + 1) * scene->models[0]->size_x + z * scene->models[0]->size_x * scene->models[0]->size_y;
					if (y == scene->models[0]->size_y - 1 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{						

							t.vertex[0] = q0.vertex[0];
							t.vertex[1] = q0.vertex[1];
							t.vertex[2] = q0.vertex[2];
							tri_vec[0].push_back(t);

							t.vertex[0] = q0.vertex[0];
							t.vertex[1] = q0.vertex[2];
							t.vertex[2] = q0.vertex[3];
							tri_vec[0].push_back(t);
					}

					neighbour_index = x + (y - 1) * scene->models[0]->size_x + z * scene->models[0]->size_x * scene->models[0]->size_y;
					if (y == 0 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (1)//cull_faces == false) // || q1.vertex[3].nx < 0 || q1.vertex[3].nz > 0 || q1.vertex[3].ny == 1)
						{

							t.vertex[0] = q1.vertex[0];
							t.vertex[1] = q1.vertex[1];
							t.vertex[2] = q1.vertex[2];
							tri_vec[0].push_back(t);

							t.vertex[0] = q1.vertex[0];
							t.vertex[1] = q1.vertex[2];
							t.vertex[2] = q1.vertex[3];
							tri_vec[0].push_back(t);
						}
					}

					neighbour_index = x + (y)*scene->models[0]->size_x + (z + 1) * scene->models[0]->size_x * scene->models[0]->size_y;
					if (z == scene->models[0]->size_z - 1 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{

							t.vertex[0] = q2.vertex[0];
							t.vertex[1] = q2.vertex[1];
							t.vertex[2] = q2.vertex[2];
							tri_vec[0].push_back(t);

							t.vertex[0] = q2.vertex[0];
							t.vertex[1] = q2.vertex[2];
							t.vertex[2] = q2.vertex[3];
							tri_vec[0].push_back(t);
					}


					neighbour_index = x + (y)*scene->models[0]->size_x + (z - 1) * scene->models[0]->size_x * scene->models[0]->size_y;
					if (z == 0 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (1)//cull_faces == false) //|| q3.vertex[3].nx < 0 || q3.vertex[3].nz > 0 || q3.vertex[3].ny == 1)
						{
							t.vertex[0] = q3.vertex[0];
							t.vertex[1] = q3.vertex[1];
							t.vertex[2] = q3.vertex[2];
							tri_vec[0].push_back(t);

							t.vertex[0] = q3.vertex[0];
							t.vertex[1] = q3.vertex[2];
							t.vertex[2] = q3.vertex[3];
							tri_vec[0].push_back(t);
						}
					}
						
					neighbour_index = (x + 1) + (y)*scene->models[0]->size_x + (z)*scene->models[0]->size_x * scene->models[0]->size_y;
					if (x == scene->models[0]->size_x - 1 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
							t.vertex[0] = q4.vertex[0];
							t.vertex[1] = q4.vertex[1];
							t.vertex[2] = q4.vertex[2];
							tri_vec[0].push_back(t);

							t.vertex[0] = q4.vertex[0];
							t.vertex[1] = q4.vertex[2];
							t.vertex[2] = q4.vertex[3];
							tri_vec[0].push_back(t);

					}

					neighbour_index = (x - 1) + (y)*scene->models[0]->size_x + (z)*scene->models[0]->size_x * scene->models[0]->size_y;
					if (x == 0 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (1)//cull_faces == false)// || q5.vertex[3].nx < 0 || q5.vertex[3].nz > 0 || q5.vertex[3].ny == 1)
						{
							t.vertex[0] = q5.vertex[0];
							t.vertex[1] = q5.vertex[1];
							t.vertex[2] = q5.vertex[2];
							tri_vec[0].push_back(t);

							t.vertex[0] = q5.vertex[0];
							t.vertex[1] = q5.vertex[2];
							t.vertex[2] = q5.vertex[3];
							tri_vec[0].push_back(t);
						}
					}
				}
			}
		}

		ogt_vox_destroy_scene(scene);

		centre_mesh_on_xyz();

		for (size_t i = 0; i < tri_vec.size(); i++)
		{
			for (size_t j = 0; j < tri_vec[i].size(); j++)
			{
				vertex_3 normal0 = vertex_3(tri_vec[i][j].vertex[0].nx, tri_vec[i][j].vertex[0].ny, tri_vec[i][j].vertex[0].nz);
				vertex_3 normal1 = vertex_3(tri_vec[i][j].vertex[1].nx, tri_vec[i][j].vertex[1].ny, tri_vec[i][j].vertex[1].nz);
				vertex_3 normal2 = vertex_3(tri_vec[i][j].vertex[2].nx, tri_vec[i][j].vertex[2].ny, tri_vec[i][j].vertex[2].nz);

				tri_vec[i][j].vertex[0].rotate_x( pi<float>() - pi<float>() / 2.0f);
				tri_vec[i][j].vertex[1].rotate_x( pi<float>() - pi<float>() / 2.0f);
				tri_vec[i][j].vertex[2].rotate_x( pi<float>() - pi<float>() / 2.0f);
				normal0.rotate_x(pi<float>() - pi<float>() / 2.0f);
				normal1.rotate_x(pi<float>() - pi<float>() / 2.0f);
				normal2.rotate_x(pi<float>() - pi<float>() / 2.0f);

				tri_vec[i][j].vertex[0].rotate_y(pi<float>() / 2.0f);
				tri_vec[i][j].vertex[1].rotate_y(pi<float>() / 2.0f);
				tri_vec[i][j].vertex[2].rotate_y(pi<float>() / 2.0f);
				normal0.rotate_y(pi<float>() / 2.0f);
				normal1.rotate_y(pi<float>() / 2.0f);
				normal2.rotate_y(pi<float>() / 2.0f);

				tri_vec[i][j].vertex[0].nx = normal0.x;
				tri_vec[i][j].vertex[0].ny = normal0.y;
				tri_vec[i][j].vertex[0].nz = normal0.z;
				tri_vec[i][j].vertex[1].nx = normal1.x;
				tri_vec[i][j].vertex[1].ny = normal1.y;
				tri_vec[i][j].vertex[1].nz = normal1.z;
				tri_vec[i][j].vertex[2].nx = normal2.x;
				tri_vec[i][j].vertex[2].ny = normal2.y;
				tri_vec[i][j].vertex[2].nz = normal2.z;

			}
		}

		init_opengl_data();

		return true;
	}










protected:
	void init_opengl_data(void);
};



// Inherit from class A
class binned_mesh : public mesh
{
public:


	const size_t level_size = 128;
	const size_t grid_cell_size = 16;
	const size_t num_cells_wide = level_size / grid_cell_size;







	bool read_quads_from_vox_file(string file_name, bool cull_faces)
	{
		tri_vec.clear();
		opengl_vertex_data.clear();
//		opengl_vertex_data.resize(num_cells * num_cells); // to do: is this needed?

		ifstream infile(file_name, ifstream::ate | ifstream::binary);

		if (infile.fail())
		{
			//cout << "Could not open file " << file_name << endl;
			return false;
		}

		size_t file_size = infile.tellg();

		infile.close();

		if (file_size == 0)
			return false;

		infile.open(file_name, ifstream::binary);

		if (infile.fail())
		{
			//cout << "Could not re-open file " << file_name << endl;
			return false;
		}

		vector<unsigned char> v(file_size, 0);

		infile.read(reinterpret_cast<char*>(&v[0]), file_size);
		infile.close();

		const ogt_vox_scene* scene = ogt_vox_read_scene(&v[0], file_size);

		cout << scene->num_models << endl;

		//size_t level_size = scene->models[0]->size_x;
		//size_t grid_cell_size = 16;
		//size_t num_cells = level_size / grid_cell_size;

		tri_vec.resize(num_cells_wide * num_cells_wide);

		for (size_t x = 0; x < scene->models[0]->size_x; x++)
		{
			for (size_t y = 0; y < scene->models[0]->size_y; y++)
			{
				for (size_t z = 0; z < scene->models[0]->size_z; z++)
				{
					size_t x_cell = x / grid_cell_size;
					size_t y_cell = y / grid_cell_size;
					size_t cell_index = y_cell * num_cells_wide + x_cell;

					float scale = 0.1;

					const size_t voxel_index = x + (y * scene->models[0]->size_x) + (z * scene->models[0]->size_x * scene->models[0]->size_y);
					const uint8_t colour_index = scene->models[0]->voxel_data[voxel_index];

					if (colour_index == 0)
						continue;

					const float uv_index = float(colour_index - 1) / 254.0f;

					vertex_3 translate(x * scale, y * scale, z * scale);

					quad q0, q1, q2, q3, q4, q5;

					// Top face (y = 1.0f)
					q0.vertex[0] = vertex_3(scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q0.vertex[1] = vertex_3(-scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q0.vertex[2] = vertex_3(-scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q0.vertex[3] = vertex_3(scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q0.vertex[0].ny = 1;
					q0.vertex[1].ny = 1;
					q0.vertex[2].ny = 1;
					q0.vertex[3].ny = 1;
					q0.vertex[0].u = uv_index;
					q0.vertex[1].u = uv_index;
					q0.vertex[2].u = uv_index;
					q0.vertex[3].u = uv_index;



					// Bottom face (y = -scale*0.5f)
					q1.vertex[0] = vertex_3(scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q1.vertex[1] = vertex_3(-scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q1.vertex[2] = vertex_3(-scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q1.vertex[3] = vertex_3(scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q1.vertex[0].ny = -1;
					q1.vertex[1].ny = -1;
					q1.vertex[2].ny = -1;
					q1.vertex[3].ny = -1;
					q1.vertex[0].u = uv_index;
					q1.vertex[1].u = uv_index;
					q1.vertex[2].u = uv_index;
					q1.vertex[3].u = uv_index;



					// Front face  (z = scale*0.5f)
					q2.vertex[0] = vertex_3(scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[1] = vertex_3(-scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[2] = vertex_3(-scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[3] = vertex_3(scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q2.vertex[0].nz = 1;
					q2.vertex[1].nz = 1;
					q2.vertex[2].nz = 1;
					q2.vertex[3].nz = 1;
					q2.vertex[0].u = uv_index;
					q2.vertex[1].u = uv_index;
					q2.vertex[2].u = uv_index;
					q2.vertex[3].u = uv_index;

					// Back face (z = -scale*0.5f)
					q3.vertex[0] = vertex_3(scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[1] = vertex_3(-scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[2] = vertex_3(-scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[3] = vertex_3(scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q3.vertex[0].nz = -1;
					q3.vertex[1].nz = -1;
					q3.vertex[2].nz = -1;
					q3.vertex[3].nz = -1;
					q3.vertex[0].u = uv_index;
					q3.vertex[1].u = uv_index;
					q3.vertex[2].u = uv_index;
					q3.vertex[3].u = uv_index;

					// Right face (x = scale*0.5f)
					q4.vertex[0] = vertex_3(scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q4.vertex[1] = vertex_3(scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q4.vertex[2] = vertex_3(scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q4.vertex[3] = vertex_3(scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q4.vertex[0].nx = 1;
					q4.vertex[1].nx = 1;
					q4.vertex[2].nx = 1;
					q4.vertex[3].nx = 1;
					q4.vertex[0].u = uv_index;
					q4.vertex[1].u = uv_index;
					q4.vertex[2].u = uv_index;
					q4.vertex[3].u = uv_index;


					// Left face (x = -scale*0.5f)
					q5.vertex[0] = vertex_3(-scale * 0.5f, scale * 0.5f, scale * 0.5f) + translate;
					q5.vertex[1] = vertex_3(-scale * 0.5f, scale * 0.5f, -scale * 0.5f) + translate;
					q5.vertex[2] = vertex_3(-scale * 0.5f, -scale * 0.5f, -scale * 0.5f) + translate;
					q5.vertex[3] = vertex_3(-scale * 0.5f, -scale * 0.5f, scale * 0.5f) + translate;
					q5.vertex[0].nx = -1;
					q5.vertex[1].nx = -1;
					q5.vertex[2].nx = -1;
					q5.vertex[3].nx = -1;
					q5.vertex[0].u = uv_index;
					q5.vertex[1].u = uv_index;
					q5.vertex[2].u = uv_index;
					q5.vertex[3].u = uv_index;




					triangle t;
					size_t neighbour_index = 0;

					neighbour_index = x + (y + 1) * scene->models[0]->size_x + z * scene->models[0]->size_x * scene->models[0]->size_y;
					if (y == scene->models[0]->size_y - 1 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (cull_faces == false)
						{
							t.vertex[0] = q0.vertex[0];
							t.vertex[1] = q0.vertex[1];
							t.vertex[2] = q0.vertex[2];
							tri_vec[cell_index].push_back(t);

							t.vertex[0] = q0.vertex[0];
							t.vertex[1] = q0.vertex[2];
							t.vertex[2] = q0.vertex[3];
							tri_vec[cell_index].push_back(t);
						}
					}

					neighbour_index = x + (y - 1) * scene->models[0]->size_x + z * scene->models[0]->size_x * scene->models[0]->size_y;
					if (y == 0 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (1)//if (cull_faces == false)/cull_faces == false) // || q1.vertex[3].nx < 0 || q1.vertex[3].nz > 0 || q1.vertex[3].ny == 1)
						{

							t.vertex[0] = q1.vertex[0];
							t.vertex[1] = q1.vertex[1];
							t.vertex[2] = q1.vertex[2];
							tri_vec[cell_index].push_back(t);

							t.vertex[0] = q1.vertex[0];
							t.vertex[1] = q1.vertex[2];
							t.vertex[2] = q1.vertex[3];
							tri_vec[cell_index].push_back(t);
						}
					}

					neighbour_index = x + (y)*scene->models[0]->size_x + (z + 1) * scene->models[0]->size_x * scene->models[0]->size_y;
					if (z == scene->models[0]->size_z - 1 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{

						t.vertex[0] = q2.vertex[0];
						t.vertex[1] = q2.vertex[1];
						t.vertex[2] = q2.vertex[2];
						tri_vec[cell_index].push_back(t);

						t.vertex[0] = q2.vertex[0];
						t.vertex[1] = q2.vertex[2];
						t.vertex[2] = q2.vertex[3];
						tri_vec[cell_index].push_back(t);
					}


					neighbour_index = x + (y)*scene->models[0]->size_x + (z - 1) * scene->models[0]->size_x * scene->models[0]->size_y;
					if (z == 0 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (cull_faces == false) //|| q3.vertex[3].nx < 0 || q3.vertex[3].nz > 0 || q3.vertex[3].ny == 1)
						{
							t.vertex[0] = q3.vertex[0];
							t.vertex[1] = q3.vertex[1];
							t.vertex[2] = q3.vertex[2];
							tri_vec[cell_index].push_back(t);

							t.vertex[0] = q3.vertex[0];
							t.vertex[1] = q3.vertex[2];
							t.vertex[2] = q3.vertex[3];
							tri_vec[cell_index].push_back(t);
						}
					}

					neighbour_index = (x + 1) + (y)*scene->models[0]->size_x + (z)*scene->models[0]->size_x * scene->models[0]->size_y;
					if (x == scene->models[0]->size_x - 1 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (cull_faces == false)
						{
							t.vertex[0] = q4.vertex[0];
							t.vertex[1] = q4.vertex[1];
							t.vertex[2] = q4.vertex[2];
							tri_vec[cell_index].push_back(t);

							t.vertex[0] = q4.vertex[0];
							t.vertex[1] = q4.vertex[2];
							t.vertex[2] = q4.vertex[3];
							tri_vec[cell_index].push_back(t);
						}

					}

					neighbour_index = (x - 1) + (y)*scene->models[0]->size_x + (z)*scene->models[0]->size_x * scene->models[0]->size_y;
					if (x == 0 || 0 == scene->models[0]->voxel_data[neighbour_index])
					{
						if (1)//cull_faces == false)// || q5.vertex[3].nx < 0 || q5.vertex[3].nz > 0 || q5.vertex[3].ny == 1)
						{
							t.vertex[0] = q5.vertex[0];
							t.vertex[1] = q5.vertex[1];
							t.vertex[2] = q5.vertex[2];
							tri_vec[cell_index].push_back(t);

							t.vertex[0] = q5.vertex[0];
							t.vertex[1] = q5.vertex[2];
							t.vertex[2] = q5.vertex[3];
							tri_vec[cell_index].push_back(t);
						}
					}
				}
			}
		}

		ogt_vox_destroy_scene(scene);

		centre_mesh_on_xyz();

		for (size_t i = 0; i < tri_vec.size(); i++)
		{
			for (size_t j = 0; j < tri_vec[i].size(); j++)
			{
				vertex_3 normal0 = vertex_3(tri_vec[i][j].vertex[0].nx, tri_vec[i][j].vertex[0].ny, tri_vec[i][j].vertex[0].nz);
				vertex_3 normal1 = vertex_3(tri_vec[i][j].vertex[1].nx, tri_vec[i][j].vertex[1].ny, tri_vec[i][j].vertex[1].nz);
				vertex_3 normal2 = vertex_3(tri_vec[i][j].vertex[2].nx, tri_vec[i][j].vertex[2].ny, tri_vec[i][j].vertex[2].nz);

				tri_vec[i][j].vertex[0].rotate_x(pi<float>() - pi<float>() / 2.0f);
				tri_vec[i][j].vertex[1].rotate_x(pi<float>() - pi<float>() / 2.0f);
				tri_vec[i][j].vertex[2].rotate_x(pi<float>() - pi<float>() / 2.0f);
				normal0.rotate_x(pi<float>() - pi<float>() / 2.0f);
				normal1.rotate_x(pi<float>() - pi<float>() / 2.0f);
				normal2.rotate_x(pi<float>() - pi<float>() / 2.0f);

				tri_vec[i][j].vertex[0].rotate_y(pi<float>() / 2.0f);
				tri_vec[i][j].vertex[1].rotate_y(pi<float>() / 2.0f);
				tri_vec[i][j].vertex[2].rotate_y(pi<float>() / 2.0f);
				normal0.rotate_y(pi<float>() / 2.0f);
				normal1.rotate_y(pi<float>() / 2.0f);
				normal2.rotate_y(pi<float>() / 2.0f);

				tri_vec[i][j].vertex[0].nx = normal0.x;
				tri_vec[i][j].vertex[0].ny = normal0.y;
				tri_vec[i][j].vertex[0].nz = normal0.z;
				tri_vec[i][j].vertex[1].nx = normal1.x;
				tri_vec[i][j].vertex[1].ny = normal1.y;
				tri_vec[i][j].vertex[1].nz = normal1.z;
				tri_vec[i][j].vertex[2].nx = normal2.x;
				tri_vec[i][j].vertex[2].ny = normal2.y;
				tri_vec[i][j].vertex[2].nz = normal2.z;

			}
		}

		init_opengl_data();

		return true;
	}




	void draw(GLint render_shader_program,
		size_t cell_x, size_t cell_y,
		int win_x,
		int win_y,
		string texture_filename, string specular_texture_filename)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		glUseProgram(render_shader_program);

		const GLuint components_per_vertex = 8;
		const GLuint components_per_position = 3;
		const GLuint components_per_normal = 3;
		const GLuint components_per_texcoord = 2;

		if (colour_tex == 0)
		{
			std::vector<unsigned char> buffer;
			loadFile(buffer, texture_filename.c_str());
			unsigned long w, h;
			decodePNG(colour_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

			colour_data_x = w;
			colour_data_y = h;

			glGenTextures(1, &colour_tex);
			glBindTexture(GL_TEXTURE_2D, colour_tex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}


		if (specular_tex == 0)
		{
			std::vector<unsigned char> buffer;
			loadFile(buffer, specular_texture_filename.c_str());
			unsigned long w, h;
			decodePNG(specular_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

			specular_data_x = w;
			specular_data_y = h;

			glGenTextures(1, &specular_tex);
			glBindTexture(GL_TEXTURE_2D, specular_tex);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glBindTexture(GL_TEXTURE_2D, colour_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(colour_data_x), static_cast<GLsizei>(colour_data_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour_data[0]);

		glBindTexture(GL_TEXTURE_2D, specular_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(specular_data_x), static_cast<GLsizei>(specular_data_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, &specular_data[0]);


		//for (size_t v = 0; v < opengl_vertex_data.size(); v++)
		{
			GLuint num_vertices = static_cast<GLuint>(opengl_vertex_data[cell_index].size()) / components_per_vertex;

			unsigned int VBO, VAO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, opengl_vertex_data[cell_index].size() * sizeof(GLfloat), &opengl_vertex_data[cell_index][0], GL_DYNAMIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
			// normal attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			// texture coord attribute
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			glDrawArrays(GL_TRIANGLES, 0, num_vertices);

			glDeleteBuffers(1, &VBO);
			glDeleteVertexArrays(1, &VAO);
		}

	}


	float get_y_extent(void)
	{
		float y_min = numeric_limits<float>::max();
		float y_max = numeric_limits<float>::min();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y < y_min)
						y_min = tri_vec[t][i].vertex[j].y;

					if (tri_vec[t][i].vertex[j].y > y_max)
						y_max = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return fabsf(y_min - y_max);
	}

	float get_y_extent(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float y_min = numeric_limits<float>::max();
		float y_max = numeric_limits<float>::min();

			for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[cell_index][i].vertex[j].y < y_min)
						y_min = tri_vec[cell_index][i].vertex[j].y;

					if (tri_vec[cell_index][i].vertex[j].y > y_max)
						y_max = tri_vec[cell_index][i].vertex[j].y;
				}
			}

		return fabsf(y_min - y_max);
	}


	float get_y_max(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float y_max = numeric_limits<float>::min();

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{

				if (tri_vec[cell_index][i].vertex[j].y > y_max)
					y_max = tri_vec[cell_index][i].vertex[j].y;
			}
		}

		return y_max;
	}



	vec3 get_centre(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = numeric_limits<float>::min();
		float y_max = numeric_limits<float>::min();
		float z_max = numeric_limits<float>::min();


			for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[cell_index][i].vertex[j].x < x_min)
						x_min = tri_vec[cell_index][i].vertex[j].x;

					if (tri_vec[cell_index][i].vertex[j].x > x_max)
						x_max = tri_vec[cell_index][i].vertex[j].x;

					if (tri_vec[cell_index][i].vertex[j].y < y_min)
						y_min = tri_vec[cell_index][i].vertex[j].y;

					if (tri_vec[cell_index][i].vertex[j].y > y_max)
						y_max = tri_vec[cell_index][i].vertex[j].y;

					if (tri_vec[cell_index][i].vertex[j].z < z_min)
						z_min = tri_vec[cell_index][i].vertex[j].z;

					if (tri_vec[cell_index][i].vertex[j].z > z_max)
						z_max = tri_vec[cell_index][i].vertex[j].z;
				}
			}


		vec3 ret(-(x_max + x_min) / 2.0f, -(y_max + y_min) / 2.0f, -(z_max + z_min) / 2.0f);

		return ret;
	}

};