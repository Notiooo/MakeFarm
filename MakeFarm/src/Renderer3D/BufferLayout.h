#pragma once
#include <vector>
#include "Renderer3D/BufferElement.h"

/**
 * Represents the data layout for a given buffer.
 */
class BufferLayout
{
public:
	BufferLayout();

    /**
     * Adds information that the layout consists of the given number of elements of type T.
     * @tparam T Parameters other than float, unsigned int and unsigned char are not supported.
     * @param count Number of elements of a given type per row.
     */
	template <typename T>
	void push(unsigned int count)
	{
		//static_assert(false);
	}

    /**
     * Adds information that the layout consists of the given number of elements of type float.
     * @param count Number of elements of a float type per row.
     */
	template<>
	void push<float>(unsigned int count)
	{
		mBufferElements.push_back({GL_FLOAT, count, GL_FALSE });
        mStride += BufferElement::sizeOfGLType(GL_FLOAT) * count;
	}

    /**
     * Adds information that the layout consists of the given number of elements of type unsigned int.
     * @param count Number of elements of a unsigned int type per row.
     */
	template<>
	void push<unsigned int>(unsigned int count)
	{
		mBufferElements.push_back({GL_UNSIGNED_INT, count, GL_FALSE });
        mStride += BufferElement::sizeOfGLType(GL_UNSIGNED_INT) * count;
	}

    /**
     * Adds information that the layout consists of the given number of elements of type unsigned char.
     * @param count Number of elements of a unsigned char type per row.
     */
	template<>
	void push<unsigned char>(unsigned int count)
	{
		mBufferElements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE });
        mStride += BufferElement::sizeOfGLType(GL_UNSIGNED_BYTE) * count;
	}

    /**
     * Contains all buffer elements
     * @return A vector of many single buffer elements
     */
	[[nodiscard]] const std::vector<BufferElement> bufferElements() const;

    /**
     * Determines the size that one 'entry', or otherwise 'row' of data contains before the next set of data begins.
     * @return Size of the stride
     */
	[[nodiscard]] unsigned int stride() const;

private:
	std::vector<BufferElement> mBufferElements;
	unsigned int mStride;
};