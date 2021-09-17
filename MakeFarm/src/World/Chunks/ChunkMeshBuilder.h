#pragma once
#include "Renderer3D/Mesh3D.h"
#include "World/Block/Block.h"


class Chunk;

class ChunkMeshBuilder
{
public:

	explicit ChunkMeshBuilder(const Chunk& chunk);
	
	void addQuad(const Block::Face& blockFace,
		const std::vector<GLfloat>& textureQuad,
		const sf::Vector3i& blockPosition);

	Mesh3D getMesh3D() const;

private:

	std::vector<GLfloat> getFaceVertices(const Block::Face& blockFace) const;
	GLuint index = 0;

	const Chunk& chunk;
	Mesh3D chunkMesh;
};
