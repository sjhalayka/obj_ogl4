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

	GLuint triangle_buffer;

	glGenBuffers(1, &triangle_buffer);



	std::vector<unsigned char> buffer, image;
	loadFile(buffer, "chr_knight.png");
	unsigned long w, h;
	decodePNG(image, w, h, &buffer[0], buffer.size() * sizeof(unsigned char));
	
	GLuint tex_handle = 0;

	const size_t tex_width = w;
	const size_t tex_height = h;

	cout << tex_width << " " << tex_height << endl;

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &tex_handle);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(tex_width), static_cast<GLsizei>(tex_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_handle);
	glUniform1i(glGetUniformLocation(render_shader_program, "tex"), 2);


	GLuint num_vertices = static_cast<GLuint>(opengl_vertex_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer);
	glBufferData(GL_ARRAY_BUFFER, opengl_vertex_data.size() * sizeof(GLfloat), &opengl_vertex_data[0], GL_DYNAMIC_DRAW);

	//glEnableVertexAttribArray(glGetAttribLocation(render_shader_program, "position"));
	//glVertexAttribPointer(glGetAttribLocation(render_shader_program, "position"),
	//	components_per_position,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	components_per_vertex * sizeof(GLfloat),
	//	NULL);

	//glEnableVertexAttribArray(glGetAttribLocation(render_shader_program, "normal"));
	//glVertexAttribPointer(glGetAttribLocation(render_shader_program, "normal"),
	//	components_per_normal,
	//	GL_FLOAT,
	//	GL_TRUE,
	//	components_per_vertex * sizeof(GLfloat),
	//	(const GLvoid*)(components_per_position * sizeof(GLfloat)));

	//glEnableVertexAttribArray(glGetAttribLocation(render_shader_program, "texcoord"));
	//glVertexAttribPointer(glGetAttribLocation(render_shader_program, "texcoord"),
	//	components_per_texcoord,
	//	GL_FLOAT,
	//	GL_TRUE,
	//	components_per_vertex * sizeof(GLfloat),
	//	(const GLvoid*)(components_per_normal * sizeof(GLfloat) + components_per_position * sizeof(GLfloat)));


	//glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer);
	//glBufferData(GL_ARRAY_BUFFER, opengl_vertex_data.size() * sizeof(GLfloat), &opengl_vertex_data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		components_per_normal,
		GL_FLOAT,
		GL_TRUE,
		components_per_vertex * sizeof(GLfloat),
		(const GLvoid*)(components_per_position * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,
		components_per_texcoord,
		GL_FLOAT,
		GL_TRUE,
		components_per_vertex * sizeof(GLfloat),
		(const GLvoid*)(components_per_normal * sizeof(GLfloat) + components_per_position * sizeof(GLfloat)));





	glDrawArrays(GL_TRIANGLES, 0, num_vertices);


	glDeleteBuffers(1, &triangle_buffer);



	glDeleteTextures(1, &tex_handle);
}