#pragma once

#include "Renderer3D/Meshes/Mesh3D.h"
#include <memory>

/**
 * @brief This is the mesh of a typical block located inside the game. Such a block consists of
 * vertices, textures and shadows on each block face.
 */
struct WorldBlockMesh : public Mesh3D
{
    /**
     * @brief Returns the Vertex Buffer that creates the mesh
     * @return Vertex Buffer that creates the mesh
     */
    std::vector<VertexBuffer> vertexBuffer() override;

    /**
     * @brief Resets mesh to initial values (clears it).
     */
    void reset() override;

    /**
     * @brief Returns BufferLayout, which determines the memory layout of a given mesh
     * @return BufferLayout, which determines the memory layout of a given mesh
     */
    BufferLayout bufferLayout() override;

    /**
     * @brief Copies the mesh and returns a pointer to its copy
     * @return Pointer to mesh copy
     */
    std::unique_ptr<Mesh3D> clone() override;

    /* ==== Members ===== */
    std::vector<float> vertices;
    std::vector<float> textureCoordinates;
    std::vector<float> directionalLightning;
};