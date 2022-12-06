#include "WireframeBlockMesh.h"

std::vector<VertexBuffer> WireframeBlockMesh::vertexBuffer()
{
    std::vector<VertexBuffer> vb;
    vb.emplace_back(vertices);
    return vb;
}

BufferLayout WireframeBlockMesh::bufferLayout()
{
    BufferLayout bl;
    bl.push<float>(3);
    return bl;
}

void WireframeBlockMesh::reset()
{
    vertices.clear();
    indices.clear();
}

std::unique_ptr<Mesh3D> WireframeBlockMesh::clone()
{
    return std::make_unique<WireframeBlockMesh>(*this);
}
