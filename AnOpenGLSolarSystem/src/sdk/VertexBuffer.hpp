#pragma once

#include "Headers.hpp"

class VertexBuffer
{
	NONCOPYABLE(VertexBuffer)

private:
	GLuint _id;

public:
	VertexBuffer(const void* data, const size_t size);
	~VertexBuffer();
	VertexBuffer(VertexBuffer&& vb) noexcept:
		_id(vb._id) {
		vb._id = 0;
	};

public:
	void bind() const;
	void unbind() const;
};

VertexBuffer::VertexBuffer(const void* data, const size_t size)
{
	GLCall(glGenBuffers(1, &_id));
	this->bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	this->unbind();
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &_id))
}

void VertexBuffer::bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, _id));
}

void VertexBuffer::unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}