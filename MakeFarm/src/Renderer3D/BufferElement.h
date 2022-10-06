#pragma once

/**
 * Represents the memory layout for the buffer layout part of a given type and number of elements.
 */
struct BufferElement
{
    unsigned int type;       //!< OpenGL type of given data
    unsigned int count;      //!< Number of elements of given type
    unsigned char normalized;//!< Is the data normalized

    /**
     * Returns the data size of a given OpenGL type.
     * @param type OpenGL Type.
     * @return Size of given OpenGL type.
     */
    static unsigned int sizeOfGLType(unsigned int type);
};