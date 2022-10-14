#pragma once

#include "Headers.hpp"

typedef struct
{
	GLenum type;
	const unsigned int count;
	const bool normalized;
}LayoutElement;

class BufferLayout
{
private:
	unsigned int _stride{ 0 };
	std::vector<LayoutElement> _elements;

public:
	BufferLayout() = default;
	~BufferLayout() = default;

	void push(GLenum type, const unsigned int count, const bool normalized);

	const std::vector<LayoutElement> elements() const { return _elements; };

	const unsigned int stride() const { return _stride; };

	static inline unsigned int getTypeSize(const GLenum type);
};

void BufferLayout::push(GLenum type, const unsigned int count, const bool normalized)
{
	LayoutElement element = { type, count, normalized };
	_elements.push_back(element);
	_stride += count * getTypeSize(type);
}

unsigned int BufferLayout::getTypeSize(const GLenum type)
{
	switch (type)
	{
	case GL_FLOAT: return sizeof(float);
	case GL_UNSIGNED_INT: return sizeof(unsigned int);
	default:
		ASSERT(GL_FALSE);
	}

	return NULL;
}