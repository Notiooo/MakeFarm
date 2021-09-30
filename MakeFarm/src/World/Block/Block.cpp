#include "pch.h"
#include "Block.h"

#include "BlockMap.h"
#include "Renderer3D/BufferLayout.h"

sf::Vector3<Block::SizeType> Block::Coordinate::getNonBlockMetric() const
{
	return sf::Vector3<SizeType>(
		static_cast<SizeType>(x) * BLOCK_SIZE,
		static_cast<SizeType>(y) * BLOCK_SIZE,
		static_cast<SizeType>(z) * BLOCK_SIZE
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
