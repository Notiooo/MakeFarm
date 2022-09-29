#include "pch.h"
#include "VertexArray.h"

#include "Renderer3D/BufferLayout.h"
#include "Renderer3D/Renderer3D.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &mBufferId));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &mBufferId));
}

void VertexArray::bind() const
{
	GLCall(glBindVertexArray(mBufferId));
}

void VertexArray::unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::setBuffer(const std::vector<VertexBuffer>& vb, const BufferLayout& layout)
{
	const auto& elements = layout.bufferElements();
	
	if (vb.size() != elements.size())
    {
        throw std::runtime_error("Given vertex buffer, and buffer layout are incompatible!");
    }

	bind();
    for (unsigned int i = 0; i < vb.size(); ++i)
	{
		vb[i].bind();
		const auto& element = elements[i];
		const auto& elementStride = element.count * BufferElement::sizeOfGLType(element.type);
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, elementStride, reinterpret_cast<const void*>(0)));
	}
}

void VertexArray::setBuffer(const VertexBuffer& vb, const BufferLayout& layout)
{
	bind();
	vb.bind();

    const auto& elements = layout.bufferElements();
	unsigned int offset = 0;
	for(unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride(), reinterpret_cast<const void*>(offset)));
		offset += element.count * BufferElement::sizeOfGLType(element.type);
	}
}
