#pragma once
#include "Buffer.h"

class VertexBuffer : public Buffer
{
public:
	VertexBuffer(const void* data, unsigned int size);

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&) noexcept = default;

	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer& operator=(VertexBuffer&&) noexcept = default;
	
	~VertexBuffer() override;

	void bind() const override;
	void unbind() const override;
};
