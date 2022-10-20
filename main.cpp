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
	//static std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	//std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<float, std::milli> elapsed = end_time - start_time;

	//float x = elapsed.count() / 1000.0f;
	//x *= 0.25f;

	//start_time = std::chrono::high_resolution_clock::now();

	glutPostRedisplay();
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

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, static_cast<GLsizei>(shadowMapWidth), static_cast<GLsizei>(shadowMapHeight));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign the depth buffer texture to texture channel 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthTex, 0);



	glActiveTexture(GL_TEXTURE8);
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, static_cast<GLsizei>(win_x), static_cast<GLsizei>(win_y));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign the colour buffer texture to texture channel 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, fbo_texture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);




	//GLenum drawBuffers[] = { GL_NONE };
	//glDrawBuffers(1, drawBuffers);


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
}



void draw_stuff(void)
{
	static std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> elapsed = end_time - start_time;

	glActiveTexture(GL_TEXTURE0);

	mat4 lightPV, shadowBias;
	Frustum lightFrustum;

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClearDepth(1.0f);
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

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shadow_map.get_program(), "shadow_map"), 0);

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

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
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




	glFlush();


	// reset camera matrices

	if (false == screenshot_mode)
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

		//glUniform3f(glGetUniformLocation(shadow_map.get_program(), "MaterialKd"), 0, 0, 0);

		//glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 1);
		//player_game_piece_meshes[i].draw(shadow_map.get_program(), win_x, win_y);
		//glUniform1i(glGetUniformLocation(shadow_map.get_program(), "flat_colour"), 0);

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

	glFlush();

}




void blur_image(vector<unsigned char>& write_p, GLuint width, GLuint height, size_t num_channels)
{
	const vector<unsigned char> read_p = write_p;

	for (size_t i = 1; i < (width - 1); i++)
	{
		for (size_t j = 1; j < (height - 1); j++)
		{
			size_t centre_index = num_channels * (j * width + i);

			size_t up_index = num_channels * ((j + 1) * width + i);
			size_t down_index = num_channels * ((j - 1) * width + i);
			size_t left_index = num_channels * (j * width + (i + 1));
			size_t right_index = num_channels * (j * width + (i - 1));

			size_t upper_left_index = num_channels * ((j + 1) * width + (i + 1));
			size_t upper_right_index = num_channels * ((j + 1) * width + (i - 1));
			size_t lower_left_index = num_channels * ((j - 1) * width + (i + 1));
			size_t lower_right_index = num_channels * ((j - 1) * width + (i - 1));

			float r = 0, g = 0, b = 0, a = 0;

			r = static_cast<float>(read_p[centre_index]) +
				static_cast<float>(read_p[up_index]) +
				static_cast<float>(read_p[down_index]) +
				static_cast<float>(read_p[left_index]) +
				static_cast<float>(read_p[right_index]) +
				static_cast<float>(read_p[upper_left_index]) +
				static_cast<float>(read_p[upper_right_index]) +
				static_cast<float>(read_p[lower_left_index]) +
				static_cast<float>(read_p[lower_left_index]);
			\
				r /= 9.0;

			g = static_cast<float>(read_p[centre_index + 1]) +
				static_cast<float>(read_p[up_index + 1]) +
				static_cast<float>(read_p[down_index + 1]) +
				static_cast<float>(read_p[left_index + 1]) +
				static_cast<float>(read_p[right_index + 1]) +
				static_cast<float>(read_p[upper_left_index + 1]) +
				static_cast<float>(read_p[upper_right_index + 1]) +
				static_cast<float>(read_p[lower_left_index + 1]) +
				static_cast<float>(read_p[lower_left_index + 1]);

			g /= 9.0;

			b = static_cast<float>(read_p[centre_index + 2]) +
				static_cast<float>(read_p[up_index + 2]) +
				static_cast<float>(read_p[down_index + 2]) +
				static_cast<float>(read_p[left_index + 2]) +
				static_cast<float>(read_p[right_index + 2]) +
				static_cast<float>(read_p[upper_left_index + 2]) +
				static_cast<float>(read_p[upper_right_index + 2]) +
				static_cast<float>(read_p[lower_left_index + 2]) +
				static_cast<float>(read_p[lower_left_index + 2]);

			b /= 9.0;

			a = 255.0f;

			write_p[centre_index + 0] = static_cast<unsigned char>(r);
			write_p[centre_index + 1] = static_cast<unsigned char>(g);
			write_p[centre_index + 2] = static_cast<unsigned char>(b);
			write_p[centre_index + 3] = static_cast<unsigned char>(a);
		}
	}

}



void use_buffers(void)
{

	vector<unsigned char> output_pixels(win_x * win_y * 4);

	//	vector<unsigned char> output_pixels(win_x * win_y * 4);
	//	glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glReadPixels(0, 0, win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &output_pixels[0]);

	//glDrawPixels(win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &output_pixels[0]);

	vector<unsigned short> depth_pixels(win_x * win_y);
	vector<unsigned char> depth_output_pixels(4 * win_x * win_y);


	glReadBuffer(GL_DEPTH_ATTACHMENT);
	glReadPixels(0, 0, win_x, win_y, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, &depth_pixels[0]);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &output_pixels[0]);



	unsigned short depth_max = 0, depth_min = -1;

	for (size_t i = 0; i < win_x; i++)
	{
		for (size_t j = 0; j < win_y; j++)
		{
			size_t depth_index = j * win_x + i;
			size_t tga_index = depth_index * 4;

			float val = depth_pixels[depth_index];

			if (val > depth_max)
				depth_max = val;

			else if (val < depth_min)
				depth_min = val;
		}
	}


	for (size_t i = 0; i < win_x; i++)
	{
		for (size_t j = 0; j < win_y; j++)
		{
			size_t depth_index = j * win_x + i;
			size_t tga_index = depth_index * 4;

			float val = (depth_pixels[depth_index] - depth_min) / float(depth_max - depth_min);

			depth_output_pixels[tga_index + 0] = val * 255.0f;// / static_cast<unsigned short>(-1);
			depth_output_pixels[tga_index + 1] = val * 255.0f;//  / static_cast<unsigned short>(-1);
			depth_output_pixels[tga_index + 2] = val * 255.0f;// / static_cast<unsigned short>(-1);
			depth_output_pixels[tga_index + 3] = 255;
		}
	}

	cout << depth_min << " " << depth_max << endl;


	vector<unsigned char> fbpixels_blurred = output_pixels;
	vector<unsigned char> target_pixels = output_pixels;

	for (size_t i = 0; i < 20; i++)
		blur_image(fbpixels_blurred, win_x, win_y, 4);

	for (size_t i = 0; i < win_x; i++)
	{
		for (size_t j = 0; j < win_y; j++)
		{
			size_t depth_index = i * win_y + j;
			size_t fb_index = 4 * depth_index;

			float t = (depth_pixels[depth_index] - depth_min) / float(depth_max - depth_min);


			t = pow(t, 5.0f);

			float r0 = output_pixels[fb_index];
			float g0 = output_pixels[fb_index + 1];
			float b0 = output_pixels[fb_index + 2];

			float r1 = fbpixels_blurred[fb_index];
			float g1 = fbpixels_blurred[fb_index + 1];
			float b1 = fbpixels_blurred[fb_index + 2];

			float r2 = (1.0f - t) * r0 + t * r1;
			float g2 = (1.0f - t) * g0 + t * g1;
			float b2 = (1.0f - t) * b0 + t * b1;

			target_pixels[fb_index + 0] = static_cast<unsigned char>(r2);
			target_pixels[fb_index + 1] = static_cast<unsigned char>(g2);
			target_pixels[fb_index + 2] = static_cast<unsigned char>(b2);
			target_pixels[fb_index + 3] = 255;
		}
	}




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
	unsigned char  bitsperpixel = 32;
	unsigned char  imagedescriptor = 0;
	vector<char> idstring;



	// Write Targa TGA file to disk.
	ofstream out("attachment.tga", ios::binary);

	if (!out.is_open())
	{
		cout << "Failed to open TGA file for writing: attachment.tga" << endl;
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

	out.write(reinterpret_cast<char*>(&target_pixels[0]), win_x * win_y * 4 * sizeof(unsigned char));

	out.close();


	//glDrawPixels(win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &target_pixels[0]);






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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 5, vertexData, GL_STATIC_DRAW);


	// set up generic attrib pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 2 * 3, indexData, GL_STATIC_DRAW);

	// "unbind" vao
	glBindVertexArray(0);


	// texture handle
	GLuint texture;

	glActiveTexture(GL_TEXTURE3);

	// generate texture
	glGenTextures(1, &texture);

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// set texture content
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, win_x, win_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &target_pixels[0]);

	// use the shader program
	glUseProgram(tex_passthrough.get_program());

	// bind texture to third texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "tex"), 3);


	// bind the vao
	glBindVertexArray(vao);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	glDeleteTextures(1, &texture);
}





void display_func(void)
{


	glActiveTexture(GL_TEXTURE1);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);


	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	draw_stuff();

	use_buffers();


	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClearDepth(1.0f);


	//draw_stuff();






	if (false == screenshot_mode)
		glutSwapBuffers();



}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'a':
		take_screenshot(2, "out.tga");// , const bool reverse_rows = false)
		break;

	default:
		break;
	}
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




