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
	vector<triangle> triangles;

	vector<float> opengl_vertex_data;

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



	void draw_basis()
	{
		draw_dir();
		draw_tangent();
		draw_left();

	}

	void draw_left(void)
	{
		vec3 centre = model_mat * vec4((max_location + min_location) / 2.0f, 1.0f);
		vec3 left = geodesic_left * 0.5f;

		glBegin(GL_LINES);

		glVertex3f(centre.x, centre.y, centre.z);
		glVertex3f(centre.x + left.x, centre.y + left.y, centre.z + left.z);

		glEnd();
	}
	void draw_dir(void)
	{
		vec3 centre = model_mat * vec4((max_location + min_location) / 2.0f, 1.0f);
		vec3 dir = geodesic_dir * 0.5f;

		glBegin(GL_LINES);

		glVertex3f(centre.x, centre.y, centre.z);
		glVertex3f(centre.x + dir.x, centre.y + dir.y, centre.z + dir.z);

		glEnd();
	}

	void draw_tangent(void)
	{
		vec3 centre = model_mat * vec4((max_location + min_location) / 2.0f, 1.0f);
		vec3 tangent = geodesic_tangent * 0.5f;

		glBegin(GL_LINES);

		glVertex3f(centre.x, centre.y, centre.z);
		glVertex3f(centre.x + tangent.x, centre.y + tangent.y, centre.z + tangent.z);

		glEnd();
	}

	bool read_triangles_from_binary_stereo_lithography_file(const char* const file_name, bool randomize_tris);

	bool read_triangles_from_wavefront_obj_file(const char* const file_name)
	{
		triangles.clear();

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
				if (tokens.size() != 4)
					continue;

				//v / vt / vn

				triangle t;

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

				t.vertex[0].x = verts[v_index].x;
				t.vertex[0].y = verts[v_index].y;
				t.vertex[0].z = verts[v_index].z;
				t.vertex[0].u = tex_coords[vt_index].u;
				t.vertex[0].v = tex_coords[vt_index].v;
				t.vertex[0].nx = norms[vn_index].x;
				t.vertex[0].ny = norms[vn_index].y;
				t.vertex[0].nz = norms[vn_index].z;

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

				t.vertex[1].x = verts[v_index].x;
				t.vertex[1].y = verts[v_index].y;
				t.vertex[1].z = verts[v_index].z;
				t.vertex[1].u = tex_coords[vt_index].u;
				t.vertex[1].v = tex_coords[vt_index].v;
				t.vertex[1].nx = norms[vn_index].x;
				t.vertex[1].ny = norms[vn_index].y;
				t.vertex[1].nz = norms[vn_index].z;

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

				t.vertex[2].x = verts[v_index].x;
				t.vertex[2].y = verts[v_index].y;
				t.vertex[2].z = verts[v_index].z;
				t.vertex[2].u = tex_coords[vt_index].u;
				t.vertex[2].v = tex_coords[vt_index].v;
				t.vertex[2].nx = norms[vn_index].x;
				t.vertex[2].ny = norms[vn_index].y;
				t.vertex[2].nz = norms[vn_index].z;

				vtemp = vertex_3(norms[vn_index].x, norms[vn_index].y, norms[vn_index].z);
				face_normal_counts[vtemp]++;





				triangles.push_back(t);
			}
		}

		for (map<vertex_3, size_t>::const_iterator ci = face_normal_counts.begin(); ci != face_normal_counts.end(); ci++)
		{
			cout << ci->first.x << " " << ci->first.y << " " << ci->first.z << endl;
			cout << ci->second << endl << endl;
		}


		init_opengl_data();

		return true;
	}



	void draw(GLint render_shader_program, int win_x, int win_y);

	void draw_AABB(void);

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


	bool intersect_triangles(vec3 ray_origin, vec3 ray_dir, vec3& closest_intersection_point)
	{
		bool global_found_intersection = false;
		bool first_assignment = true;

		for (size_t i = 0; i < triangles.size(); i++)
		{
			vec3 v0;
			v0.x = triangles[i].vertex[0].x;
			v0.y = triangles[i].vertex[0].y;
			v0.z = triangles[i].vertex[0].z;

			vec3 v1;
			v1.x = triangles[i].vertex[1].x;
			v1.y = triangles[i].vertex[1].y;
			v1.z = triangles[i].vertex[1].z;

			vec3 v2;
			v2.x = triangles[i].vertex[2].x;
			v2.y = triangles[i].vertex[2].y;
			v2.z = triangles[i].vertex[2].z;

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


protected:
void get_vertices_and_normals_from_triangles();
	void init_opengl_data(void);
};




