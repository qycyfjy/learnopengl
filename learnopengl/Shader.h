#pragma once

#include "stdafx.h"
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

	void setUniform(const char* name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
	}

	unsigned int ID;
private:
};