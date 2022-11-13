#include "mesh.h"



void mesh::init_opengl_data(void)
{
	opengl_vertex_data.clear();
	opengl_vertex_data.resize(tri_vec.size());
	opengl_line_vertex_data.clear();

	// triangles
	for (size_t v = 0; v < tri_vec.size(); v++)
	{
		map<ordered_edge, vector<size_t>> edge_neighbour_data;

		for (size_t i = 0; i < tri_vec[v].size(); i++)
		{
			const ordered_edge edge0(tri_vec[v][i].vertex[0], tri_vec[v][i].vertex[1]);
			const ordered_edge edge1(tri_vec[v][i].vertex[1], tri_vec[v][i].vertex[2]);
			const ordered_edge edge2(tri_vec[v][i].vertex[2], tri_vec[v][i].vertex[0]);

			edge_neighbour_data[edge0].push_back(i);
			edge_neighbour_data[edge1].push_back(i);
			edge_neighbour_data[edge2].push_back(i);
		}


		for (size_t i = 0; i < tri_vec[v].size(); i++)
		{
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].x);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].y);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].z);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nx);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].ny);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nz);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].u);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].v);

			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].x);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].y);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].z);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nx);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].ny);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nz);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].u);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].v);

			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].x);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].y);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].z);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nx);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].ny);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nz);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].u);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].v);

			// add adjacent triangle data here
			// only include the 3 vertices that do not belong to the centre triangle






			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].x);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].y);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].z);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nx);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].ny);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nz);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].u);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].v);

			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].x);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].y);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].z);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nx);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].ny);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nz);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].u);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].v);

			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].x);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].y);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].z);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nx);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].ny);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nz);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].u);
			//opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].v);


			

	

			const ordered_edge edge0(tri_vec[v][i].vertex[0], tri_vec[v][i].vertex[1]);
			size_t neighbour0_index = 0;
			vector<size_t> edge_sharers0 = edge_neighbour_data[edge0];
			vertex_3 far_vertex0;

			if (edge_sharers0.size() == 2)
			for (size_t j = 0; j < edge_sharers0.size(); j++)
			{
				if (edge_sharers0[j] != i)
				{
					neighbour0_index = edge_sharers0[j];

					vertex_3 neighbour_v0 = tri_vec[v][neighbour0_index].vertex[0];
					vertex_3 neighbour_v1 = tri_vec[v][neighbour0_index].vertex[1];
					vertex_3 neighbour_v2 = tri_vec[v][neighbour0_index].vertex[2];

					if (neighbour_v0 == edge0.v0)
					{
						if (neighbour_v1 == edge0.v1)
							far_vertex0 = tri_vec[v][neighbour0_index].vertex[2];
						else
							far_vertex0 = tri_vec[v][neighbour0_index].vertex[1];
					}
					else
					{
						if (neighbour_v1 == edge0.v1)
							far_vertex0 = tri_vec[v][neighbour0_index].vertex[2];
						else
							far_vertex0 = tri_vec[v][neighbour0_index].vertex[0];
					}
				}
			}


			//if (far_vertex0.x != 0 && far_vertex0.y != 0 && far_vertex0.z != 0)
			//{
			//	opengl_vertex_data[v].push_back(far_vertex0.x);
			//	opengl_vertex_data[v].push_back(far_vertex0.y);
			//	opengl_vertex_data[v].push_back(far_vertex0.z);
			//	opengl_vertex_data[v].push_back(far_vertex0.nx);
			//	opengl_vertex_data[v].push_back(far_vertex0.ny);
			//	opengl_vertex_data[v].push_back(far_vertex0.nz);
			//	opengl_vertex_data[v].push_back(far_vertex0.u);
			//	opengl_vertex_data[v].push_back(far_vertex0.v);
			//}


	














		

				const ordered_edge edge1(tri_vec[v][i].vertex[1], tri_vec[v][i].vertex[2]);
				size_t neighbour1_index = 0;
				vector<size_t> edge_sharers1 = edge_neighbour_data[edge1];
				vertex_3 far_vertex1;
				if (edge_sharers1.size() == 2)
				for (size_t j = 0; j < edge_sharers1.size(); j++)
				{
					if (edge_sharers1[j] != i)
					{
						neighbour1_index = edge_sharers1[j];

						vertex_3 neighbour_v0 = tri_vec[v][neighbour1_index].vertex[0];
						vertex_3 neighbour_v1 = tri_vec[v][neighbour1_index].vertex[1];
						vertex_3 neighbour_v2 = tri_vec[v][neighbour1_index].vertex[2];

						if (neighbour_v0 == edge1.v0)
						{
							if (neighbour_v1 == edge1.v1)
								far_vertex1 = tri_vec[v][neighbour1_index].vertex[2];
							else
								far_vertex1 = tri_vec[v][neighbour1_index].vertex[1];
						}
						else
						{
							if (neighbour_v1 == edge1.v1)
								far_vertex1 = tri_vec[v][neighbour1_index].vertex[2];
							else
								far_vertex1 = tri_vec[v][neighbour1_index].vertex[0];
						}
					}
				}

	/*			if (far_vertex1.x != 0 && far_vertex1.y != 0 && far_vertex1.z != 0)
				{
					opengl_vertex_data[v].push_back(far_vertex1.x);
					opengl_vertex_data[v].push_back(far_vertex1.y);
					opengl_vertex_data[v].push_back(far_vertex1.z);
					opengl_vertex_data[v].push_back(far_vertex1.nx);
					opengl_vertex_data[v].push_back(far_vertex1.ny);
					opengl_vertex_data[v].push_back(far_vertex1.nz);
					opengl_vertex_data[v].push_back(far_vertex1.u);
					opengl_vertex_data[v].push_back(far_vertex1.v);
				}*/


		








			

				const ordered_edge edge2(tri_vec[v][i].vertex[2], tri_vec[v][i].vertex[0]);
				size_t neighbour2_index = 0;
				vector<size_t> edge_sharers2 = edge_neighbour_data[edge2];
				vertex_3 far_vertex2;

				if(edge_sharers2.size() == 2)
				for (size_t j = 0; j < edge_sharers2.size(); j++)
				{
					if (edge_sharers2[j] != i)
					{
						neighbour2_index = edge_sharers2[j];

						vertex_3 neighbour_v0 = tri_vec[v][neighbour2_index].vertex[0];
						vertex_3 neighbour_v1 = tri_vec[v][neighbour2_index].vertex[1];
						vertex_3 neighbour_v2 = tri_vec[v][neighbour2_index].vertex[2];

						if (neighbour_v0 == edge2.v0)
						{
							if (neighbour_v1 == edge2.v1)
								far_vertex2 = tri_vec[v][neighbour2_index].vertex[2];
							else
								far_vertex2 = tri_vec[v][neighbour2_index].vertex[1];
						}
						else
						{
							if (neighbour_v1 == edge2.v1)
								far_vertex2 = tri_vec[v][neighbour2_index].vertex[2];
							else
								far_vertex2 = tri_vec[v][neighbour2_index].vertex[0];
						}
					}
				}

				//if (far_vertex2.x != 0 && far_vertex2.y != 0 && far_vertex2.z != 0)
				//{
				//	opengl_vertex_data[v].push_back(far_vertex2.x);
				//	opengl_vertex_data[v].push_back(far_vertex2.y);
				//	opengl_vertex_data[v].push_back(far_vertex2.z);
				//	opengl_vertex_data[v].push_back(far_vertex2.nx);
				//	opengl_vertex_data[v].push_back(far_vertex2.ny);
				//	opengl_vertex_data[v].push_back(far_vertex2.nz);
				//	opengl_vertex_data[v].push_back(far_vertex2.u);
				//	opengl_vertex_data[v].push_back(far_vertex2.v);
				//}
		



			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].x);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].y);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].z);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nx);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].ny);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nz);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].u);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].v);

			opengl_vertex_data[v].push_back(far_vertex0.x);
			opengl_vertex_data[v].push_back(far_vertex0.y);
			opengl_vertex_data[v].push_back(far_vertex0.z);
			opengl_vertex_data[v].push_back(far_vertex0.nx);
			opengl_vertex_data[v].push_back(far_vertex0.ny);
			opengl_vertex_data[v].push_back(far_vertex0.nz);
			opengl_vertex_data[v].push_back(far_vertex0.u);
			opengl_vertex_data[v].push_back(far_vertex0.v);

			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].x);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].y);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].z);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nx);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].ny);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nz);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].u);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].v);

			opengl_vertex_data[v].push_back(far_vertex1.x);
			opengl_vertex_data[v].push_back(far_vertex1.y);
			opengl_vertex_data[v].push_back(far_vertex1.z);
			opengl_vertex_data[v].push_back(far_vertex1.nx);
			opengl_vertex_data[v].push_back(far_vertex1.ny);
			opengl_vertex_data[v].push_back(far_vertex1.nz);
			opengl_vertex_data[v].push_back(far_vertex1.u);
			opengl_vertex_data[v].push_back(far_vertex1.v);

			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].x);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].y);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].z);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nx);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].ny);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nz);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].u);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].v);

			opengl_vertex_data[v].push_back(far_vertex2.x);
			opengl_vertex_data[v].push_back(far_vertex2.y);
			opengl_vertex_data[v].push_back(far_vertex2.z);
			opengl_vertex_data[v].push_back(far_vertex2.nx);
			opengl_vertex_data[v].push_back(far_vertex2.ny);
			opengl_vertex_data[v].push_back(far_vertex2.nz);
			opengl_vertex_data[v].push_back(far_vertex2.u);
			opengl_vertex_data[v].push_back(far_vertex2.v);




		}
	}


// lines
	for (size_t i = 0; i < tri_vec.size(); i++)
	{


		for (size_t t = 0; t < tri_vec[i].size(); t++)
		{
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[0].x);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[0].y);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[0].z);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[1].x);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[1].y);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[1].z);

			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[1].x);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[1].y);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[1].z);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[2].x);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[2].y);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[2].z);

			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[2].x);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[2].y);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[2].z);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[0].x);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[0].y);
			opengl_line_vertex_data.push_back(tri_vec[i][t].vertex[0].z);
		}
	}
}





void mesh::draw(GLint render_shader_program,
	int win_x,
	int win_y,
	string texture_filename, string specular_texture_filename)
{
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


	for (size_t v = 0; v < opengl_vertex_data.size(); v++)
	{
		GLuint num_vertices = static_cast<GLuint>(opengl_vertex_data[v].size()) / components_per_vertex;

		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, opengl_vertex_data[v].size() * sizeof(GLfloat), &opengl_vertex_data[v][0], GL_DYNAMIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glDrawArrays(GL_TRIANGLES_ADJACENCY, 0, num_vertices);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
}