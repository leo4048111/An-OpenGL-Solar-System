#pragma once

#include "Headers.hpp"

class IndexBuffer
{
	NONCOPYABLE(IndexBuffer)

private:
	GLuint _id;
	const unsigned int _count;

public:
	IndexBuffer(const void* data, const unsigned int count);
	~IndexBuffer();
	IndexBuffer(IndexBuffer&& ib) noexcept :
		_id(ib._id), _count(ib.count()) {
		ib._id = 0;
	};

public:
	void bind() const;
	void unbind() const;
	const unsigned int count() const { return _count; };
};

IndexBuffer::IndexBuffer(const void* data, const unsigned int count) :
	_count(count)
{
	glGenBuffers(1, &_id);
	this->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	this->unbind();
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &_id))
}

void IndexBuffer::bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id));
}

void IndexBuffer::unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
