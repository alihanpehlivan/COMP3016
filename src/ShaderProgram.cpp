#include "StdAfx.h"
#include "ShaderProgram.h"

//
// Verifys shader compilation statuses
//
inline static void CheckShaderCompileStatus(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			LOG_ERROR("shader compilation error of type: {} / {}", type, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			LOG_ERROR("linking error of type: {} / {}", type, infoLog);
		}
	}
}

void ShaderCreateProgram(Shader& self)
{
	// compile shaders
	unsigned int vertex, fragment;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &self.vs_code, NULL);
	glCompileShader(vertex);
	CheckShaderCompileStatus(vertex, "VERTEX"); // verify compilation

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &self.ps_code, NULL);
	glCompileShader(fragment);
	CheckShaderCompileStatus(fragment, "FRAGMENT"); // verify compilation

	// shader Program
	self.ID = glCreateProgram();
	glAttachShader(self.ID, vertex);
	glAttachShader(self.ID, fragment);
	glLinkProgram(self.ID);
	CheckShaderCompileStatus(self.ID, "PROGRAM"); // verify compilation

	// free the shaders as they're linked into our program and not necessary to keep them
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

void ShaderUseProgram(Shader & self)
{
	glUseProgram(self.ID);
}

//
// See: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
//
void setBool(Shader & self, const std::string & name, bool value)
{
	glUniform1i(glGetUniformLocation(self.ID, name.c_str()), (int)value);
}

void setInt(Shader & self, const std::string & name, int value)
{
	glUniform1i(glGetUniformLocation(self.ID, name.c_str()), value);
}

void setFloat(Shader & self, const std::string & name, float value)
{
	glUniform1f(glGetUniformLocation(self.ID, name.c_str()), value);
}

void setVec2(Shader & self, const std::string & name, const glm::vec2 & value)
{
	glUniform2fv(glGetUniformLocation(self.ID, name.c_str()), 1, &value[0]);
}

void setVec2(Shader & self, const std::string & name, float x, float y)
{
	glUniform2f(glGetUniformLocation(self.ID, name.c_str()), x, y);
}

void setVec3(Shader & self, const std::string & name, const glm::vec3 & value)
{
	glUniform3fv(glGetUniformLocation(self.ID, name.c_str()), 1, &value[0]);
}

void setVec3(Shader & self, const std::string & name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(self.ID, name.c_str()), x, y, z);
}

void setVec4(Shader & self, const std::string & name, const glm::vec4 & value)
{
	glUniform4fv(glGetUniformLocation(self.ID, name.c_str()), 1, &value[0]);
}

void setVec4(Shader & self, const std::string & name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(self.ID, name.c_str()), x, y, z, w);
}

void setMat2(Shader & self, const std::string & name, const glm::mat2 & mat)
{
	glUniformMatrix2fv(glGetUniformLocation(self.ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void setMat3(Shader & self, const std::string & name, const glm::mat3 & mat)
{
	glUniformMatrix3fv(glGetUniformLocation(self.ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void setMat4(Shader & self, const std::string & name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(glGetUniformLocation(self.ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
