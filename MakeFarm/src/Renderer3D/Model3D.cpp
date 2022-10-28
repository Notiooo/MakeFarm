#include "Model3D.h"
#include "pch.h"

Model3D::Model3D()
{
    mBufferLayout.push<GLfloat>(3);
    mBufferLayout.push<GLfloat>(2);
}

void Model3D::setMesh(const Mesh3D& mesh)
{
    mVertexBuffers.clear();

    mVertexBuffers.emplace_back(mesh.vertices.data(),
                                mesh.vertices.size() * sizeof(mesh.vertices[0]));
    mVertexBuffers.emplace_back(mesh.textureCoordinates.data(),
                                mesh.textureCoordinates.size() *
                                    sizeof(mesh.textureCoordinates[0]));

    // TODO: I think I should use there somehow the vertexBuffer not inside vector, maybe?
    mVertexArray.setBuffer(mVertexBuffers, mBufferLayout);

    mIndices.setData(mesh.indices.data(), static_cast<unsigned int>(mesh.indices.size()));

// clang-format off
    #ifdef _DEBUG
    mVertexArray.unbind();
    mIndices.unbind();
    #endif
    // clang-format on
}

void Model3D::setMesh(Mesh3D&& mesh)
{
    mMesh = std::make_unique<Mesh3D>(std::move(mesh));
    setMesh(*mMesh);
}

void Model3D::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    renderer3d.draw(mVertexArray, mIndices, shader);
}
