#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Shader
{
	uint32_t ID;

	// constructor 
	Shader(const char* vertexPath, const char* fragmentPath);
	std::string readShaderFile(const std::string& path);
	uint32_t compileShader(uint32_t shaderType, const char* shaderCode);

	// activate shader
	void use() const;

	// utility 
	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setVec2(const std::string& name, float x, float y) const;
	// error checking
	void checkCompilationErrors(uint32_t shader, const std::string type) const;

	~Shader();
};