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

#include <utility>
using std::pair;

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

	pair<size_t, size_t> cell_location;

	float get_y_min(void)
	{
		float y_min = numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y < y_min)
						y_min = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return y_min;
	}

	float get_y_max(void)
	{
		float y_max = -numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y > y_max)
						y_max = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return y_max;
	}



	float get_x_min(void)
	{
		float x_min = numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].x < x_min)
						x_min = tri_vec[t][i].vertex[j].x;
				}
			}
		}

		return x_min;
	}

	float get_x_max(void)
	{
		float x_max = -numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].x > x_max)
						x_max = tri_vec[t][i].vertex[j].x;
				}
			}
		}

		return x_max;
	}





	float get_z_min(void)
	{
		float z_min = numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].z < z_min)
						z_min = tri_vec[t][i].vertex[j].z;
				}
			}
		}

		return z_min;
	}

	float get_z_max(void)
	{
		float z_max = -numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].z > z_max)
						z_max = tri_vec[t][i].vertex[j].z;
				}
			}
		}

		return z_max;
	}





	float get_x_extent(void)
	{
		float x_min = get_x_min();
		float x_max = get_x_max();

		return distance(x_min, x_max);
	}

	float get_z_extent(void)
	{
		float z_min = get_z_min();
		float z_max = get_z_max();

		return distance(z_min, z_max);
	}

	float get_y_extent(void)
	{
		float y_min = get_y_min();
		float y_max = get_y_max();

		return distance(y_min, y_max);
	}

	
	bool intersect_triangles(vec3 ray_origin, vec3 ray_dir, vec3& closest_intersection_point, bool abort_early)
	{
		bool global_found_intersection = false;
		bool first_assignment = true;

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{

				vec3 v0;
				v0.x = tri_vec[t][i].vertex[0].x;
				v0.y = tri_vec[t][i].vertex[0].y;
				v0.z = tri_vec[t][i].vertex[0].z;

				vec3 v1;
				v1.x = tri_vec[t][i].vertex[1].x;
				v1.y = tri_vec[t][i].vertex[1].y;
				v1.z = tri_vec[t][i].vertex[1].z;

				vec3 v2;
				v2.x = tri_vec[t][i].vertex[2].x;
				v2.y = tri_vec[t][i].vertex[2].y;
				v2.z = tri_vec[t][i].vertex[2].z;

				vec3 closest;
				bool found_intersection = RayIntersectsTriangle(ray_origin,
					ray_dir,
					v0, v1, v2,
					closest);

				if (true == found_intersection)
				{
					global_found_intersection = true;

					if (first_assignment)
					{
						closest_intersection_point = closest;
						first_assignment = false;

						if (abort_early)
							return global_found_intersection;
					}
					else
					{
						vec3 c0 = ray_origin - closest;
						vec3 c1 = ray_origin - closest_intersection_point;

						if (length(c0) < length(c1))
							closest_intersection_point = closest;
					}
				}

			}
		}

		return global_found_intersection;
	}

	bool RayIntersectsTriangle(const vec3 rayOrigin,
		const vec3 rayVector,
		const vec3 v0, const vec3 v1, const vec3 v2,
		vec3& outIntersectionPoint)
	{
		const float EPSILON = 0.0000001f;
		vec3 vertex0 = v0;// inTriangle->vertex0;
		vec3 vertex1 = v1;// inTriangle->vertex1;
		vec3 vertex2 = v2;// inTriangle->vertex2;
		vec3 edge1, edge2, h, s, q;
		float a, f, u, v;
		edge1 = vertex1 - vertex0;
		edge2 = vertex2 - vertex0;
		h = cross(rayVector, edge2);
		a = dot(edge1, h);
		if (a > -EPSILON && a < EPSILON)
			return false;    // This ray is parallel to this triangle.
		f = 1.0f / a;
		s = rayOrigin - vertex0;
		u = f * dot(s, h);
		if (u < 0.0f || u > 1.0f)
			return false;
		q = cross(s, edge1);
		v = f * dot(rayVector, q);
		if (v < 0.0f || u + v > 1.0f)
			return false;

		// At this stage we can compute t to find out where the intersection point is on the line.

		float t = f * dot(edge2, q);

		if (t > EPSILON) // ray intersection
		{
			outIntersectionPoint = rayOrigin + rayVector * t;
			return true;
		}
		else // This means that there is a line intersection but not a ray intersection.
			return false;
	}

	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	bool intersect_AABB(vec3 ray_origin, vec3 ray_dir)
	{
		float tmin = (min_location.x - ray_origin.x) / ray_dir.x;
		float tmax = (max_location.x - ray_origin.x) / ray_dir.x;

		if (tmin > tmax) swap(tmin, tmax);

		float tymin = (min_location.y - ray_origin.y) / ray_dir.y;
		float tymax = (max_location.y - ray_origin.y) / ray_dir.y;

		if (tymin > tymax) swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (min_location.z - ray_origin.z) / ray_dir.z;
		float tzmax = (max_location.z - ray_origin.z) / ray_dir.z;

		if (tzmin > tzmax) swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}




	void calc_AABB_min_max_locations(void)
	{
		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();

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

		min_location.x = x_min;
		min_location.y = y_min;
		min_location.z = z_min;

		max_location.x = x_max;
		max_location.y = y_max;
		max_location.z = z_max;
	}



	vec3 min_location, max_location;




	GLuint glow_tex = 0;
	GLuint colour_tex = 0;
	GLuint specular_tex = 0;

	~mesh(void)
	{
		if (glow_tex != 0)
			glDeleteTextures(1, &glow_tex);

		if (colour_tex != 0)
			glDeleteTextures(1, &colour_tex);

		if (specular_tex != 0)
			glDeleteTextures(1, &specular_tex);
	}

	vector<unsigned char> glow_data;
	vector<unsigned char> colour_data;
	vector<unsigned char> specular_data;


	size_t glow_data_x = 0, glow_data_y = 0;
	size_t colour_data_x = 0, colour_data_y = 0;
	size_t specular_data_x = 0, specular_data_y = 0;

	GLuint get_glow_tex_handle(void)
	{
		return glow_tex;
	}

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
	vector<float> opengl_line_vertex_data;
	vector<float> opengl_circle_vertex_data;


	vector<float> opengl_aabb_line_vertex_data;

	mat4 model_mat;


	void draw_AABB(GLuint line_program)
	{
		opengl_aabb_line_vertex_data.clear();


		opengl_aabb_line_vertex_data.push_back(max_location.x);
		opengl_aabb_line_vertex_data.push_back(min_location.y);
		opengl_aabb_line_vertex_data.push_back(min_location.z);

		opengl_aabb_line_vertex_data.push_back(max_location.x);
		opengl_aabb_line_vertex_data.push_back(min_location.y);
		opengl_aabb_line_vertex_data.push_back(max_location.z);

		//glVertex3f(max_location.x, min_location.y, min_location.z);
		//glVertex3f(max_location.x, min_location.y, max_location.z);

		opengl_aabb_line_vertex_data.push_back(min_location.x);
		opengl_aabb_line_vertex_data.push_back(max_location.y);
		opengl_aabb_line_vertex_data.push_back(min_location.z);

		opengl_aabb_line_vertex_data.push_back(min_location.x);
		opengl_aabb_line_vertex_data.push_back(max_location.y);
		opengl_aabb_line_vertex_data.push_back(max_location.z);

		//glVertex3f(min_location.x, max_location.y, min_location.z);
		//glVertex3f(min_location.x, max_location.y, max_location.z);


		opengl_aabb_line_vertex_data.push_back(max_location.x);
		opengl_aabb_line_vertex_data.push_back(max_location.y);
		opengl_aabb_line_vertex_data.push_back(min_location.z);

		opengl_aabb_line_vertex_data.push_back(max_location.x);
		opengl_aabb_line_vertex_data.push_back(max_location.y);
		opengl_aabb_line_vertex_data.push_back(max_location.z);



		//glVertex3f(max_location.x, max_location.y, min_location.z);
		//glVertex3f(max_location.x, max_location.y, max_location.z);


		opengl_aabb_line_vertex_data.push_back(min_location.x);
		opengl_aabb_line_vertex_data.push_back(min_location.y);
		opengl_aabb_line_vertex_data.push_back(min_location.z);

		opengl_aabb_line_vertex_data.push_back(min_location.x);
		opengl_aabb_line_vertex_data.push_back(min_location.y);
		opengl_aabb_line_vertex_data.push_back(max_location.z);


		//glVertex3f(min_location.x, min_location.y, min_location.z);
		//glVertex3f(min_location.x, min_location.y, max_location.z);


		GLuint components_per_vertex = 3;
		GLuint components_per_position = 3;

		GLuint axis_buffer;

		glGenBuffers(1, &axis_buffer);

		GLuint num_vertices = static_cast<GLuint>(opengl_aabb_line_vertex_data.size()) / components_per_vertex;

		glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
		glBufferData(GL_ARRAY_BUFFER, opengl_aabb_line_vertex_data.size() * sizeof(GLfloat), &opengl_aabb_line_vertex_data[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(glGetAttribLocation(line_program, "position"));
		glVertexAttribPointer(glGetAttribLocation(line_program, "position"),
			components_per_position,
			GL_FLOAT,
			GL_FALSE,
			components_per_vertex * sizeof(GLfloat),
			NULL);

		glDrawArrays(GL_LINES, 0, num_vertices);

		glDeleteBuffers(1, &axis_buffer);
	}




	void draw_lines(GLuint line_program)
	{
		//glUseProgram(line_program);

		GLuint components_per_vertex = 3;
		GLuint components_per_position = 3;

		GLuint axis_buffer;

		glGenBuffers(1, &axis_buffer);

		GLuint num_vertices = static_cast<GLuint>(opengl_line_vertex_data.size()) / components_per_vertex;

		glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
		glBufferData(GL_ARRAY_BUFFER, opengl_line_vertex_data.size() * sizeof(GLfloat), &opengl_line_vertex_data[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(glGetAttribLocation(line_program, "position"));
		glVertexAttribPointer(glGetAttribLocation(line_program, "position"),
			components_per_position,
			GL_FLOAT,
			GL_FALSE,
			components_per_vertex * sizeof(GLfloat),
			NULL);

		glDrawArrays(GL_LINES, 0, num_vertices);

		glDeleteBuffers(1, &axis_buffer);
	}



	void draw_circles(GLuint circle_program)
	{
		glUseProgram(circle_program);

		GLuint components_per_vertex = 3;
		GLuint components_per_position = 3;

		GLuint axis_buffer;

		glGenBuffers(1, &axis_buffer);

		GLuint num_vertices = static_cast<GLuint>(opengl_circle_vertex_data.size()) / components_per_vertex;

		glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
		glBufferData(GL_ARRAY_BUFFER, opengl_circle_vertex_data.size() * sizeof(GLfloat), &opengl_circle_vertex_data[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(glGetAttribLocation(circle_program, "position"));
		glVertexAttribPointer(glGetAttribLocation(circle_program, "position"),
			components_per_position,
			GL_FLOAT,
			GL_FALSE,
			components_per_vertex * sizeof(GLfloat),
			NULL);

		glDrawArrays(GL_POINTS, 0, num_vertices);

		glDeleteBuffers(1, &axis_buffer);
	}




	void centre_mesh_on_xyz(void)
	{
		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();

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


	void process_line(bool cull_back_faces, string line,
		vector<triangle>& triangles,
		vector<vertex_3>& verts,
		vector<uv_coord>& tex_coords,
		vector<vertex_3>& norms,
		map<vertex_3, size_t>& face_normal_counts)
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



	void draw(GLint render_shader_program);

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

	bool read_quads_from_vox_file(string file_name, string glow_tex_file_name, string colour_tex_file_name, string specular_tex_file_name, bool cull_faces)
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

		//cout << scene->num_models << endl;



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

					// Is this correct?
					const float uv_index = float(colour_index - 1) / 255.0f;

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


		if (glow_tex != 0)
			glDeleteTextures(1, &glow_tex);

		if (colour_tex != 0)
			glDeleteTextures(1, &colour_tex);

		if (specular_tex != 0)
			glDeleteTextures(1, &specular_tex);



		std::vector<unsigned char> buffer;
		loadFile(buffer, colour_tex_file_name.c_str());
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


		//		std::vector<unsigned char> buffer;
		loadFile(buffer, glow_tex_file_name.c_str());
		//			unsigned long w, h;
		decodePNG(glow_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		glow_data_x = w;
		glow_data_y = h;

		glGenTextures(1, &glow_tex);
		glBindTexture(GL_TEXTURE_2D, glow_tex);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);




		//		std::vector<unsigned char> buffer;
		loadFile(buffer, specular_tex_file_name.c_str());
		//			unsigned long w, h;
		decodePNG(specular_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		specular_data_x = w;
		specular_data_y = h;

		glGenTextures(1, &specular_tex);
		glBindTexture(GL_TEXTURE_2D, specular_tex);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);




		init_opengl_data();

		calc_AABB_min_max_locations();

		return true;
	}










protected:
	void init_opengl_data(void);
};



class neighbour_data
{
public:
	size_t x = 0;
	size_t y = 0;
};



// Inherit from class A
class binned_mesh : public mesh
{
public:


	const size_t level_size = 128;
	const size_t grid_cell_size = 16;
	const size_t num_cells_wide = level_size / grid_cell_size;

	float cached_y_min = numeric_limits<float>::max();
	float cached_y_max = -numeric_limits<float>::max();

	map<size_t, float> cached_y_max_cells;
	float cached_y_min_extent = numeric_limits<float>::max();
	float cached_y_max_extent = -numeric_limits<float>::max();


	vector<vec3> bin_min_locations;
	vector<vec3> bin_max_locations;

	void calc_bin_AABB_min_max_locations(void)
	{
		bin_min_locations.clear();
		bin_min_locations.resize(num_cells_wide*num_cells_wide);

		bin_max_locations.clear();
		bin_max_locations.resize(num_cells_wide * num_cells_wide);

		for (size_t cell_x = 0; cell_x < num_cells_wide; cell_x++)
		{
			for (size_t cell_y = 0; cell_y < num_cells_wide; cell_y++)
			{
				size_t cell_index = cell_y * num_cells_wide + cell_x;

				float x_min = numeric_limits<float>::max();
				float y_min = numeric_limits<float>::max();
				float z_min = numeric_limits<float>::max();
				float x_max = -numeric_limits<float>::max();
				float y_max = -numeric_limits<float>::max();
				float z_max = -numeric_limits<float>::max();


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

					bin_min_locations[cell_index].x = x_min;
					bin_min_locations[cell_index].y = y_min;
					bin_min_locations[cell_index].z = z_min;

					bin_max_locations[cell_index].x = x_max;
					bin_max_locations[cell_index].y = y_max;
					bin_max_locations[cell_index].z = z_max;





			}
		}




		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();

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

		min_location.x = x_min;
		min_location.y = y_min;
		min_location.z = z_min;

		max_location.x = x_max;
		max_location.y = y_max;
		max_location.z = z_max;
	}

	//float get_neighbour_average_height(size_t x, size_t y)
	//{

	//	long long signed int x_plus_start = x + 1;
	//	long long signed int x_minus_start = x - 1;

	//	if (x_minus_start < 0)
	//		x_minus_start = 0;

	//	if (x_plus_start > num_cells_wide - 1)
	//		x_plus_start = num_cells_wide - 1;

	//	long long signed int y_plus_start = y + 1;
	//	long long signed int y_minus_start = y - 1;

	//	if (y_minus_start < 0)
	//		y_minus_start = 0;

	//	if (y_plus_start > num_cells_wide - 1)
	//		y_plus_start = num_cells_wide - 1;



	//	float average_height = 0;
	//	float current_height = distance(get_y_min(), get_y_max(x, y));

	//	//				average_height += current_height;
	//	size_t count = 0;

	//	for (long long unsigned int i = x_minus_start; i <= x_plus_start; i++)
	//	{
	//		for (long long unsigned int j = y_minus_start; j <= y_plus_start; j++)
	//		{
	//			size_t cell_index = i * num_cells_wide + j;

	//			float neighbour_height = distance(get_y_min(), get_y_max(i, j));

	//			if (neighbour_height <= current_height)
	//			{
	//				average_height += neighbour_height;

	//				count++;
	//			}
	//		}
	//	}

	//	average_height /= count;

	//	return average_height;
	//}	


	void get_all_neighbour_indices(size_t x, size_t y, vector<neighbour_data>& ret)
	{
		ret.clear();

		long long signed int x_plus_start = x + 1;
		long long signed int x_minus_start = x - 1;

		if (x_minus_start < 0)
			x_minus_start = 0;

		if (x_plus_start > num_cells_wide - 1)
			x_plus_start = num_cells_wide - 1;

		long long signed int y_plus_start = y + 1;
		long long signed int y_minus_start = y - 1;

		if (y_minus_start < 0)
			y_minus_start = 0;

		if (y_plus_start > num_cells_wide - 1)
			y_plus_start = num_cells_wide - 1;


		for (long long unsigned int i = x_minus_start; i <= x_plus_start; i++)
		{
			for (long long unsigned int j = y_minus_start; j <= y_plus_start; j++)
			{
				if (i == x && j == y)
					continue;

				float neighbour_height = distance(get_y_min(), get_y_max(i, j));

				neighbour_data nd;
				nd.x = i;
				nd.y = j;

				ret.push_back(nd);
			}
		}
	}







	//void mirror_y(size_t cell_x, size_t cell_y)
	//{
	//	vec3 centre = get_centre(cell_x, cell_y);

	//	size_t cell_index = cell_y * num_cells_wide + cell_x;

	//	for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
	//	{
	//		for (size_t j = 0; j < 3; j++)
	//		{
	//			tri_vec[cell_index][i].vertex[j].x -= centre.x;
	//			tri_vec[cell_index][i].vertex[j].y -= centre.y;
	//			tri_vec[cell_index][i].vertex[j].z -= centre.z;
	//		}
	//	}

	//	for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
	//	{
	//		for (size_t j = 0; j < 3; j++)
	//		{
	//			tri_vec[cell_index][i].vertex[j].y = -tri_vec[cell_index][i].vertex[j].y;
	//		}
	//	}

	//	for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
	//	{
	//		for (size_t j = 0; j < 3; j++)
	//		{
	//			tri_vec[cell_index][i].vertex[j].x += centre.x;
	//			tri_vec[cell_index][i].vertex[j].y += centre.y;
	//			tri_vec[cell_index][i].vertex[j].z += centre.z;
	//		}
	//	}

	//	init_opengl_data();
	//}


	bool read_quads_from_vox_file(string file_name, string glow_tex_file_name, string colour_tex_file_name, string specular_tex_file_name, bool cull_faces)
	{
		tri_vec.clear();
		opengl_vertex_data.clear();

		cached_y_min = numeric_limits<float>::max();
		cached_y_max = -numeric_limits<float>::max();

		cached_y_max_cells.clear();
		cached_y_min_extent = numeric_limits<float>::max();
		cached_y_max_extent = -numeric_limits<float>::max();

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

		//cout << scene->num_models << endl;

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

					const float uv_index = float(colour_index - 1) / 256.0f;

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
							if (y != 0)
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
							if (x != 0)
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

		if (glow_tex != 0)
			glDeleteTextures(1, &glow_tex);

		if (colour_tex != 0)
			glDeleteTextures(1, &colour_tex);

		if (specular_tex != 0)
			glDeleteTextures(1, &specular_tex);


		std::vector<unsigned char> buffer;
		loadFile(buffer, colour_tex_file_name.c_str());
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


		loadFile(buffer, glow_tex_file_name.c_str());
		//			unsigned long w, h;
		decodePNG(glow_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		glow_data_x = w;
		glow_data_y = h;

		glGenTextures(1, &glow_tex);
		glBindTexture(GL_TEXTURE_2D, glow_tex);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



		//		std::vector<unsigned char> buffer;
		loadFile(buffer, specular_tex_file_name.c_str());
		//			unsigned long w, h;
		decodePNG(specular_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		specular_data_x = w;
		specular_data_y = h;

		glGenTextures(1, &specular_tex);
		glBindTexture(GL_TEXTURE_2D, specular_tex);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		calc_AABB_min_max_locations();

		init_opengl_data();

		return true;
	}




	void draw(GLint render_shader_program,
		size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		glUseProgram(render_shader_program);

		const GLuint components_per_vertex = 8;
		const GLuint components_per_position = 3;
		const GLuint components_per_normal = 3;
		const GLuint components_per_texcoord = 2;

		if (glow_tex == 0)
		{
			glGenTextures(1, &glow_tex);
			glBindTexture(GL_TEXTURE_2D, glow_tex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		if (colour_tex == 0)
		{
			glGenTextures(1, &colour_tex);
			glBindTexture(GL_TEXTURE_2D, colour_tex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		if (specular_tex == 0)
		{
			glGenTextures(1, &specular_tex);
			glBindTexture(GL_TEXTURE_2D, specular_tex);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glBindTexture(GL_TEXTURE_2D, glow_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(glow_data_x), static_cast<GLsizei>(glow_data_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, &glow_data[0]);


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






	float get_y_min(void)
	{
		if (cached_y_min == numeric_limits<float>::max())
		{
			for (size_t t = 0; t < tri_vec.size(); t++)
			{
				for (size_t i = 0; i < tri_vec[t].size(); i++)
				{
					for (size_t j = 0; j < 3; j++)
					{
						if (tri_vec[t][i].vertex[j].y < cached_y_min)
							cached_y_min = tri_vec[t][i].vertex[j].y;
					}
				}
			}
		}

		return cached_y_min;
	}

	float get_y_max(void)
	{
		if (cached_y_max == -numeric_limits<float>::max())
		{
			for (size_t t = 0; t < tri_vec.size(); t++)
			{
				for (size_t i = 0; i < tri_vec[t].size(); i++)
				{
					for (size_t j = 0; j < 3; j++)
					{
						if (tri_vec[t][i].vertex[j].y > cached_y_max)
							cached_y_max = tri_vec[t][i].vertex[j].y;
					}
				}
			}
		}

		return cached_y_max;
	}



	float get_x_min(void)
	{
		float x_min = numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y < x_min)
						x_min = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return x_min;
	}

	float get_x_max(void)
	{
		float x_max = -numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y > x_max)
						x_max = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return x_max;
	}





	float get_z_min(void)
	{
		float z_min = numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y < z_min)
						z_min = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return z_min;
	}

	float get_z_max(void)
	{
		float z_max = -numeric_limits<float>::max();

		for (size_t t = 0; t < tri_vec.size(); t++)
		{
			for (size_t i = 0; i < tri_vec[t].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[t][i].vertex[j].y > z_max)
						z_max = tri_vec[t][i].vertex[j].y;
				}
			}
		}

		return z_max;
	}





	float get_x_extent(void)
	{
		float x_min = get_x_min();
		float x_max = get_x_max();

		return distance(x_min, x_max);
	}

	float get_z_extent(void)
	{
		float y_min = get_z_min();
		float y_max = get_z_max();

		return distance(y_min, y_max);
	}

	float get_y_extent(void)
	{
		float y_min = get_y_min();
		float y_max = get_y_max();

		return distance(y_min, y_max);
	}

	float get_y_extent(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float y_min = numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();

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

		return distance(y_min, y_max);
	}

	float get_x_extent(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float x_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (tri_vec[cell_index][i].vertex[j].y < x_min)
					x_min = tri_vec[cell_index][i].vertex[j].y;

				if (tri_vec[cell_index][i].vertex[j].y > x_max)
					x_max = tri_vec[cell_index][i].vertex[j].y;
			}
		}

		return distance(x_min, x_max);
	}

	float get_z_extent(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float z_min = numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (tri_vec[cell_index][i].vertex[j].y < z_min)
					z_min = tri_vec[cell_index][i].vertex[j].y;

				if (tri_vec[cell_index][i].vertex[j].y > z_max)
					z_max = tri_vec[cell_index][i].vertex[j].y;
			}
		}

		return distance(z_min, z_max);
	}


	float get_y_max(size_t cell_x, size_t cell_y)
	{
		float ret = 0;

		size_t cell_index = cell_y * num_cells_wide + cell_x;

		map<size_t, float>::const_iterator ci = cached_y_max_cells.find(cell_index);

		// If not in cache
		if (ci == cached_y_max_cells.end())
		{
			ret = -numeric_limits<float>::max();

			for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (tri_vec[cell_index][i].vertex[j].y > ret)
						ret = tri_vec[cell_index][i].vertex[j].y;
				}
			}

			cached_y_max_cells[cell_index] = ret;
		}
		else
		{
			ret = ci->second;
		}

		return ret;
	}




	float get_y_plane_min(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float y_min = numeric_limits<float>::max();

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (tri_vec[cell_index][i].vertex[j].y < y_min)
				{
					vec3 a = normalize(vec3(tri_vec[cell_index][i].vertex[j].nx, tri_vec[cell_index][i].vertex[j].ny, tri_vec[cell_index][i].vertex[j].nz));
					vec3 b(0, 1, 0);

					//cout << dot(a, b) << endl;

					if (dot(a, b) > 0.95)
						y_min = tri_vec[cell_index][i].vertex[j].y;
				}
			}
		}

		return y_min;
	}


	float get_y_plane_max(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float y_max = -numeric_limits<float>::max();

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (tri_vec[cell_index][i].vertex[j].y > y_max)
				{
					vec3 a = normalize(vec3(tri_vec[cell_index][i].vertex[j].nx, tri_vec[cell_index][i].vertex[j].ny, tri_vec[cell_index][i].vertex[j].nz));
					vec3 b(0, 1, 0);

					//cout << dot(a, b) << endl;

					if (dot(a, b) > 0.95)
						y_max = tri_vec[cell_index][i].vertex[j].y;
				}
			}
		}

		return y_max;
	}






	float get_y_min(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float y_min = numeric_limits<float>::max();

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (tri_vec[cell_index][i].vertex[j].y < y_min)
					y_min = tri_vec[cell_index][i].vertex[j].y;
			}
		}

		return y_min;
	}

	vec3 get_y_plane_centre(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();


		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			
				vec3 a = normalize(vec3(tri_vec[cell_index][i].vertex[0].nx, tri_vec[cell_index][i].vertex[0].ny, tri_vec[cell_index][i].vertex[0].nz));
				vec3 b(0, 1, 0);

				//cout << dot(a, b) << endl;

				if (dot(a, b) < 0.95)
					continue;// y_max = tri_vec[cell_index][i].vertex[j].y;
			



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


		vec3 ret((x_max + x_min) / 2.0f, (y_max + y_min) / 2.0f, (z_max + z_min) / 2.0f);

		return ret;
	}


	vec3 get_centre(size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();

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


		vec3 ret((x_max + x_min) / 2.0f, (y_max + y_min) / 2.0f, (z_max + z_min) / 2.0f);

		return ret;
	}



	void calc_AABB_min_max_locations(void)
	{
		float x_min = numeric_limits<float>::max();
		float y_min = numeric_limits<float>::max();
		float z_min = numeric_limits<float>::max();
		float x_max = -numeric_limits<float>::max();
		float y_max = -numeric_limits<float>::max();
		float z_max = -numeric_limits<float>::max();

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

		min_location.x = x_min;
		min_location.y = y_min;
		min_location.z = z_min;

		max_location.x = x_max;
		max_location.y = y_max;
		max_location.z = z_max;

		calc_bin_AABB_min_max_locations();
	}

	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	bool intersect_bin_AABB(vec3 ray_origin, vec3 ray_dir, size_t cell_x, size_t cell_y)
	{
		size_t cell_index = cell_y * num_cells_wide + cell_x;

		float tmin = (bin_min_locations[cell_index].x - ray_origin.x) / ray_dir.x;
		float tmax = (bin_max_locations[cell_index].x - ray_origin.x) / ray_dir.x;

		if (tmin > tmax) swap(tmin, tmax);

		float tymin = (bin_min_locations[cell_index].y - ray_origin.y) / ray_dir.y;
		float tymax = (bin_max_locations[cell_index].y - ray_origin.y) / ray_dir.y;

		if (tymin > tymax) swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (bin_min_locations[cell_index].z - ray_origin.z) / ray_dir.z;
		float tzmax = (bin_max_locations[cell_index].z - ray_origin.z) / ray_dir.z;

		if (tzmin > tzmax) swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}


	bool intersect_triangles(vec3 ray_origin, vec3 ray_dir, vec3& closest_intersection_point, size_t cell_x, size_t cell_y)
	{
		if (false == intersect_bin_AABB(ray_origin, ray_dir, cell_x, cell_y))
			return false;

		bool global_found_intersection = false;
		bool first_assignment = true;

		size_t cell_index = cell_y * num_cells_wide + cell_x;

		for (size_t i = 0; i < tri_vec[cell_index].size(); i++)
		{
			vec3 v0;
			v0.x = tri_vec[cell_index][i].vertex[0].x;
			v0.y = tri_vec[cell_index][i].vertex[0].y;
			v0.z = tri_vec[cell_index][i].vertex[0].z;

			vec3 v1;
			v1.x = tri_vec[cell_index][i].vertex[1].x;
			v1.y = tri_vec[cell_index][i].vertex[1].y;
			v1.z = tri_vec[cell_index][i].vertex[1].z;

			vec3 v2;
			v2.x = tri_vec[cell_index][i].vertex[2].x;
			v2.y = tri_vec[cell_index][i].vertex[2].y;
			v2.z = tri_vec[cell_index][i].vertex[2].z;

			vec3 closest;
			bool found_intersection = RayIntersectsTriangle(ray_origin,
				ray_dir,
				v0, v1, v2,
				closest);

			if (true == found_intersection)
			{
				global_found_intersection = true;

				if (first_assignment)
				{
					closest_intersection_point = closest;
					first_assignment = false;

					//if (abort_early)
					//	return global_found_intersection;
				}
				else
				{
					vec3 c0 = ray_origin - closest;
					vec3 c1 = ray_origin - closest_intersection_point;

					if (length(c0) < length(c1))
						closest_intersection_point = closest;
				}
			}
		}


		return global_found_intersection;
	}



};