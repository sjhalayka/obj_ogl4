#include "mesh.h"



void mesh::init_opengl_data(void)
{
	opengl_vertex_data.clear();
	opengl_vertex_data.resize(tri_vec.size());
	opengl_line_vertex_data.clear();

	// triangles
	for (size_t v = 0; v < tri_vec.size(); v++)
	{
		for (size_t i = 0; i < tri_vec[v].size(); i++)
		{
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].x);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].y);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].z);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nx);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].ny);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].nz);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].u);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[0].v);

			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].x);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].y);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].z);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nx);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].ny);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].nz);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].u);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[1].v);

			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].x);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].y);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].z);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nx);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].ny);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].nz);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].u);
			opengl_vertex_data[v].push_back(tri_vec[v][i].vertex[2].v);
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





void mesh::draw(GLint render_shader_program)
{
	glUseProgram(render_shader_program);

	const GLuint components_per_vertex = 8;
	const GLuint components_per_position = 3;
	const GLuint components_per_normal = 3;
	const GLuint components_per_texcoord = 2;


	if (glow_tex == 0)
	{
		//std::vector<unsigned char> buffer;
		//loadFile(buffer, texture_filename.c_str());
		//unsigned long w, h;
		//decodePNG(colour_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		//colour_data_x = w;
		//colour_data_y = h;

		glGenTextures(1, &glow_tex);
		glBindTexture(GL_TEXTURE_2D, glow_tex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}


	if (colour_tex == 0)
	{
		//std::vector<unsigned char> buffer;
		//loadFile(buffer, texture_filename.c_str());
		//unsigned long w, h;
		//decodePNG(colour_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		//colour_data_x = w;
		//colour_data_y = h;
		
		glGenTextures(1, &colour_tex);
		glBindTexture(GL_TEXTURE_2D, colour_tex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}


	if (specular_tex == 0)
	{
		//std::vector<unsigned char> buffer;
		//loadFile(buffer, specular_texture_filename.c_str());
		//unsigned long w, h;
		//decodePNG(specular_data, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

		//specular_data_x = w;
		//specular_data_y = h;

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

		glDrawArrays(GL_TRIANGLES, 0, num_vertices);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
}