#include "IndexBuffer.h"
#include "pch.h"

#include "Renderer3D.h"

IndexBuffer::IndexBuffer()
    : mCount(0)
{
    GLCall(glGenBuffers(1, &mBufferId));
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned count)
    : mCount(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(1, &mBufferId));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &mBufferId));
}

void IndexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId));
}

void IndexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::setData(const unsigned int* data, unsigned count)
{
    this->mCount = count;
    bind();
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

unsigned IndexBuffer::size() const
{
    return mCount;
}
