#include "Shader.h"
#include "spdlog/spdlog.h"

static void CheckShader(unsigned int shader) {
	int success;
	char err_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, err_log);
		spdlog::error("failed to compile shader: {}\n", err_log);
	}
}

static void CheckProgram(unsigned int program) {
	int success;
	char err_log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, err_log);
		spdlog::error("failed to link shaders: {}\n", err_log);
	}
}

Shader::Shader(const char* vertex_fpath, const char* fragment_fpath)
{

	auto vert = readfile(vertex_fpath);
	auto vert_src = vert.c_str();
	auto frag = readfile(fragment_fpath);
	auto frag_src = frag.c_str();

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vert_src, nullptr);
	glCompileShader(vertex);
	CheckShader(vertex);
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &frag_src, nullptr);
	glCompileShader(fragment);
	CheckShader(fragment);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	CheckProgram(ID);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
