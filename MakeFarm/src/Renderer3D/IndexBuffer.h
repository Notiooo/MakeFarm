#pragma once
#include "Buffer.h"

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

	void bind() const override;
	void unbind() const override;

	void setData(const unsigned int* data, unsigned count);

	unsigned int getCount() const;

private:
	unsigned int count;
};
