#include "pch.h"
#include "Chunk.h"

#include "ChunkMeshBuilder.h"

Chunk::Chunk(const TexturePack& texturePack)
	: texturePack(texturePack)
	, meshBuilder(*this)
{
	for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
	for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
	for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
	{
		chunkOfBlocks[x][y][z] = std::make_unique<Block>(texturePack);
	}
}

void Chunk::createCube(const sf::Vector3i& pos)
{
    //chunkOfBlocks[pos.x][pos.y][pos.z]->generateCube(pos.x, pos.y, pos.z);

	auto& block = getBlock(pos);

	for(int i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
	{
		if(!faceHasNeighbor(static_cast<Block::Face>(i), pos))
		{
			meshBuilder.addQuad(static_cast<Block::Face>(i), 
				texturePack.getNormalizedCoordinates(block.getBlockTexture()), pos);
		}
	}
	
}

void Chunk::createChunk()
{	
	for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
	for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
	for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
	{
		if (getBlock({x, y, z}).getBlockTexture() == TextureSheet::Air)
			continue;

		createCube({ x, y, z });
	}
	
	model.setMesh(meshBuilder.getMesh3D());
}

void Chunk::update(const float& deltaTime)
{
}

Block& Chunk::getBlock(const sf::Vector3i& position) const
{
	return *chunkOfBlocks[position.x][position.y][position.z];
}

bool Chunk::isPositionInsideChunk(const sf::Vector3i& position) const
{
	if( position.x < BLOCKS_PER_DIMENSION && position.x >= 0 &&
		position.y < BLOCKS_PER_DIMENSION && position.y >= 0 &&
		position.z < BLOCKS_PER_DIMENSION && position.z >= 0)
	{
		return true;
	}

	return false;
}

Block& Chunk::getBlock(const sf::Vector3i& position, const Direction& direction) const
{
	return getBlock(getBlockPosition(position, direction));
}

sf::Vector3i Chunk::getBlockPosition(const sf::Vector3i& position, const Direction& direction) const
{
	switch (direction)
	{
	case Direction::Above: 
		return { position.x, position.y + 1, position.z };
	case Direction::Below: 
		return { position.x, position.y - 1, position.z };
	case Direction::ToTheLeft: 
		return { position.x - 1, position.y, position.z };
	case Direction::ToTheRight: 
		return { position.x + 1, position.y, position.z };
	case Direction::InFront: 
		return { position.x, position.y, position.z + 1};
	case Direction::Behind: 
		return { position.x, position.y, position.z - 1};
	}
}

bool Chunk::faceHasNeighbor(Block::Face face, const sf::Vector3i& blockPos)
{
	auto getBlockType = [&blockPos, this](const Direction& face)
	{
		const auto blockNeighborPosition = getBlockPosition(blockPos, face);
		if (!isPositionInsideChunk(blockNeighborPosition))
			return TextureSheet::Air;
		return (getBlock(blockNeighborPosition).getBlockTexture());
	};
	
	switch(face)
	{
	case Block::Face::Top:
		return (getBlockType(Direction::Above) != TextureSheet::Air);
	case Block::Face::Left:
		return (getBlockType(Direction::ToTheLeft) != TextureSheet::Air);
	case Block::Face::Right:
		return (getBlockType(Direction::ToTheRight) != TextureSheet::Air);
	case Block::Face::Bottom:
		return (getBlockType(Direction::Below) != TextureSheet::Air);
	case Block::Face::Forward:
		return (getBlockType(Direction::InFront) != TextureSheet::Air);
	case Block::Face::Back:
		return (getBlockType(Direction::Behind) != TextureSheet::Air);
	}
	
}

void Chunk::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	texturePack.bind();
	model.draw(renderer3d, shader);
}
