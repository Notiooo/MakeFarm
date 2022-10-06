#include "Buffer.h"
#include "pch.h"

Buffer::Buffer(Buffer&& rhs) noexcept
{
    mBufferId = rhs.mBufferId;
    rhs.mBufferId = 0;
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
    mBufferId = rhs.mBufferId;
    rhs.mBufferId = 0;
    return *this;
}