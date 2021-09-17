#include "pch.h"
#include "ChunkMeshBuilder.h"

#include "World/Chunks/Chunk.h"

ChunkMeshBuilder::ChunkMeshBuilder(const Chunk& chunk)
	: chunk(chunk)
{
}

void ChunkMeshBuilder::addQuad(const Block::Face& blockFace, const std::vector<GLfloat>& textureQuad,
	const sf::Vector3i& blockPosition)
{
	auto& vertices = chunkMesh.vertices;
	auto& texCoords = chunkMesh.textureCoordinates;
	auto& indices = chunkMesh.indices;

	texCoords.insert(texCoords.end(), textureQuad.begin(), textureQuad.end());
	
	auto face = getFaceVertices(blockFace);
	const auto& chunkPos = chunk.position;
	
	for(int i = 0; i < 3 * 4; i += 3)
	{
		// a row in given face (x,y,z)
		vertices.emplace_back(face[i]   * Block::BLOCK_SIZE + chunkPos.x + blockPosition.x * Block::BLOCK_SIZE);
		vertices.emplace_back(face[i+1] * Block::BLOCK_SIZE + chunkPos.y + blockPosition.y * Block::BLOCK_SIZE);
		vertices.emplace_back(face[i+2] * Block::BLOCK_SIZE + chunkPos.z + blockPosition.z * Block::BLOCK_SIZE);
	}

	indices.insert(indices.end(),
	{
		index,
		index + 1,
		index + 2,
		
		index + 2,
		index + 3,
		index
	});
	index += 4;
}

Mesh3D ChunkMeshBuilder::getMesh3D() const
{
	return chunkMesh;
}


std::vector<GLfloat> ChunkMeshBuilder::getFaceVertices(const Block::Face& blockFace) const
{
	switch(blockFace)
	{
	case Block::Face::Top:
		return
		{
		  //x  y  z
			0, 1, 1, // top close left
			1, 1, 1, // top close right
			1, 1, 0, // top far right
			0, 1, 0, // top far left
		};
		
	case Block::Face::Left:
		return
		{
			0, 0, 0,
			0, 0, 1,
			0, 1, 1,
			0, 1, 0
		};
		
	case Block::Face::Right:
		return
		{
			1, 0, 1,
			1, 0, 0,
			1, 1, 0,
			1, 1, 1
		};

	case Block::Face::Bottom:
		return
		{
			0, 0, 0,
			1, 0, 0,
			1, 0, 1,
			0, 0, 1
		};

	case Block::Face::Forward:
		return
		{
			0, 0, 1, // front left bottom
			1, 0, 1, // front right bottom
			1, 1, 1, // front right top
			0, 1, 1  // front left top
		};

	case Block::Face::Back:
		return
		{
			1, 0, 0, // back right bottom
			0, 0, 0, // back left bottom
			0, 1, 0, // back left top
			1, 1, 0, // back right top
		};
	}
}
