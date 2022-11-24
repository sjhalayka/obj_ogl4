#include "compute_shader.h"


bool compute_shader::init(const char * compute_shader_filename)
{
	ifstream infile(compute_shader_filename);

	if (infile.fail())
	{
		cout << "Could not open " << compute_shader_filename << endl;
		return false;
	}

	string shader_code;
	string line;

	while (getline(infile, line))
	{
		shader_code += line;
		shader_code += "\n";
	}

	const char* cch = 0;
	GLint status = GL_FALSE;

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &(cch = shader_code.c_str()), NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (GL_FALSE == status)
	{
		string status_string = "Compute shader compile error.\n";
		vector<GLchar> buf(4096, '\0');
		glGetShaderInfoLog(shader, 4095, 0, &buf[0]);

		for (size_t i = 0; i < buf.size(); i++)
			if ('\0' != buf[i])
				status_string += buf[i];

		status_string += '\n';

		cout << status_string << endl;

		glDeleteShader(shader);

		return false;
	}

	// Link compute shader
	program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (GL_FALSE == status)
	{
		string status_string = "Program link error.\n";
		vector<GLchar> buf(4096, '\0');
		glGetShaderInfoLog(program, 4095, 0, &buf[0]);

		for (size_t i = 0; i < buf.size(); i++)
			if ('\0' != buf[i])
				status_string += buf[i];

		status_string += '\n';

		cout << status_string << endl;

		glDetachShader(program, shader);
		glDeleteShader(shader);
		glDeleteProgram(program);

		return false;
	}

	// The shader is no longer needed now that the program
	// has been linked
	glDetachShader(program, shader);
	glDeleteShader(shader);

	return true;


/*

	program = glCreateProgram();

	GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);

 const GLchar* source = read_text_file(compute_shader_filename);

	if(source == NULL) 
	{
		glDeleteProgram(program);
		glDeleteShader(compute_shader);
		return false;
	}

	glShaderSource(compute_shader, 1, &source, NULL);
	delete [] source;

	glCompileShader(compute_shader);
	GLint compiled;
	glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &compiled);

	if(!compiled)
	{
		GLsizei len;
		glGetShaderiv(compute_shader, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len+1];
		glGetShaderInfoLog(compute_shader, len, &len, log);
		cerr << "Compute shader compilation failed: " << log << endl;
		delete [] log;

		glDeleteProgram(program);
		glDeleteShader(compute_shader);
		return false;
	}

	glAttachShader(program, compute_shader);


    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if(!linked)
	{
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog(program, len, &len, log);
        cerr << "Shader linking failed: " << log << endl;
        delete [] log;

		glDeleteProgram(program);
		glDeleteShader(compute_shader);
        
        return false;
    }

	glDeleteShader(compute_shader);

    return true;
	*/
}



#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

const GLchar* compute_shader::read_text_file(const char* filename)
{
	FILE* infile = fopen(filename, "rb");

	if(!infile)
	{
		cerr << "Unable to open file '" << filename << "'" << endl;
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = new GLchar[len+1];

	fread(source, sizeof(char), len, infile);
	fclose(infile);

	source[len] = 0;

	return const_cast<const GLchar*>(source);
}

void compute_shader::use_program(void)
{
	glUseProgram(program);
}