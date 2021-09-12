#pragma once

#include "Renderer3D/VertexBuffer.h"

class BufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;
	void setBuffer(const VertexBuffer& vb, const BufferLayout& layout);

private:
	unsigned int vertexArrayId;
};