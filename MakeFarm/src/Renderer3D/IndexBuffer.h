#pragma once
#include "Buffer.h"

/**
 * A higher level representation of OpenGL IndexBuffer which is an array of pointers into the vertex
 * buffer.
 */
class IndexBuffer : public Buffer
{
public:
    IndexBuffer();
    IndexBuffer(const unsigned int* data, unsigned int count);

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) noexcept = default;

    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

    ~IndexBuffer() override;

    /**
     * Binds a buffer object to the specific (GL_ELEMENT_ARRAY_BUFFER) buffer binding point
     */
    void bind() const override;

    /**
     * Unbinds a buffer object (GL_ELEMENT_ARRAY_BUFFER)
     */
    void unbind() const override;

    /**
     * Sets the data with the specified number of ints and binds the buffer
     * @param data Array of unsigned ints containing buffer indexes
     * @param count Size of the Array
     */
    void setData(const unsigned int* data, unsigned count);

    /**
     * Returns the number of elements in the index buffer
     * @return Size of the index buffer
     */
    unsigned int size() const;

private:
    unsigned int mCount;
};
