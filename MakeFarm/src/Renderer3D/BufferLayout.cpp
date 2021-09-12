#include "pch.h"
#include "BufferLayout.h"
#include "Renderer3D/Renderer3D.h"

unsigned BufferElement::getSizeOfType(unsigned type)
{
	switch (type)
	{
	case GL_FLOAT: return 4;
	case GL_UNSIGNED_INT: return 4;
	case GL_UNSIGNED_BYTE: return 1;
	}
	ASSERT(false);
	return 0;
}

BufferLayout::BufferLayout(): stride(0)
{
}

const std::vector<BufferElement> BufferLayout::getElements() const
{
	return bufferElements;
}

unsigned BufferLayout::getStride() const
{
	return stride;
}
