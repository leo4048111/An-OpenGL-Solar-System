#pragma once

#include "Headers.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"

class Renderer
{
	NONCOPYABLE(Renderer)

public:
	~Renderer() = default;

private:
	Renderer() = default;

public:
	static Renderer* getInstance() {
		if (_inst.get() == nullptr) _inst.reset(new Renderer);
		return _inst.get();
	}

	void draw(const VertexArray& va, const Shader& shader, const GLenum polygonMode, const GLenum elementMode) const;

private:
	static std::unique_ptr<Renderer> _inst;
};

std::unique_ptr<Renderer> Renderer::_inst;

// TODO: encapsulate both glDrawElements and glDrawArrays instead of using default glDrawElements
void Renderer::draw(const VertexArray& va, const Shader& shader, const GLenum polygonMode, const GLenum elementMode) const
{
	va.bind();
	shader.enable();
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, polygonMode));
	GLCall(glDrawElements(elementMode, va.count(), GL_UNSIGNED_INT, nullptr));
	va.unbind();
	shader.disable();
}