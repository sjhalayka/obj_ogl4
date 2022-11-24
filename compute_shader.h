#ifndef COMPUTE_SHADER
#define COMPUTE_SHADER

#include "GL/glew.h"
#include "GL/glut.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;


class compute_shader
{
public:

	compute_shader(void) { program = 0; }
	~compute_shader(void) { if(program !=0) {glDeleteProgram(program);} }

	bool init(const char* compute_shader_filename);
	void use_program(void);
	GLuint get_program(void) { return program; };

private: 
	const GLchar* read_text_file(const char* filename);
	GLuint program;

};



#endif