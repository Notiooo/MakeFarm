#include "Renderer3D.h"
#include "pch.h"

#include <SFML/Graphics/Shader.hpp>

void GLClearError()
{
    while (glGetError() /* != GL_NO_ERROR*/)
    {
        ;
    }
}

bool GLLogCall(const char* function, const char* file, int line)
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec << "): " << function
                  << " " << file << " at line: " << line << std::endl;
        return false;
    }
    return true;
}

void Renderer3D::draw(const VertexArray& va, const IndexBuffer& ib, const sf::Shader& shader,
                      const DrawMode& drawMode) const
{
    sf::Shader::bind(&shader);
    va.bind();
    ib.bind();
    GLCall(glDrawElements(toOpenGL(drawMode), ib.size(), GL_UNSIGNED_INT, nullptr));

    // clang-format off
    #ifdef _DEBUG
    sf::Shader::bind(nullptr);
    va.unbind();
    ib.unbind();
    #endif
    // clang-format on
}

void Renderer3D::draw(const VertexArray& va, const sf::Shader& shader, int number,
                      const DrawMode& drawMode) const
{
    sf::Shader::bind(&shader);
    va.bind();
    GLCall(glDrawArrays(toOpenGL(drawMode), 0, number));

    // clang-format off
    #ifdef _DEBUG
    sf::Shader::bind(nullptr);
    va.unbind();
    #endif
    // clang-format on
}

unsigned Renderer3D::toOpenGL(const Renderer3D::DrawMode& drawMode) const
{
    switch (drawMode)
    {
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::Quads: return GL_QUADS;
    }
}
