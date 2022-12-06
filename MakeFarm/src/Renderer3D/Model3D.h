#pragma once
#include "BufferLayout.h"
#include "IndexBuffer.h"
#include "Renderer3D.h"
#include "Renderer3D/Meshes/Mesh3D.h"
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
    void setMesh(std::unique_ptr<Mesh3D> mesh);

    /**
     * Draws this 3D Model to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

    /**
     * Draws this 3D Model to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer3D& renderer3d, const sf::Shader& shader,
              const Renderer3D::DrawMode& drawMode) const;

    /**
     * @brief Sets the given buffer layout for this model which represents the arrangement of data
     * in memory.
     * @param bufferLayout BufferLayout which represents the arrangement of data.
     */
    void setLayout(const BufferLayout& bufferLayout);

private:
    VertexArray mVertexArray;
    BufferLayout mBufferLayout;
    IndexBuffer mIndices;
    std::vector<VertexBuffer> mVertexBuffers;
    std::unique_ptr<Mesh3D> mMesh;
};