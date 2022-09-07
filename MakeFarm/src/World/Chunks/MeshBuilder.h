#pragma once

#include <mutex>
#include "Renderer3D/Mesh3D.h"
#include "World/Block/Block.h"


class Chunk;

class MeshBuilder
{
public:
	explicit MeshBuilder(const Block::Coordinate& origin);
	MeshBuilder(MeshBuilder&&) noexcept = default;

	/* TODO: Consider changing this setter to something else
	 * Maybe it should be moved to the constructor?
	 * Or maybe there should be another builder or director?
	 */
	void setFaceSize(const float& faceSize);

	void addQuad(const Block::Face& blockFace,
		const std::vector<GLfloat>& textureQuad,
		const Block::Coordinate& blockPosition);

	void resetMesh();

    void blockMesh() const;
    void unblockMesh() const;
	[[nodiscard]] Mesh3D getMesh3D() const;

private:
	[[nodiscard]] std::vector<GLfloat> getFaceVertices(const Block::Face& blockFace) const;

private:
	GLuint index = 0;
	const Block::Coordinate& mOrigin;
    mutable std::recursive_mutex rebuildMeshMutex;
	Mesh3D chunkMesh;
	float mBlockFaceSize = Block::BLOCK_SIZE;
};
