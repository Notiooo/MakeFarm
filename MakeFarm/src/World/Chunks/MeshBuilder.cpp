#include "pch.h"
#include "MeshBuilder.h"

#include "World/Chunks/Chunk.h"

MeshBuilder::MeshBuilder(const Block::Coordinate& origin)
	: mOrigin(origin)
{
}

void MeshBuilder::setFaceSize(const float& faceSize)
{
	mBlockFaceSize = faceSize;
}

void MeshBuilder::addQuad(const Block::Face& blockFace, const std::vector<GLfloat>& textureQuad,
                          const Block::Coordinate& blockPosition)
{
	auto& vertices = chunkMesh.vertices;
	auto& texCoords = chunkMesh.textureCoordinates;
	auto& indices = chunkMesh.indices;

	texCoords.insert(texCoords.end(), textureQuad.begin(), textureQuad.end());
	
	auto face = getFaceVertices(blockFace);
	const auto& originPos = mOrigin.getNonBlockMetric();
	const auto& blockPos = blockPosition.getNonBlockMetric();

	/*
	 * Some blocks are larger than others.
	 * It would be good if they were not just longer in one plane,
	 * but it was spread out among all of them -- increased relative to the center.
	 */
	const auto blockSizeDifference = (mBlockFaceSize - Block::BLOCK_SIZE) / 2.f;

	for(int i = 0; i < 3 * 4; i += 3)
	{
		// a row in given face (x,y,z)
		vertices.emplace_back(face[i]   * mBlockFaceSize + originPos.x + blockPos.x - blockSizeDifference);
		vertices.emplace_back(face[i+1] * mBlockFaceSize + originPos.y + blockPos.y - blockSizeDifference);
		vertices.emplace_back(face[i+2] * mBlockFaceSize + originPos.z + blockPos.z - blockSizeDifference);
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

void MeshBuilder::resetMesh()
{
	chunkMesh.indices.clear();
	chunkMesh.textureCoordinates.clear();
	chunkMesh.vertices.clear();
	index = 0;
}

Mesh3D MeshBuilder::getMesh3D() const
{
	return chunkMesh;
}


std::vector<GLfloat> MeshBuilder::getFaceVertices(const Block::Face& blockFace) const
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

	case Block::Face::Front:
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
