#pragma once
#include <vector>

struct BufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int getSizeOfType(unsigned int type);
};


class BufferLayout
{
public:
	BufferLayout();

	template <typename T>
	void push(unsigned int count)
	{
		//static_assert(false);
	}

	template<>
	void push<float>(unsigned int count)
	{
		bufferElements.push_back({GL_FLOAT, count, GL_FALSE });
		stride += BufferElement::getSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void push<unsigned int>(unsigned int count)
	{
		bufferElements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		stride += BufferElement::getSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void push<unsigned char>(unsigned int count)
	{
		bufferElements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		stride += BufferElement::getSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	const std::vector<BufferElement> getElements() const;
	unsigned int getStride() const;

private:
	std::vector<BufferElement> bufferElements;
	unsigned int stride;
};