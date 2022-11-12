#pragma once

#include "Renderer3D/IndexBuffer.h"
#include "Renderer3D/VertexArray.h"

/**
 * Activates breakpoint
 */
#ifdef _MSC_VER
    #define ASSERT(x)                                                                              \
        if (!x)                                                                                    \
            __debugbreak();
#else
    #define ASSERT(x)                                                                              \
        if (!x)                                                                                    \
            raise(SIGTRAP);
#endif

/**
 * Macro to use on OpenGL functions to correctly catch errors and write them out
 */
#define GLCall(x)                                                                                  \
    GLClearError();                                                                                \
    x;                                                                                             \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

/**
 * Cleans up OpenGL errors
 */
void GLClearError();

/**
 * Prints an error if an OpenGL error occurred
 * @param function The function in which the error occurred
 * @param file The name of the file in which the error occurred
 * @param line The line where the error occurred
 * @return False if the error occurred, true otherwise
 */
bool GLLogCall(const char* function, const char* file, int line);

/**
 * 3D Renderer allows to draw 3D objects to the screen using appropriate buffers and shaders
 */
class Renderer3D
{
public:
    enum class DrawMode
    {
        Lines,
        Triangles,
        Quads
    };

    /**
     * Draws the data given in VertexArray, IndexBuffer to the screen using the interpretation given
     * in Shader.
     * @param va Stores all Vertex Data.
     * @param ib Specifies the drawing order of the VertexArray.
     * @param shader Shader telling how to draw data.
     */
    void draw(const VertexArray& va, const IndexBuffer& ib, const sf::Shader& shader,
              const DrawMode& drawMode = DrawMode::Triangles) const;

    /**
     * Draws the data given in VertexArray to the screen using the interpretation given in Shader.
     * @param va Stores all Vertex Data.
     * @param shader Shader telling how to draw data.
     * @param number VertexArray size
     */
    void draw(const VertexArray& va, const sf::Shader& shader, int number,
              const DrawMode& drawMode = DrawMode::Triangles) const;

private:
    unsigned toOpenGL(const DrawMode& drawMode) const;
};