#pragma once



#include "picopng.h"

#include "primitives.h"
#include "uv_camera.h"


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



	void centre_mesh_on_xz(void)
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
					tri_vec[t][i].vertex[j].y += 0;// -(y_max + y_min) / 2.0f;
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

	bool read_quads_from_obj_array(string filename_base)
	{
		tri_vec.clear();

		vector<string> filenames = get_masked_paths(filename_base);

		if (filenames.size() == 0)
			return false;

		tri_vec.resize(filenames.size());

		for (size_t i = 0; i < filenames.size(); i++)
		{
			if (false == read_quads_from_wavefront_obj_file(filenames[i], i))
				return false;
		}

		centre_mesh_on_xz();


		init_opengl_data();

		return true;
	}

	void process_line(string line, 
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

	/*
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

		string line;

		map<vertex_3, size_t> face_normal_counts;

		while (getline(infile, line))
		{
			if (line == "")
				continue;

			process_line(line, triangles, verts, tex_coords, norms, face_normal_counts);
		}

		tri_vec[index] = triangles;

		return true;
	}
	*/



	bool read_quads_from_wavefront_obj_file(string file_name, size_t index)
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
				process_line(temp_token, triangles, verts, tex_coords, norms, face_normal_counts);

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


protected:
	void init_opengl_data(void);
};




