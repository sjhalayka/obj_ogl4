#include "main.h"
#include "mesh.h"


int main(int argc, char** argv)
{

	mesh game_piece_mesh;

	if (false == game_piece_mesh.read_triangles_from_wavefront_obj_file("chr_knight.obj"))
	{
		cout << "Error: Could not properly read file " << argv[1] << endl;
		return 2;
	}

	player_game_piece_meshes.push_back(game_piece_mesh);




	for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
	{
		vec3 dir(-1, 0, 0);
		vec3 dir2(0, -1, 0);

		vec3 left = normalize(cross(dir, dir2));
		vec3 tangent = cross(dir, left);

		player_game_piece_meshes[i].init_model_matrix(dir, left, tangent);
	}


	vec3 c;

	c.x = 1.0f;
	c.y = 0.0f;
	c.z = 0.0f;

	colours.push_back(c);

	c.x = 0.0f;
	c.y = 1.0f;
	c.z = 0.0f;

	colours.push_back(c);

	c.x = 0.0f;
	c.y = 0.0f;
	c.z = 1.0f;
	colours.push_back(c);






	glutInit(&argc, argv);

	if (false == init_opengl(win_x, win_y))
		return 1;



	glutReshapeFunc(reshape_func);
	glutIdleFunc(idle_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutPassiveMotionFunc(passive_motion_func);
	//glutIgnoreKeyRepeat(1);
	glutMainLoop();
	glutDestroyWindow(win_id);

	return 0;
}



void idle_func(void)
{
	glutPostRedisplay();
}


void init_offscreen_fbo(void)
{
	if (offscreen_fbo != 0)
		glDeleteFramebuffers(1, &offscreen_fbo);

	if(offscreen_colour_tex != 0)
		glDeleteTextures(1, &offscreen_colour_tex);

	if (offscreen_depth_tex != 0)
		glDeleteTextures(1, &offscreen_depth_tex);

	glActiveTexture(GL_TEXTURE7);
	glGenTextures(1, &offscreen_depth_tex);
	glBindTexture(GL_TEXTURE_2D, offscreen_depth_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE12);
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

	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

}


bool init_opengl(const int& width, const int& height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("OBJ file viewer");

	if (GLEW_OK != glewInit())
	{
		cout << "GLEW initialization error" << endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	if (false == shadow_map.init("shadowmap.vs.glsl", "shadowmap.fs.glsl"))
	{
		cout << "Could not load shadowmap shader" << endl;
		return false;
	}

	if (false == tex_passthrough.init("ortho.vs.glsl", "ortho.fs.glsl"))
	{
		cout << "Could not load ortho shader" << endl;
		return false;
	}




	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, static_cast<GLsizei>(shadowMapWidth), static_cast<GLsizei>(shadowMapHeight));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Assign the depth buffer texture to texture channel 0

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);


	init_offscreen_fbo();


	return true;
}

void reshape_func(int width, int height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);

	main_camera.calculate_camera_matrices(win_x, win_y);

	init_offscreen_fbo();
}



void draw_stuff(GLuint fbo_handle)
{
	static std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> elapsed = end_time - start_time;

	mat4 lightPV, shadowBias;
	Frustum lightFrustum;

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	srand(0);


	// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows



	shadow_map.use_program();
	glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 0);

	GLuint programHandle = shadow_map.get_program();
	pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
	pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	shadowBias = mat4(
		vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	vec3 left = cross(normalize(main_camera.eye), normalize(main_camera.up));
	vec3 lightPos = normalize(main_camera.eye) + normalize(main_camera.up) * 2.0f + left * 2.0f;
	lightPos = normalize(lightPos) * 10.0f;

	lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lightFrustum.setPerspective(45.0f, 1.0f, 1.0f, 25.0f);

	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(shadow_map.get_program(), "shadow_map"), 4);

	mat4 model = mat4(1.0f);
	mat4 view = lightFrustum.getViewMatrix();
	mat4 proj = lightFrustum.getProjectionMatrix();

	mat3 normal = mat3(vec3((lightFrustum.getViewMatrix() * model)[0]), vec3((lightFrustum.getViewMatrix() * model)[1]), vec3((lightFrustum.getViewMatrix() * model)[2]));
	lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();
	mat4 shadow = lightPV * model;

	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ViewMatrix"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ProjectionMatrix"), 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);

	vec4 lp = view * vec4(lightPos, 0.0f);
	glUniform4f(glGetUniformLocation(shadow_map.get_program(), "LightPosition"), lp.x, lp.y, lp.z, lp.w);

	vec4 lp_untransformed = vec4(lightPos, 0.0f);
	glUniform4f(glGetUniformLocation(shadow_map.get_program(), "LightPosition_Untransformed"), lp_untransformed.x, lp_untransformed.y, lp_untransformed.z, lp_untransformed.w);


	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, static_cast<GLsizei>(shadowMapWidth), static_cast<GLsizei>(shadowMapHeight));
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
	glDisable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.5f, 10.0f);





	glUniform3f(glGetUniformLocation(shadow_map.get_program(), "MaterialKd"), 1.0f, 1.0f, 1.0f);

	model = mat4(1.0f);
	normal = mat3(vec3((view * model)[0]), vec3((view * model)[1]), vec3((view * model)[2]));
	shadow = lightPV * model;
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);


	for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
	{
		glUniform3f(glGetUniformLocation(shadow_map.get_program(), "MaterialKd"), colours[i].x, colours[i].y, colours[i].z);

		model = player_game_piece_meshes[i].model_mat;
		normal = mat3(vec3((lightFrustum.getViewMatrix() * model)[0]), vec3((lightFrustum.getViewMatrix() * model)[1]), vec3((lightFrustum.getViewMatrix() * model)[2]));
		shadow = lightPV * model;
		glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);

		player_game_piece_meshes[i].draw(shadow_map.get_program(), win_x, win_y);

	}









	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);


	main_camera.calculate_camera_matrices(win_x, win_y);

	model = mat4(1.0f);
	view = main_camera.view_mat;
	proj = main_camera.projection_mat;
	normal = mat3(vec3((view * model)[0]), vec3((view * model)[1]), vec3((view * model)[2]));
	lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();
	shadow = lightPV * model;

	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ViewMatrix"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ProjectionMatrix"), 1, GL_FALSE, &proj[0][0]);

	glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);


	lp = view * vec4(lightPos, 0.0f);
	glUniform4f(glGetUniformLocation(shadow_map.get_program(), "LightPosition"), lp.x, lp.y, lp.z, lp.w);
	lp_untransformed = vec4(lightPos, 0.0f);
	glUniform4f(glGetUniformLocation(shadow_map.get_program(), "LightPosition_Untransformed"), lp_untransformed.x, lp_untransformed.y, lp_untransformed.z, lp_untransformed.w);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, win_x, win_y);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUniform3f(glGetUniformLocation(shadow_map.get_program(), "MaterialKd"), 1.0f, 1.0f, 1.0f);
	model = mat4(1.0f);
	normal = mat3(vec3((view * model)[0]), vec3((view * model)[1]), vec3((view * model)[2]));
	shadow = lightPV * model;

	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);




	for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
	{
		glUniform3f(glGetUniformLocation(shadow_map.get_program(), "MaterialKd"), colours[i].x, colours[i].y, colours[i].z);

		model = player_game_piece_meshes[i].model_mat;
		normal = mat3(vec3((view * model)[0]), vec3((view * model)[1]), vec3((view * model)[2]));
		shadow = lightPV * model;

		glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);

		player_game_piece_meshes[i].draw(shadow_map.get_program(), win_x, win_y);


		glCullFace(GL_FRONT);
		glPolygonMode(GL_BACK, GL_LINE);


		// Draw outlines



		glUniform3f(glGetUniformLocation(shadow_map.get_program(), "MaterialKd"), 0, 0, 0);

		glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 1);
		player_game_piece_meshes[i].draw(shadow_map.get_program(), win_x, win_y);
		glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 0);

		glPolygonMode(GL_BACK, GL_FILL);
		glCullFace(GL_BACK);


		glPolygonMode(GL_FRONT, GL_FILL);
	}

	model = mat4(1.0f);
	normal = mat3(vec3((view * model)[0]), vec3((view * model)[1]), vec3((view * model)[2]));
	shadow = lightPV * model;

	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ModelMatrix"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(shadow_map.get_program(), "NormalMatrix"), 1, GL_FALSE, &normal[0][0]);		glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadow_map.get_program(), "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);

	glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 1);
	draw_axis(shadow_map.get_program());
	glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 0);


}


void use_buffers(GLuint frame_buffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);


	// vao and vbo handle
	GLuint vao, vbo, ibo;

	// https://raw.githubusercontent.com/progschj/OpenGL-Examples/master/03texture.cpp

	// generate and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// generate and bind the vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// http://www.songho.ca/opengl/gl_transform.html


	// data for a fullscreen quad (this time with texture coords)
	static const GLfloat vertexData[] = {
		//  X     Y     Z           U     V     
		   1.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
		  -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
		   1.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
		  -1.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
	}; // 4 vertices with 5 components (floats) each

	// fill with data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 5, vertexData, GL_DYNAMIC_DRAW);


	// set up generic attrib pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)0 + 3 * sizeof(GLfloat));


	// generate and bind the index buffer object
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	static const GLuint indexData[] = {
		0,1,2, // first triangle
		2,1,3, // second triangle
	};

	// fill with data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 2 * 3, indexData, GL_DYNAMIC_DRAW);

	// "unbind" vao
	glBindVertexArray(0);


	// use the shader program
	glUseProgram(tex_passthrough.get_program());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, offscreen_depth_tex);
	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "depth_tex"), 7);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, offscreen_colour_tex);
	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "colour_tex"), 12);

	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "img_width"), win_x);
	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "img_height"), win_y);

	const vec3 m = vec3(player_game_piece_meshes[0].model_mat[3].x, player_game_piece_meshes[0].model_mat[3].y, player_game_piece_meshes[0].model_mat[3].z);
	const vec3 m2 = main_camera.eye;	

	glUniform1f(glGetUniformLocation(tex_passthrough.get_program(), "model_distance"), distance(m, m2));
	glUniform1f(glGetUniformLocation(tex_passthrough.get_program(), "near"), main_camera.near_plane);
	glUniform1f(glGetUniformLocation(tex_passthrough.get_program(), "far"), main_camera.far_plane);




	// bind the vao
	glBindVertexArray(vao);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	glFlush();
}


void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_stuff(offscreen_fbo);
	use_buffers(0);

	glutSwapBuffers();
}






void mouse_func(int button, int state, int x, int y)
{
	if (GLUT_LEFT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
		{
			lmb_down = true;
		}
		else
			lmb_down = false;
	}
	else if (GLUT_MIDDLE_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			mmb_down = true;
		else
			mmb_down = false;
	}
	else if (GLUT_RIGHT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			rmb_down = true;
		else
			rmb_down = false;
	}
}

void motion_func(int x, int y)
{
	int prev_mouse_x = mouse_x;
	int prev_mouse_y = mouse_y;

	mouse_x = x;
	mouse_y = y;

	int mouse_delta_x = mouse_x - prev_mouse_x;
	int mouse_delta_y = prev_mouse_y - mouse_y;

	if (true == lmb_down && (0 != mouse_delta_x || 0 != mouse_delta_y))
	{
		main_camera.u -= static_cast<float>(mouse_delta_y) * u_spacer;
		main_camera.v += static_cast<float>(mouse_delta_x) * v_spacer;

		main_camera.calculate_camera_matrices(win_x, win_y);
	}
	else if (true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y) * w_spacer;

		if (main_camera.w < 2.0f)
			main_camera.w = 2.0f;

		main_camera.calculate_camera_matrices(win_x, win_y);
	}
}

void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
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


	init_offscreen_fbo();

	main_camera.calculate_camera_matrices(win_x, win_y);

	glBindFramebuffer(GL_FRAMEBUFFER, offscreen_fbo);


	glUseProgram(shadow_map.get_program());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	draw_stuff(offscreen_fbo);
	use_buffers(offscreen_fbo);


	glGenVertexArrays(1, &quad_vao);

	glDisable(GL_DEPTH_TEST);
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
	main_camera.calculate_camera_matrices(win_x, win_y);

	screenshot_mode = false;

	init_offscreen_fbo();

}


void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'm':
		take_screenshot2(4, "out.tga");// , const bool reverse_rows = false)
		//take_screenshot3(1, "out.tga");


		break;

	case 'a':
		player_game_piece_meshes[0].model_mat[3][0] = player_game_piece_meshes[0].model_mat[3][0] + 0.1f;// vec4(n_up * displacement, 1.0f);
		break;

	case 's':
		player_game_piece_meshes[0].model_mat[3][0] = player_game_piece_meshes[0].model_mat[3][0] - 0.1f;// vec4(n_up * displacement, 1.0f);
		break;

	default:
		break;
	}
}