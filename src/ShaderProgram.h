#pragma once

#define GL_CHECK_ERRORS {GLenum err = glGetError(); if (err != GL_NO_ERROR) {const char *s = (const char *)gluErrorString(err); printf("GL ERR: %s",  s);} assert(err == GL_NO_ERROR);}

struct Shader
{
	unsigned int ID;

	const char* vs_code;
	const char* ps_code;
};

void ShaderCreateProgram(Shader& self);
void ShaderUseProgram(Shader& self);

// Utility functions
void setBool(Shader & self, const std::string &name, bool value);
void setInt(Shader & self, const std::string &name, int value);
void setFloat(Shader & self, const std::string &name, float value);

void setVec2(Shader & self, const std::string &name, const glm::vec2 &value);
void setVec2(Shader & self, const std::string &name, float x, float y);
void setVec3(Shader & self, const std::string &name, const glm::vec3 &value);
void setVec3(Shader & self, const std::string &name, float x, float y, float z);
void setVec4(Shader & self, const std::string &name, const glm::vec4 &value);
void setVec4(Shader & self, const std::string &name, float x, float y, float z, float w);

void setMat2(Shader & self, const std::string &name, const glm::mat2 &mat);
void setMat3(Shader & self, const std::string &name, const glm::mat3 &mat);
void setMat4(Shader & self, const std::string &name, const glm::mat4 &mat);
