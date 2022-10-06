#pragma once

/*
 * The base class for all kinds of buffers found in OpenGL.
 */
class Buffer
{
public:
    Buffer() = default;

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept;

    virtual ~Buffer() = default;

    /**
     * Binds a buffer object to the specific (depends on derived class) buffer binding point
     */
    virtual void bind() const = 0;

    /**
     * Unbinds a buffer object
     */
    virtual void unbind() const = 0;

protected:
    unsigned int mBufferId;
};
