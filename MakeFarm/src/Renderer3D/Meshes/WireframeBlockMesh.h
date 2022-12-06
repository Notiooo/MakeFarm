#pragma once

#include "Renderer3D/Meshes/Mesh3D.h"
#include <memory>

/**
 * @brief Mesh being the wireframe representation of the block. That is, a block consisting of just
 * vertices without textures.
 */
struct WireframeBlockMesh : public Mesh3D
{
    /**
     * @brief Returns the Vertex Buffer that creates the mesh
     * @return Vertex Buffer that creates the mesh
     */
    std::vector<VertexBuffer> vertexBuffer() override;

    /**
     * @brief Returns BufferLayout, which determines the memory layout of a given mesh
     * @return BufferLayout, which determines the memory layout of a given mesh
     */
    BufferLayout bufferLayout() override;

    /**
     * @brief Resets mesh to initial values (clears it).
     */
    void reset() override;

    /**
     * @brief Copies the mesh and returns a pointer to its copy
     * @return Pointer to mesh copy
     */
    std::unique_ptr<Mesh3D> clone() override;

    /* ==== Members ===== */
    std::vector<float> vertices;
};