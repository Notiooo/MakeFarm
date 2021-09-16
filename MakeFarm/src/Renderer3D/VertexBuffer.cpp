#include "pch.h"
#include "VertexBuffer.h"

#include "Renderer3D/Renderer3D.h"

VertexBuffer::VertexBuffer(const void* data, unsigned size)
{
    GLCall(glGenBuffers(1, &bufferId));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferId));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &bufferId));
}

void VertexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferId));
}

void VertexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
