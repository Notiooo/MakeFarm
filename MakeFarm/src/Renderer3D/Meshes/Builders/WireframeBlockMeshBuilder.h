#pragma once

#include "Physics/AABB.h"
#include "Renderer3D/Meshes/Builders/BlockMeshBuilder.h"
#include "Renderer3D/Meshes/Mesh3D.h"
#include "Renderer3D/Meshes/WireframeBlockMesh.h"

/**
 * It can generate a mesh of block wireframe, which can then be projected onto the screen as a 3D
 * object.
 */
class WireframeBlockMeshBuilder : public BlockMeshBuilder
{
public:
    explicit WireframeBlockMeshBuilder(Block::Coordinate origin);
    WireframeBlockMeshBuilder();

    /**
     * Resets the state of the mesh and returns to the initial values
     */
    virtual void resetMesh() override;

    /**
     * Returns the 3d mesh created
     * @return The created 3D mesh
     */
    [[nodiscard]] virtual std::unique_ptr<Mesh3D> mesh3D() override;

    /**
     * @brief Builds a simple block wireframe mesh without texture.
     * @param wireframeBlock A cube placed in space using two points.
     */
    void addWireframeBlock(const WireframeBlock& wireframeBlock);


private:
    using BlockMeshBuilder::addQuad;

    /**
     * @brief Adds indices of typical block face
     * @param indices Indices to which new indices are to be added
     */
    void addBlockFaceIndices(std::vector<GLuint>& indices);

    /**
     * @brief For a given face block, it adds vertices building it up
     * @param wireframeBlock An object representing a block wireframe having a start point and an
     * end point.
     * @param blockFace Block face ID
     * @param vertices Container with vertices to fill
     */
    void addBlockFaceVertices(const WireframeBlock& wireframeBlock, int blockFace,
                              std::vector<float>& vertices) const;

protected:
    /* ==== Members ===== */
    std::unique_ptr<WireframeBlockMesh> mMesh;
};