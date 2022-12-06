#include "WorldBlockMesh.h"

std::vector<VertexBuffer> WorldBlockMesh::vertexBuffer()
{
    std::vector<VertexBuffer> vb;
    vb.emplace_back(vertices);
    vb.emplace_back(textureCoordinates);
    vb.emplace_back(directionalLightning);
    return vb;
}

void WorldBlockMesh::reset()
{
    vertices.clear();
    textureCoordinates.clear();
    directionalLightning.clear();
    indices.clear();
}

BufferLayout WorldBlockMesh::bufferLayout()
{
    BufferLayout bl;
    bl.push<float>(3);
    bl.push<float>(2);
    bl.push<float>(1);
    return bl;
}

std::unique_ptr<Mesh3D> WorldBlockMesh::clone()
{
    return std::make_unique<WorldBlockMesh>(*this);
}
