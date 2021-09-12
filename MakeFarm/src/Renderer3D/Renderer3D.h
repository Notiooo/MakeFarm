#pragma once

#include "Renderer3D/IndexBuffer.h"
#include "Renderer3D/VertexArray.h"

#define ASSERT(x) if(!x) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);


class Renderer3D
{
public:
	void draw(const VertexArray& va, const IndexBuffer& ib, const sf::Shader& shader) const;
	void draw(const VertexArray& va, const sf::Shader& shader, int number) const;
};