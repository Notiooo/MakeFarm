#include "pch.h"
#include "Chunk.h"

#include "ChunkContainer.h"
#include "ChunkMeshBuilder.h"
#include "World/Block/BlockType.h"

Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, const ChunkContainer& parent)
	: Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack, parent)
{

}

Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack)
	: Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack)
{
}

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack, const ChunkContainer& parent)
	: Chunk(blockPosition, texturePack)
{
	mParentContainer = &parent;
}

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack)
	: mChunkPosition(std::move(blockPosition))
	, mTexturePack(texturePack)
	, mMeshBuilder(*this)
	, mChunkOfBlocks(std::make_unique<MultiDimensionalArray<Block, 
		BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION>>())
{
	for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
	for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
	for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
	{
		// for test purposes
		if (y == BLOCKS_PER_DIMENSION - 1)
			(*mChunkOfBlocks)[x][y][z].setBlockType("Grass");
		else if (y > BLOCKS_PER_DIMENSION - 5)
			(*mChunkOfBlocks)[x][y][z].setBlockType("Dirt");
		else
			(*mChunkOfBlocks)[x][y][z].setBlockType("Stone");

	}
}

Chunk::Chunk(Chunk&& rhs) noexcept
	: mChunkPosition(std::move(rhs.mChunkPosition))
	, mTexturePack(rhs.mTexturePack)
	, mParentContainer(std::move(rhs.mParentContainer))
	, mMeshBuilder(*this) // new one
	, mChunkOfBlocks(std::move(rhs.mChunkOfBlocks))
	, mModel(std::move(rhs.mModel))
{
	rhs.mParentContainer = nullptr;
}

void Chunk::createBlockMesh(const Block::Coordinate& pos)
{
	auto& block = getLocalBlock(pos);

	for(auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
	{
		if(faceHasTransparentNeighbor(static_cast<Block::Face>(i), pos))
		{
			mMeshBuilder.addQuad(static_cast<Block::Face>(i), 
				mTexturePack.getNormalizedCoordinates(block.getBlockTextureId(static_cast<Block::Face>(i))), pos);
		}
	}
	
}

void Chunk::createMesh()
{
	for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
	for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
	for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
	{
		if (getLocalBlock({x, y, z}).getBlockId() == "Air")
			continue;

		createBlockMesh({ x, y, z });
	}
	
	mModel.setMesh(mMeshBuilder.getMesh3D());
}

void Chunk::update(const float& deltaTime)
{
}

bool Chunk::areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates) const
{
	if( localCoordinates.x < BLOCKS_PER_DIMENSION && localCoordinates.x >= 0 &&
		localCoordinates.y < BLOCKS_PER_DIMENSION && localCoordinates.y >= 0 &&
		localCoordinates.z < BLOCKS_PER_DIMENSION && localCoordinates.z >= 0)
	{
		return true;
	}

	return false;
}

Block::Coordinate Chunk::globalToLocalCoordinates(const Block::Coordinate& globalPosition) const
{
	return globalPosition - mChunkPosition;
}

Block& Chunk::getLocalBlock(const Block::Coordinate& position)
{
	return const_cast<Block&>(static_cast<const Chunk&>(*this).getLocalBlock(position));
}

Block& Chunk::getLocalBlock(const Block::Coordinate& position, const Direction& direction)
{
	return const_cast<Block&>(static_cast<const Chunk&>(*this).getLocalBlock(position, direction));
}

const Block& Chunk::getLocalBlock(const Block::Coordinate& localCoordinates) const
{
	return (*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z];
}

const Block& Chunk::getLocalBlock(const Block::Coordinate& localCoordinates, const Direction& direction) const
{
	return getLocalBlock(getBlockPosition(localCoordinates, direction));
}

Block::Coordinate Chunk::getBlockPosition(const Block::Coordinate& position, const Direction& direction) const
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

Block::Coordinate Chunk::localToGlobalCoordinates(const Block::Coordinate& position) const
{
	return mChunkPosition + position;
}

bool Chunk::faceHasTransparentNeighbor(const Block::Face& face, const Block::Coordinate& blockPos)
{
	auto isBlockTransparent = [&blockPos, this](const Direction& face)
	{
		const auto blockNeighborPosition = getBlockPosition(blockPos, face);
		if (areLocalCoordinatesInsideChunk(blockNeighborPosition))
			return (getLocalBlock(blockNeighborPosition).isTransparent());

		if (belongsToContainer())
		{
			if (const auto& neighborBlock = mParentContainer->getWorldBlock(
											localToGlobalCoordinates(blockNeighborPosition)))
			{
				return neighborBlock->isTransparent();
			}
		}

		return true;
	};
	
	switch(face)
	{
	case Block::Face::Top:
		return (isBlockTransparent(Direction::Above));
	case Block::Face::Left:
		return (isBlockTransparent(Direction::ToTheLeft));
	case Block::Face::Right:
		return (isBlockTransparent(Direction::ToTheRight));
	case Block::Face::Bottom:
		return (isBlockTransparent(Direction::Below));
	case Block::Face::Front:
		return (isBlockTransparent(Direction::InFront));
	case Block::Face::Back:
		return (isBlockTransparent(Direction::Behind));
	}
	
}

bool Chunk::belongsToContainer() const
{
	return (mParentContainer != nullptr);
}

void Chunk::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	mModel.draw(renderer3d, shader);
}
