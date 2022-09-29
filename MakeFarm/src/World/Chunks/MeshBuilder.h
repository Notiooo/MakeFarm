#pragma once

#include <mutex>
#include "Renderer3D/Mesh3D.h"
#include "World/Block/Block.h"


class Chunk;

/**
 * It can generate a mesh, which can then be projected onto the screen as a 3D object.
 */
class MeshBuilder
{
public:
	explicit MeshBuilder(const Block::Coordinate& origin);
	MeshBuilder(MeshBuilder&&) noexcept = default;

    /**
     * Sets the size of the face to be created
     * @param faceSize Block face size
     */
	void setFaceSize(const float& faceSize);

    /**
     * Adds a quad to the mesh in place of the designated face at the given coordinates and with the given quad texture.
     * @param blockFace A block face to add
     * @param textureQuad Position in the texture pack of the texture to be displayed
     * @param blockPosition The position on which the quad will be added
     */
	void addQuad(const Block::Face& blockFace,
		const std::vector<GLfloat>& textureQuad,
		const Block::Coordinate& blockPosition);

    /**
     * Resets the state of the mesh and returns to the initial values
     */
	void resetMesh();

    /**
     * Blocks mesh from rebuilding
     */
    void blockMesh() const;

    /**
     * Unblocks mesh from rebuilding
     */
    void unblockMesh() const;

    /**
     * Returns the 3d mesh created
     * @return The created 3D mesh
     */
	[[nodiscard]] Mesh3D mesh3D() const;

private:
    /**
     * Returns the vertices for a given block face
     * @param blockFace The face of the block
     * @return The vertices for a given block face
     */
	[[nodiscard]] std::vector<GLfloat> faceVertices(const Block::Face& blockFace) const;

private:
	GLuint mIndex = 0;
	const Block::Coordinate& mOrigin;
    mutable std::recursive_mutex mRebuildMeshMutex;
	Mesh3D mChunkMesh;
	float mBlockFaceSize = Block::BLOCK_SIZE;
};
