#pragma once
#include "BufferLayout.h"
#include "IndexBuffer.h"
#include "Mesh3D.h"
#include "Renderer3D.h"
#include "VertexArray.h"
#include "VertexBuffer.h"


/**
 * 3D model consisting of a mesh, which allows to directly draw on the screen.
 */
class Model3D
{
public:
	Model3D();
	Model3D(Model3D&&) noexcept = default;
	
	/**
	 * \brief This function expects that mesh data
	 * will be available through all Model3D existance
	 * \param mesh Mesh to display
	 */
	void setMesh(const Mesh3D& mesh);

	/**
	 * \brief This function stores data of the mesh
	 * \param mesh Mesh to display
	 */
	void setMesh(Mesh3D&& mesh);

    /**
     * Draws this 3D Model to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

private:
	VertexArray mVertexArray;
	BufferLayout mBufferLayout;
	IndexBuffer mIndices;
	std::vector<VertexBuffer> mVertexBuffers;
	std::unique_ptr<Mesh3D> mMesh;
};
