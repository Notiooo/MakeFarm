#include "Renderer3D.h"
#include "pch.h"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
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

void SfmlDraw(const sf::Drawable& drawable, sf::RenderTarget& target, const sf::RenderStates states)
{
    target.pushGLStates();
    target.draw(drawable, states);
    target.popGLStates();
}

void Renderer3D::draw(const VertexArray& va, const IndexBuffer& ib, const sf::Shader& shader,
                      const DrawMode& drawMode) const
{
    sf::Shader::bind(&shader);
    va.bind();
    ib.bind();
    GLCall(glDrawElements(toOpenGL(drawMode), ib.size(), GL_UNSIGNED_INT, nullptr));

#ifdef _DEBUG
    sf::Shader::bind(nullptr);
    va.unbind();
    ib.unbind();
#endif
}

void Renderer3D::draw(const VertexArray& va, const sf::Shader& shader, int number,
                      const DrawMode& drawMode) const
{
    sf::Shader::bind(&shader);
    va.bind();
    GLCall(glDrawArrays(toOpenGL(drawMode), 0, number));

#ifdef _DEBUG
    sf::Shader::bind(nullptr);
    va.unbind();
#endif
}

unsigned Renderer3D::toOpenGL(const Renderer3D::DrawMode& drawMode) const
{
    switch (drawMode)
    {
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::Quads: return GL_QUADS;
        default: throw std::runtime_error("This DrawMode is not supported!");
    }
}
