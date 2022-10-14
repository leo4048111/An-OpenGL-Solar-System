#pragma once

#include "Headers.hpp"
#include "VertexBuffer.hpp"
#include "BufferLayout.hpp"
#include "IndexBuffer.hpp"

class VertexArray
{
	NONCOPYABLE(VertexArray)

private:
	GLuint _id;
	VertexBuffer _vbo;
	IndexBuffer _ibo;

public:
	VertexArray(VertexBuffer& vbo, IndexBuffer& ibo, const BufferLayout& layout);
	~VertexArray();
	VertexArray(VertexArray&& va) noexcept :
		_id(va._id), _vbo(std::move(va._vbo)), _ibo(std::move(va._ibo)) {
		va._id = 0;
	}

public:
	void bind() const;
	void unbind() const;
	const unsigned int count() const { return _ibo.count(); };
};

VertexArray::VertexArray(VertexBuffer& vbo, IndexBuffer& ibo, const BufferLayout& layout) :
	_vbo(std::move(vbo)), _ibo(std::move(ibo))
{
	GLCall(glGenVertexArrays(1, &_id));
	this->bind();
	_vbo.bind();
	_ibo.bind();
	auto& elements = layout.elements();
	unsigned int offset = 0;
	for (int i = 0; i < elements.size(); i++)
	{
		auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride(), (const void*)offset));
		offset += element.count * BufferLayout::getTypeSize(element.type);
	}
	this->unbind();
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &_id));
}

void VertexArray::bind() const
{
	GLCall(glBindVertexArray(_id));
}

void VertexArray::unbind() const
{
	GLCall(glBindVertexArray(0));
}