#pragma once



#include "picopng.h"

#include "primitives.h"
#include "uv_camera.h"



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



	void std_strtok(const string& s, const string& regex_s, vector<string>& tokens)
	{
		tokens.clear();

		regex r(regex_s);

		sregex_token_iterator iter(s.begin(), s.end(), r, -1);
		sregex_token_iterator end;

		while (iter != end)
			tokens.push_back(*iter++);
	}


	bool read_quads_from_obj_array(string filename_base)
	{
		tri_vec.clear();
		tri_vec.resize(1);
		string filename = filename_base + ".obj";
		if (false == read_quads_from_wavefront_obj_file(filename, 0))
			return false;

		init_opengl_data();

		return true;
	}



	bool read_quads_from_wavefront_obj_file(string file_name, size_t index)
	{
		tri_vec[index].clear();

		vector<triangle> triangles;

		// Write to file.
		ifstream infile(file_name);

		if (infile.fail())
			return false;

		vector<vertex_3> verts;
		vector<uv_coord> tex_coords;
		vector<vertex_3> norms;

		string mtllib_filename_string;
		string usemtl_palette_string;

		string line;

		map<vertex_3, size_t> face_normal_counts;
		vertex_3 vtemp;



		while (getline(infile, line))
		{
			if (line == "")
				continue;

			vector<string> tokens;

			std_strtok(line, "[ \t]", tokens);

			if (tokens[0] == "mtllib")
			{
				for (size_t i = 1; i < tokens.size() - 1; i++)
				{
					mtllib_filename_string += tokens[i];
					mtllib_filename_string += ' ';
				}

				mtllib_filename_string += tokens[tokens.size() - 1];
			}
			else if (tokens[0] == "usemtl")
			{
				for (size_t i = 1; i < tokens.size() - 1; i++)
				{
					usemtl_palette_string += tokens[i];
					usemtl_palette_string += ' ';
				}

				usemtl_palette_string += tokens[tokens.size() - 1];
			}
			else if (tokens[0] == "vn")
			{
				if (tokens.size() != 4)
					continue;

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
					continue;

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
					continue;

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
					continue;

				//v / vt / vn

				quad q;
				//triangle t;

				vector<string> face_tokens;


				std_strtok(tokens[1], "[/]", face_tokens);

				if (face_tokens.size() != 3)
					continue;

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
					continue;

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
					continue;


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
					continue;


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


protected:
	void init_opengl_data(void);
};




