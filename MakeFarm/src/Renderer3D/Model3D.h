#pragma once
#include "BufferLayout.h"
#include "IndexBuffer.h"
#include "Mesh3D.h"
#include "Renderer3D.h"
#include "VertexArray.h"
#include "VertexBuffer.h"


class Model3D
{
public:

	Model3D();
	
	/**
	 * \brief This function expects that mesh data
	 * will be available through all Model3D existance
	 * \param mesh 
	 */
	void setMesh(const Mesh3D& mesh);
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

private:
	VertexArray vertexArray;
	BufferLayout bufferLayout;
	IndexBuffer indices;
	std::vector<VertexBuffer> vertexBuffers;
};
