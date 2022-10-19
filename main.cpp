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

	GLenum drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);


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






void display_func(void)
{
	static std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> elapsed = end_time - start_time;

	glActiveTexture(GL_TEXTURE0);

	mat4 lightPV, shadowBias;
	Frustum lightFrustum;

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

	if(false == screenshot_mode)
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

	glFlush();


	vector<unsigned char> output_pixels(win_x * win_y * 4);

	//	vector<unsigned char> output_pixels(win_x * win_y * 4);
	//	glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glReadPixels(0, 0, win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &output_pixels[0]);

	//glDrawPixels(win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &output_pixels[0]);






	vector<unsigned short> depth_pixels(win_x * win_y);

glReadBuffer(GL_DEPTH_ATTACHMENT);
glReadPixels(0, 0, win_x, win_y, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, &depth_pixels[0]);

//glReadBuffer(GL_COLOR_ATTACHMENT0);
//glReadPixels(0, 0, win_x, win_y, GL_BGRA, GL_UNSIGNED_BYTE, &output_pixels[0]);

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

			float val = depth_pixels[depth_index] - depth_min / (1.0f - depth_min);

			output_pixels[tga_index + 0] = val * 255.0f;// / static_cast<unsigned short>(-1);
			output_pixels[tga_index + 1] = val * 255.0f;//  / static_cast<unsigned short>(-1);
			output_pixels[tga_index + 2] = val * 255.0f;// / static_cast<unsigned short>(-1);
			output_pixels[tga_index + 3] = 255;
		}
	}

	cout << depth_min << " " << depth_max << endl;

	//glDrawPixels(win_x, win_y, GL_RGBA, GL_UNSIGNED_BYTE, &output_pixels[0]);


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

out.write(reinterpret_cast<char*>(&output_pixels[0]), win_x * win_y * 4 * sizeof(unsigned char));

out.close();

//exit(1);



	if(false == screenshot_mode)
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




