#pragma once


class Buffer
{
public:
	Buffer() = default;
	
	Buffer(const Buffer&) = delete;
	Buffer(Buffer&&) noexcept;

	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&&) noexcept;
	
	virtual ~Buffer() = default;
	
	virtual void bind() const = 0;
	virtual void unbind() const = 0;

protected:
	unsigned int bufferId;
};

inline Buffer::Buffer(Buffer&& rhs) noexcept
{
	bufferId = std::move(rhs.bufferId);
	rhs.bufferId = 0;
}

inline Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
	bufferId = std::move(rhs.bufferId);
	rhs.bufferId = 0;
}
