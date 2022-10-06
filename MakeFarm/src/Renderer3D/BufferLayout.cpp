#include "BufferLayout.h"
#include "Renderer3D/Renderer3D.h"
#include "pch.h"

unsigned BufferElement::sizeOfGLType(unsigned type)
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

BufferLayout::BufferLayout()
    : mStride(0)
{
}

const std::vector<BufferElement> BufferLayout::bufferElements() const
{
    return mBufferElements;
}

unsigned BufferLayout::stride() const
{
    return mStride;
}
