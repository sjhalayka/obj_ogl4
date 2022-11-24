#include "main.h"


int main(int argc, char** argv)
{

	mesh game_piece_mesh;



	if (false == game_piece_mesh.read_quads_from_vox_file("chr_knight.vox", "black.png", "chr_knight.png", "black.png", false))
	{
		cout << "Error: Could not properly read vox file" << endl;
		return 2;
	}

	player_game_piece_meshes.push_back(game_piece_mesh);




	if (false == game_piece_mesh.read_quads_from_vox_file("chr_rain.vox", "black.png", "chr_rain.png", "black.png", false))
	{
		cout << "Error: Could not properly read vox file" << endl;
		return 2;
	}

	player_game_piece_meshes.push_back(game_piece_mesh);



	if (false == game_piece_mesh.read_quads_from_vox_file("wraith.vox", "wraith_glow.png", "wraith.png", "black.png", false))
	{
		cout << "Error: Could not properly read vox file" << endl;
		return 2;
	}

	player_game_piece_meshes.push_back(game_piece_mesh);


	if (false == game_piece_mesh.read_quads_from_vox_file("beholder.vox",  "black.png", "beholder.png", "black.png", false))
	{
		cout << "Error: Could not properly read vox file" << endl;
		return 2;
	}

	player_game_piece_meshes.push_back(game_piece_mesh);






	if (false == light_mesh.read_quads_from_vox_file("3x3x3.vox", "3x3x3.png", "3x3x3.png", "3x3x3.png", false))
	{	
		cout << "Error: Could not properly read file 3x3x3.vox" << endl;
		return 2;
	}


	if (false == board_mesh.read_quads_from_vox_file("board.vox", "black.png", "board.png", "board_specular.png", true))
	{
		cout << "Error: Could not properly read file board.vox" << endl;
		return 2;
	}

	board_highlight_colours.resize(board_mesh.num_cells_wide*board_mesh.num_cells_wide, vec3(0, 0, 0));
	board_highlight_enabled.resize(board_mesh.num_cells_wide * board_mesh.num_cells_wide, false);



	for (size_t i = 0; i < player_game_piece_meshes.size(); i++)
	{
		player_game_piece_meshes[i].model_mat = mat4(1.0f);
		player_highlight_colours.push_back(vec3(0, 0, 0));
		player_highlight_enabled.push_back(false);
	}




	situate_player_mesh(0, 0, 0);
	situate_player_mesh(4, 2, 1);

	situate_player_mesh(4, 4, 2);
	situate_player_mesh(6, 1, 3);

	board_mesh.model_mat = mat4(1.0f);

























	glutInit(&argc, argv);

	if (false == init_opengl(win_x, win_y))
		return 1;
	

	main_camera.u = glm::pi<float>()/6;
	main_camera.v = glm::pi<float>() - pi<float>() / 4;
	main_camera.w = 27.5f;


	main_camera.calculate_camera_matrices(win_x, win_y, true);

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
	if (anim_deque.size() != 0)
	{
		bool popped_front = false;

		//	auto i = anim_deque.begin();

		std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> elapsed = end_time - anim_deque[0].start_time;

		float e = elapsed.count() / 1000.0f;

		if (e > anim_deque[0].duration)
		{
			player_game_piece_meshes[current_player].model_mat = mat4(1.0f);
			player_game_piece_meshes[current_player].model_mat = translate(player_game_piece_meshes[current_player].model_mat, anim_deque[0].end_location);

			vec3 n_forward = anim_deque[0].end_location - anim_deque[0].start_location;
			n_forward.y = 0;
			n_forward = normalize(n_forward);

			const vec3 n_up = vec3(0, 1, 0);
			const vec3 n_left = cross(n_forward, n_up);

			player_game_piece_meshes[current_player].model_mat[0] = normalize(vec4(-n_forward, 0.0f));
			player_game_piece_meshes[current_player].model_mat[1] = normalize(vec4(n_up, 0.0f));
			player_game_piece_meshes[current_player].model_mat[2] = normalize(vec4(-n_left, 0.0f));

			situate_player_mesh(anim_deque[0].end_cell_x, anim_deque[0].end_cell_y, current_player, false);

			anim_deque.pop_front();
			popped_front = true;
		}
		else
		{
			player_game_piece_meshes[current_player].model_mat = mat4(1.0f);
			player_game_piece_meshes[current_player].model_mat = translate(player_game_piece_meshes[current_player].model_mat, anim_deque[0].curr_pos());

			vec3 n_forward = anim_deque[0].end_location - anim_deque[0].start_location;
			n_forward.y = 0;
			n_forward = normalize(n_forward);

			const vec3 n_up = vec3(0, 1, 0);
			const vec3 n_left = cross(n_forward, n_up);

			player_game_piece_meshes[current_player].model_mat[0] = normalize(vec4(-n_forward, 0.0f));
			player_game_piece_meshes[current_player].model_mat[1] = normalize(vec4(n_up, 0.0f));
			player_game_piece_meshes[current_player].model_mat[2] = normalize(vec4(-n_left, 0.0f));
		}

		// Trigger the next animation
		if (popped_front && anim_deque.size() > 0)
			anim_deque[0].start_time = std::chrono::high_resolution_clock::now();
	}
	else
	{
		if (advance_current_player)
		{
			advance_current_player = false;

			// to do: if no move or attack left, then advance the player

			if (player_game_piece_meshes.size() > 0 && current_player == player_game_piece_meshes.size() - 1)
				current_player = 0;
			else
				current_player++;
		}

		get_hover_collision_location(mouse_x, mouse_y);
		update_board_highlighting();
	}

	glutPostRedisplay();
}



void use_buffers(GLuint frame_buffer, GLuint depth_tex_handle, GLuint colour_tex_handle)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	glClear(GL_DEPTH_BUFFER_BIT);

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_tex_handle);
	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "depth_tex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colour_tex_handle);
	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "colour_tex"), 1);


	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "img_width"), win_x);
	glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "img_height"), win_y);

	const vec3 m = vec3(player_game_piece_meshes[clicked_collision_location_index].model_mat[3].x, player_game_piece_meshes[clicked_collision_location_index].model_mat[3].y, player_game_piece_meshes[clicked_collision_location_index].model_mat[3].z);
	const vec3 m2 = main_camera.eye;

	glUniform1f(glGetUniformLocation(tex_passthrough.get_program(), "model_distance"), distance(m, m2));
	
	glUniform1f(glGetUniformLocation(tex_passthrough.get_program(), "near"), main_camera.near_plane);
	glUniform1f(glGetUniformLocation(tex_passthrough.get_program(), "far"), main_camera.far_plane);

	if (screenshot_mode)
		glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "cam_factor"), cam_factor);
	else
		glUniform1i(glGetUniformLocation(tex_passthrough.get_program(), "cam_factor"), 1);



	// bind the vao
	glBindVertexArray(vao);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	glFlush();
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_2D);


	if (false == point_shader.init("point.vs.glsl", "point.fs.glsl"))
	{
		cout << "Could not load point shader" << endl;
		return false;
	}

	if (false == point_depth_shader.init("point_depth.vs.glsl", "point_depth.gs.glsl", "point_depth.fs.glsl"))
	{
		cout << "Could not load point depth shader" << endl;
		return false;
	}


	if (false == tex_passthrough.init("ortho.vs.glsl", "ortho.fs.glsl"))
	{
		cout << "Could not load ortho shader" << endl;
		return false;
	}

	if (false == line_shader.init("lines.vs.glsl", "lines.gs.glsl", "lines.fs.glsl"))
	{
		cout << "Could not load line shader" << endl;
		return false;
	}


	
	if (false == tex_reflectance.init("ortho_reflectance.vs.glsl", "ortho_reflectance.fs.glsl"))
	{
		cout << "Could not load ortho reflectance shader" << endl;
		return false;
	}

	if (false == ortho_text.init("ortho_text.vs.glsl", "ortho_text.fs.glsl"))
	{
		cout << "Could not load ortho text shader" << endl;
		return false;
	}

	if (false == glowmap_copier.init("glow.cs.glsl"))
	{
		cout << "Could not load glowmap copier shader" << endl;
		return false;
	}



			

	init_character_set();


	for (size_t i = 0; i < max_num_lights; i++)
	{
		glGenFramebuffers(1, &depthMapFBOs[i]);

		glGenTextures(1, &depthCubemaps[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps[i]);

		for (unsigned int j = 0; j < 6; ++j)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

		//		glTexImag
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemaps[i], 0);

		GLenum drawBuffers[] = { GL_NONE };
		glDrawBuffers(1, drawBuffers);

		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

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

	main_camera.calculate_camera_matrices(win_x, win_y, true);

	init_offscreen_fbo();
}















void draw_scene(GLuint fbo_handle)
{
	glUseProgram(point_shader.get_program());

	GLuint upside_down_white_mask_tex = 0;
	GLuint upside_down_tex = 0;
	GLuint reflectance_tex = 0;
	GLuint regular_tex = 0;
	GLuint glowmap_tex = 0;
	GLuint d_tex = 0;

	glGenTextures(1, &upside_down_white_mask_tex);
	glBindTexture(GL_TEXTURE_2D, upside_down_white_mask_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &upside_down_tex);
	glBindTexture(GL_TEXTURE_2D, upside_down_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &reflectance_tex);
	glBindTexture(GL_TEXTURE_2D, reflectance_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &regular_tex);
	glBindTexture(GL_TEXTURE_2D, regular_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &d_tex);
	glBindTexture(GL_TEXTURE_2D, d_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &glowmap_tex);
	glBindTexture(GL_TEXTURE_2D, glowmap_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// to do: recreate this on window size change
	if (false == glIsTexture(last_frame_glowmap_tex))
	{
		glGenTextures(1, &last_frame_glowmap_tex);
		glBindTexture(GL_TEXTURE_2D, last_frame_glowmap_tex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, win_x, win_y);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glReadBuffer(GL_COLOR_ATTACHMENT0);


	// Upside down white mask
	draw_stuff(offscreen_fbo, true, false, true, false);
	glBindTexture(GL_TEXTURE_2D, upside_down_white_mask_tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, win_x, win_y);

	//// Upside down
	draw_stuff(offscreen_fbo, true, false, false, false);
	glBindTexture(GL_TEXTURE_2D, upside_down_tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, win_x, win_y);

	// Reflectance map
	draw_stuff(offscreen_fbo, false, true, false, false);
	glBindTexture(GL_TEXTURE_2D, reflectance_tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, win_x, win_y);

	// Regular map
	draw_stuff(offscreen_fbo, false, false, false, false);
	glBindTexture(GL_TEXTURE_2D, regular_tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, win_x, win_y);

	// Glow map
	draw_stuff(offscreen_fbo, false, false, false, true);
	glBindTexture(GL_TEXTURE_2D, glowmap_tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, win_x, win_y);





	glReadBuffer(GL_DEPTH_ATTACHMENT);
	glBindTexture(GL_TEXTURE_2D, d_tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, win_x, win_y);

	// use the shader program
	glUseProgram(tex_reflectance.get_program());

	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "img_width"), win_x);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "img_height"), win_y);

	if (screenshot_mode)
		glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "cam_factor"), cam_factor);
	else
		glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "cam_factor"), 1);




	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, upside_down_white_mask_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "upside_down_white_mask_tex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, upside_down_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "upside_down_tex"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, reflectance_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "reflectance_tex"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, regular_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "regular_tex"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, glowmap_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "glowmap_tex"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, last_frame_glowmap_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "last_frame_glowmap_tex"), 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, d_tex);
	glUniform1i(glGetUniformLocation(tex_reflectance.get_program(), "depth_tex"), 6);





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


	// bind the vao
	glBindVertexArray(vao);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	use_buffers(fbo_handle, d_tex, offscreen_colour_tex);





	glowmap_copier.use_program();

	// create output temp texture, with texstorage
	GLuint temp_tex;

	glGenTextures(1, &temp_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, temp_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_x, win_y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, temp_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, temp_tex);
	glUniform1i(glGetUniformLocation(glowmap_copier.get_program(), "output_image"), 0);

	// activate glow and last frame glow input textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glowmap_tex);
	glUniform1i(glGetUniformLocation(glowmap_copier.get_program(), "inputa_image"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, last_frame_glowmap_tex);
	glUniform1i(glGetUniformLocation(glowmap_copier.get_program(), "inputb_image"), 2);

	// call compute shader
	glDispatchCompute((GLuint)win_x, (GLuint)win_y, 1);

	// Wait for compute shader to finish
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// copy from temp to last frame using glCopyImageSubData
	glCopyImageSubData(temp_tex, GL_TEXTURE_2D, 0, 0, 0, 0,
		last_frame_glowmap_tex, GL_TEXTURE_2D, 0, 0, 0, 0,
		win_x, win_y, 1);

	//glCopyImageSubData(glowmap_tex, GL_TEXTURE_2D, 0, 0, 0, 0,
	//	last_frame_glowmap_tex, GL_TEXTURE_2D, 0, 0, 0, 0,
	//	win_x, win_y, 1);

	glDeleteTextures(1, &temp_tex);




	glDeleteTextures(1, &upside_down_white_mask_tex);
	glDeleteTextures(1, &upside_down_tex);
	glDeleteTextures(1, &reflectance_tex);
	glDeleteTextures(1, &regular_tex);
	glDeleteTextures(1, &glowmap_tex);
	glDeleteTextures(1, &d_tex);




	//const string s = "Paused. Press esc to continue!";
	//const float font_scale = win_x / 1024.0f;

	//size_t sentence_width = get_sentence_width(font_scale, mimgs, s);
	//size_t window_width = win_x;
	//size_t window_height = win_y;

	//ortho_text.use_program();
	//print_sentence(font_scale, mimgs, ortho_text.get_program(), win_x, win_y, window_width / 2 - sentence_width / 2, window_height / 3, s);

}




void display_func(void)
{
	draw_scene(0);




	glutSwapBuffers();
}





void mouse_func(int button, int state, int x, int y)
{
	if (GLUT_LEFT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
		{
			if (anim_deque.size() > 0)
				return;

			get_collision_location(x, y);

			vec4 m = player_game_piece_meshes[current_player].model_mat[3];
			vec3 player_centre(m.x, m.y, m.z);
				
			vec3 start_centre = board_mesh.get_centre(clicked_cell_x, clicked_cell_y);
			start_centre.y = board_mesh.get_y_plane_min(clicked_cell_x, clicked_cell_y);

			size_t index = clicked_cell_y * board_mesh.num_cells_wide + clicked_cell_x;


			if (board_highlight_colours[index].g == 1.0)
			{
				advance_current_player = true;

				if (output_path.size() == 0)
				{
					output_path.push_back(pair<size_t, size_t>(clicked_cell_x, clicked_cell_y));
					output_path.push_back(pair<size_t, size_t>(clicked_cell_x, clicked_cell_y));
				}
				else
				{
					for (size_t i = 0; i < output_path.size() - 1; i++)
					{
						vec3 start_centre = board_mesh.get_centre(output_path[i].first, output_path[i].second);
						start_centre.y = board_mesh.get_y_plane_min(output_path[i].first, output_path[i].second);
						start_centre.y += player_game_piece_meshes[current_player].get_y_extent() * 0.5;

						vec3 end_centre = board_mesh.get_centre(output_path[i + 1].first, output_path[i + 1].second);
						end_centre.y = board_mesh.get_y_plane_min(output_path[i + 1].first, output_path[i + 1].second);
						end_centre.y += player_game_piece_meshes[current_player].get_y_extent() * 0.5;

						arc_animation a;
						a.start_location = start_centre;
						a.end_location = end_centre;
						a.duration = 0.5;
						a.end_cell_x = output_path[i + 1].first;
						a.end_cell_y = output_path[i + 1].second;

						// Only need to set this for the first animation in a batch
						a.start_time = std::chrono::high_resolution_clock::now();

						anim_deque.push_back(a);
					}
				}
			}

			

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

		main_camera.calculate_camera_matrices(win_x, win_y, true);
	}
	else if (true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y) * w_spacer;

		if (main_camera.w < 2.0f)
			main_camera.w = 2.0f;

		main_camera.calculate_camera_matrices(win_x, win_y, true);
	}
}



void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;

	get_hover_collision_location(x, y);

	update_board_highlighting();


}





void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'm':
		take_screenshot2(cam_factor, "out.tga");// , const bool reverse_rows = false)
		//take_screenshot3(1, "out.tga");


		break;

	//case 'z':
	//	player_game_piece_meshes[2].model_mat[3][1] = player_game_piece_meshes[2].model_mat[3][1] + 0.1f;// vec4(n_up * displacement, 1.0f);
	//	break;

	//case 'x':
	//	player_game_piece_meshes[2].model_mat[3][1] = player_game_piece_meshes[2].model_mat[3][1] - 0.1f;// vec4(n_up * displacement, 1.0f);
	//	break;

	//case 'w':
	//	player_game_piece_meshes[2].model_mat[3][0] = player_game_piece_meshes[2].model_mat[3][0] + 0.1f;// vec4(n_up * displacement, 1.0f);
	//	break;

	//case 's':
	//	player_game_piece_meshes[2].model_mat[3][0] = player_game_piece_meshes[2].model_mat[3][0] - 0.1f;// vec4(n_up * displacement, 1.0f);
	//	break;

	//case 'a':
	//	player_game_piece_meshes[2].model_mat[3][2] = player_game_piece_meshes[2].model_mat[3][2] + 0.1f;// vec4(n_up * displacement, 1.0f);
	//	break;

	//case 'd':
	//	player_game_piece_meshes[2].model_mat[3][2] = player_game_piece_meshes[2].model_mat[3][2] - 0.1f;// vec4(n_up * displacement, 1.0f);
	//	break;

	case 'o':
		y_offset -= 0.01f;
		break;

	case 'p':
		y_offset += 0.01f;
		break;

	default:
		break;
	}
}