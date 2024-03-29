#pragma once
#include "Buffer.h"

/**
 * Vertex buffer object (VBO) allows vertex array data to be stored
 * in high-performance graphics memory.
 */
class VertexBuffer : public Buffer
{
public:
    VertexBuffer(const void* data, unsigned int size);

    template<typename T>
    VertexBuffer(const std::vector<T>& vector);

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) noexcept = default;

    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

    ~VertexBuffer() override;

    /**
     * Binds a buffer object to the specific (GL_ARRAY_BUFFER) buffer binding point
     */
    void bind() const override;

    /**
     * Unbinds a buffer object (GL_ARRAY_BUFFER)
     */
    void unbind() const override;
};

template<typename T>
VertexBuffer::VertexBuffer(const std::vector<T>& vector)
    : VertexBuffer(vector.data(), vector.size() * sizeof(vector[0]))
{
}
