#include "pch.h"
#include "IndexBuffer.h"

#include "Renderer3D.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned count)
	: count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));
	
    GLCall(glGenBuffers(1, &indexBufferId));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &indexBufferId));
}

void IndexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));
}

void IndexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned IndexBuffer::getCount() const
{
	return count;
}
