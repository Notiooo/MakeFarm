#pragma once
#include "Renderer3D/Mesh3D.h"
#include "World/Block/Block.h"


class Chunk;

class ChunkMeshBuilder
{
public:
	explicit ChunkMeshBuilder(const Chunk& chunk);
	ChunkMeshBuilder(ChunkMeshBuilder&&) noexcept = default;
	
	void addQuad(const Block::Face& blockFace,
		const std::vector<GLfloat>& textureQuad,
		const Block::Coordinate& blockPosition);

	[[nodiscard]] Mesh3D getMesh3D() const;

private:
	std::vector<GLfloat> getFaceVertices(const Block::Face& blockFace) const;

private:
	GLuint index = 0;
	const Chunk& chunk;
	Mesh3D chunkMesh;
};
