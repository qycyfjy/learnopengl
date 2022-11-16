#pragma once

#include "glad/glad.h"
#include "Utils.h"

class Shader {
public:
	Shader(const char* vertex_fpath, const char* fragment_fpath);

	void use()
	{
		glUseProgram(ID);
	}

	void setUniform(const char* name, bool value)
	{
		setUniform(name, static_cast<int>(value));
	}

	void setUniform(const char* name, int value)
	{
		glUniform1i(glGetUniformLocation(ID, name), value);
	}

	void setUniform(const char* name, float value)
	{
		glUniform1f(glGetUniformLocation(ID, name), value);
	}

	unsigned int ID;
private:
};