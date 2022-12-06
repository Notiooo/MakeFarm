#pragma once

#include "Renderer3D/Meshes/Mesh3D.h"
#include "World/Block/Block.h"

class Chunk;
class AABB;

/**
 * It can generate a mesh, which can then be projected onto the screen as a 3D object.
 */
class MeshBuilder
{
public:
    explicit MeshBuilder(Block::Coordinate origin);
    MeshBuilder();
    MeshBuilder(MeshBuilder&&) noexcept = default;
    virtual ~MeshBuilder() = default;

    /**
     * Resets the state of the mesh and returns to the initial values
     */
    virtual void resetMesh() = 0;

    /**
     * Returns the 3d mesh created
     * @return The created 3D mesh
     */
    [[nodiscard]] virtual std::unique_ptr<Mesh3D> mesh3D() = 0;

protected:
    GLuint mIndex = 0;
    Block::Coordinate mOrigin;
};
