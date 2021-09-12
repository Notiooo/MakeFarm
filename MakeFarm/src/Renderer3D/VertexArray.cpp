#include "pch.h"
#include "VertexArray.h"

#include "Renderer3D/BufferLayout.h"
#include "Renderer3D/Renderer3D.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &vertexArrayId));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &vertexArrayId));
}

void VertexArray::bind() const
{
	GLCall(glBindVertexArray(vertexArrayId));
}

void VertexArray::unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::setBuffer(const VertexBuffer& vb, const BufferLayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	unsigned int offset = 0;
	for(unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), reinterpret_cast<const void*>(offset)));
		offset += element.count * BufferElement::getSizeOfType(element.type);
	}
}
