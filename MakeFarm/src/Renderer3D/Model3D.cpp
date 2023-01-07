#include "Model3D.h"
#include "pch.h"

Model3D::Model3D()
{
    mBufferLayout.push<GLfloat>(3);
    mBufferLayout.push<GLfloat>(2);
    mBufferLayout.push<GLfloat>(1);
}

void Model3D::setMesh(std::unique_ptr<Mesh3D> mesh)
{
    mVertexBuffers = mesh->vertexBuffer();
    mBufferLayout = mesh->bufferLayout();
    mVertexArray.setBuffer(mVertexBuffers, mBufferLayout);
    mIndices.setData(mesh->indices.data(), static_cast<unsigned int>(mesh->indices.size()));

#ifdef _DEBUG
    mVertexArray.unbind();
    mIndices.unbind();
#endif
}

void Model3D::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    renderer3d.draw(mVertexArray, mIndices, shader);
}

void Model3D::draw(const Renderer3D& renderer3d, const sf::Shader& shader,
                   const Renderer3D::DrawMode& drawMode) const
{
    renderer3d.draw(mVertexArray, mIndices, shader, drawMode);
}

void Model3D::setLayout(const BufferLayout& bufferLayout)
{
    mBufferLayout = bufferLayout;
}