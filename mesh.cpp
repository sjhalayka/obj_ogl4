#include "mesh.h"



void mesh::init_opengl_data(void)
{
	opengl_vertex_data.clear();

	for (size_t i = 0; i < triangles.size(); i++)
	{
		opengl_vertex_data.push_back(triangles[i].vertex[0].x);
		opengl_vertex_data.push_back(triangles[i].vertex[0].y);
		opengl_vertex_data.push_back(triangles[i].vertex[0].z);
		opengl_vertex_data.push_back(triangles[i].vertex[0].nx);
		opengl_vertex_data.push_back(triangles[i].vertex[0].ny);
		opengl_vertex_data.push_back(triangles[i].vertex[0].nz);
		opengl_vertex_data.push_back(triangles[i].vertex[0].u);
		opengl_vertex_data.push_back(triangles[i].vertex[0].v);

		opengl_vertex_data.push_back(triangles[i].vertex[1].x);
		opengl_vertex_data.push_back(triangles[i].vertex[1].y);
		opengl_vertex_data.push_back(triangles[i].vertex[1].z);
		opengl_vertex_data.push_back(triangles[i].vertex[1].nx);
		opengl_vertex_data.push_back(triangles[i].vertex[1].ny);
		opengl_vertex_data.push_back(triangles[i].vertex[1].nz);
		opengl_vertex_data.push_back(triangles[i].vertex[1].u);
		opengl_vertex_data.push_back(triangles[i].vertex[1].v);

		opengl_vertex_data.push_back(triangles[i].vertex[2].x);
		opengl_vertex_data.push_back(triangles[i].vertex[2].y);
		opengl_vertex_data.push_back(triangles[i].vertex[2].z);
		opengl_vertex_data.push_back(triangles[i].vertex[2].nx);
		opengl_vertex_data.push_back(triangles[i].vertex[2].ny);
		opengl_vertex_data.push_back(triangles[i].vertex[2].nz);
		opengl_vertex_data.push_back(triangles[i].vertex[2].u);
		opengl_vertex_data.push_back(triangles[i].vertex[2].v);

	}
}





void mesh::draw(GLint render_shader_program,
	int win_x,
	int win_y)
{
	glUseProgram(render_shader_program);

	const GLuint components_per_vertex = 8;
	const GLuint components_per_position = 3;
	const GLuint components_per_normal = 3;
	const GLuint components_per_texcoord = 2;

	std::vector<unsigned char> buffer, image;
	loadFile(buffer, "chr_knight.png");
	unsigned long w, h;
	decodePNG(image, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));

	GLuint tex_handle = 0;

	const size_t tex_width = w;
	const size_t tex_height = h;

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_handle);
	glGenTextures(1, &tex_handle);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(tex_width), static_cast<GLsizei>(tex_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);


	glUniform1i(glGetUniformLocation(render_shader_program, "colour_tex"), 1);

	GLuint num_vertices = static_cast<GLuint>(opengl_vertex_data.size()) / components_per_vertex;

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, opengl_vertex_data.size() * sizeof(GLfloat), &opengl_vertex_data[0], GL_DYNAMIC_DRAW);

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
	
	glDeleteTextures(1, &tex_handle);
}