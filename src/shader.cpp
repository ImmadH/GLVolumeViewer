#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 1: Retrieve the shader code from file path
	std::string vertexCode;
	std::string fragmentCode;

	try
	{
		vertexCode = readShaderFile(vertexPath);
		fragmentCode = readShaderFile(fragmentPath);
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n" << e.what() << std::endl;
		return;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. Compile shaders
	uint32_t vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
	uint32_t fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompilationErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use() const
{
	glUseProgram(ID);
}
//UNIFORM UTILITIES
void Shader::setBool(const std::string name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.data()), (int)value);
}
void Shader::setInt(const std::string name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.data()), value);
}
void Shader::setFloat(const std::string name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.data()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	GLint loc = glGetUniformLocation(ID, name.c_str());
	glUniform2f(loc, x, y);
}

std::string Shader::readShaderFile(const std::string& path)
{
	std::ifstream shaderFile(path);
	if (!shaderFile)
	{
		throw std::runtime_error("Failed to open shader file: " + path);
	}

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	return shaderStream.str();
}

uint32_t Shader::compileShader(uint32_t shaderType, const char* shaderCode)
{
	uint32_t shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);
	checkCompilationErrors(shader, (shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");
	return shader;
}

void Shader::checkCompilationErrors(uint32_t shader, const std::string type) const
{
	int success;
	char infoLog[1024];

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION (" << type << ")\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING (" << type << ")\n" << infoLog << std::endl;
		}
	}
}