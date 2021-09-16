#include "pch.h"
#include "IndexBuffer.h"

#include "Renderer3D.h"

IndexBuffer::IndexBuffer()
	: count(0)
{
    GLCall(glGenBuffers(1, &bufferId));
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned count)
	: count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));
	
    GLCall(glGenBuffers(1, &bufferId));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &bufferId));
}

void IndexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId));
}

void IndexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::setData(const unsigned int* data, unsigned count)
{
    this->count = count;
    bind();
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

unsigned IndexBuffer::getCount() const
{
	return count;
}
