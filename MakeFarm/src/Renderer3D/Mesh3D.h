#pragma once
#include <vector>

struct Mesh3D
{
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> textureCoordinates;
	std::vector<GLuint> indices;
};
