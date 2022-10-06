#pragma once
#include <vector>

/**
 * Mesh3D consists of vertices, texture coordinates in the texturepack
 * and indices to generate a nice 3d mesh, which can then be used in the 3D Model to draw it onto
 * the screen.
 */
struct Mesh3D
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> textureCoordinates;
    std::vector<GLuint> indices;
};
