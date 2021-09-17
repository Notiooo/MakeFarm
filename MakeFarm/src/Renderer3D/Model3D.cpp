#include "pch.h"
#include "Model3D.h"

Model3D::Model3D()
{
	bufferLayout.push<GLfloat>(3);
	bufferLayout.push<GLfloat>(2);
}

void Model3D::setMesh(const Mesh3D& mesh)
{
	vertexBuffers.emplace_back(mesh.vertices.data(), mesh.vertices.size() * sizeof(mesh.vertices[0]));
	vertexBuffers.emplace_back(mesh.textureCoordinates.data(), mesh.textureCoordinates.size() * sizeof(mesh.textureCoordinates[0]));
	vertexArray.setBuffer(vertexBuffers, bufferLayout);
	
	indices.setData(mesh.indices.data(), static_cast<unsigned int>(mesh.indices.size()));

	#ifdef _DEBUG
	vertexArray.unbind();
	indices.unbind();
	#endif
}

void Model3D::setMesh(Mesh3D&& mesh)
{
	this->mesh = std::make_unique<Mesh3D>(std::move(mesh));
	setMesh(*this->mesh);
}

void Model3D::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	renderer3d.draw(vertexArray, indices, shader);
}
