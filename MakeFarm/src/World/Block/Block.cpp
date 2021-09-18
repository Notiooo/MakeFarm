#include "pch.h"
#include "Block.h"

#include "BlockMap.h"
#include "Renderer3D/BufferLayout.h"

Block::Coordinate::Coordinate(IntegerUnit x, IntegerUnit y, IntegerUnit z)
	: mBlockCoordinates({x, y, z})
{
}

Block::Coordinate::Coordinate(const sf::Vector3i& blockCoordinates)
	: mBlockCoordinates(blockCoordinates)
{

}

Block::Coordinate::Coordinate(Coordinate&& rhs) noexcept
	: mBlockCoordinates(std::move(rhs.mBlockCoordinates))
	, x(mBlockCoordinates.x)
	, y(mBlockCoordinates.y)
	, z(mBlockCoordinates.z)
{
}

Block::Coordinate::Coordinate(const Coordinate& rhs) noexcept
	: mBlockCoordinates(rhs.mBlockCoordinates)
	, x(mBlockCoordinates.x)
	, y(mBlockCoordinates.y)
	, z(mBlockCoordinates.z)
{
}

Block::Coordinate Block::Coordinate::operator-(const Coordinate& rhs) const
{
	return mBlockCoordinates - rhs.mBlockCoordinates;
}

Block::Coordinate Block::Coordinate::operator+(const Coordinate& rhs) const
{
	return mBlockCoordinates + rhs.mBlockCoordinates;
}

bool Block::Coordinate::operator==(const Coordinate& rhs) const
{
	return mBlockCoordinates == rhs.mBlockCoordinates;
}

bool Block::Coordinate::operator!=(const Coordinate& rhs) const
{
	return !(operator==(rhs));
}

sf::Vector3<Block::SizeType> Block::Coordinate::getNonBlockMetric() const
{
	return sf::Vector3<SizeType>(
		static_cast<SizeType>(x) * BLOCK_SIZE,
		static_cast<SizeType>(y) * BLOCK_SIZE,
		static_cast<SizeType>(z) * BLOCK_SIZE
	);
}

Block::Coordinate Block::Coordinate::nonBlockToBlockMetric(const sf::Vector3i& nonBlockVector)
{
	return Block::Coordinate(
		nonBlockVector.x / static_cast<IntegerUnit>(BLOCK_SIZE),
		nonBlockVector.y / static_cast<IntegerUnit>(BLOCK_SIZE),
		nonBlockVector.z / static_cast<IntegerUnit>(BLOCK_SIZE)
	);
}

Block::Block()
	: blockType(&BlockMap::getBlockMap().getBlockType("Air"))
{
}

Block::Block(const std::string& blockName)
	: blockType(&BlockMap::getBlockMap().getBlockType(blockName))
{
}

void Block::setBlockType(const std::string& blockName)
{
	blockType = &BlockMap::getBlockMap().getBlockType(blockName);
}


Block::TextureId Block::getBlockTextureId(const Block::Face& blockFace) const
{
	return blockType->textureId.at(blockFace);
}

std::string Block::getBlockId() const
{
	return blockType->blockFileName;
}

bool Block::isTransparent() const
{
	return blockType->transparent;
}
