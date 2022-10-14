#pragma once

#include "Headers.hpp"

class Shader
{
	NONCOPYABLE(Shader)

private:
	GLuint _id;
	std::unordered_map<std::string, int> _umap;

public:
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();
	Shader(Shader&& shader) noexcept:
		_id(shader._id), _umap(shader._umap) {
		shader._id = 0;
	}

public:
	void enable() const;
	void disable() const;

	// uniform setters
	void uniformMatrix4fv(const std::string& name, glm::mat4 mat);
	void uniform4fv(const std::string& name, glm::vec4 vec);
	void uniform3fv(const std::string& name, glm::vec3 vec);
	void uniform1i(const std::string& name, GLint value);

private:
	int getUniformLocation(const std::string& name);
	std::string getShaderSource(std::string path) const;
	GLuint compileShader(GLenum type, const std::string& source) const;
};

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	_id = glCreateProgram();
	std::string vsSource = getShaderSource(vertexShaderPath);
	GLuint vs = compileShader(GL_VERTEX_SHADER, vsSource);
	std::string fsSource = getShaderSource(fragmentShaderPath);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
	GLCall(glAttachShader(_id, vs));
	GLCall(glAttachShader(_id, fs));
	GLCall(glLinkProgram(_id));
	GLCall(glValidateProgram(_id));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(_id));
}

std::string Shader::getShaderSource(std::string path) const
{
	std::ifstream ifs(path);
	std::string line;
	std::stringstream ss;
	while (std::getline(ifs, line)) ss << line << '\n';
	return ss.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& source) const
{
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(shader, 1, &src, nullptr));
	GLCall(glCompileShader(shader));

	int result;
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		printf("Failed to compile shader, reason: \n%s\n", message);
		GLCall(glDeleteShader(shader));
		return 0;
	}

	return shader;
}

void Shader::enable() const
{
	GLCall(glUseProgram(_id));
}

void Shader::disable() const
{
	GLCall(glUseProgram(0));
}

int Shader::getUniformLocation(const std::string& name)
{
	if (_umap.find(name) != _umap.end())
	{
		return _umap[name];
	}

	int pos = glGetUniformLocation(_id, name.c_str());
	if (pos == -1) printf("Uniform %s doesn't exist!\n", name.c_str());
	_umap.insert(make_pair(name, pos));
	return pos;
}

void Shader::uniformMatrix4fv(const std::string& name, glm::mat4 mat)
{
	this->enable();
	GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]));
	this->disable();
}

void Shader::uniform4fv(const std::string& name, glm::vec4 vec)
{
	this->enable();
	GLCall(glUniform4fv(getUniformLocation(name), 1, &vec.x));
	this->disable();
}

void Shader::uniform3fv(const std::string& name, glm::vec3 vec)
{
	this->enable();
	GLCall(glUniform3fv(getUniformLocation(name), 1, &vec.x));
	this->disable();
}

void Shader::uniform1i(const std::string& name, GLint value)
{
	this->enable();
	GLCall(glUniform1i(getUniformLocation(name), value));
	this->disable();
}
