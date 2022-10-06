#include "Block.h"
#include "pch.h"

#include "BlockMap.h"
#include "Renderer3D/BufferLayout.h"

sf::Vector3<Block::SizeType> Block::Coordinate::nonBlockMetric() const
{
    return sf::Vector3<SizeType>(static_cast<SizeType>(x) * BLOCK_SIZE,
                                 static_cast<SizeType>(y) * BLOCK_SIZE,
                                 static_cast<SizeType>(z) * BLOCK_SIZE);
}

Block::Block()
    : mBlockType(&BlockMap::blockMap().blockType("Air"))
{
}

Block::Block(const std::string& blockName)
    : mBlockType(&BlockMap::blockMap().blockType(blockName))
{
}

void Block::setBlockType(const std::string& blockName)
{
    mBlockType = &BlockMap::blockMap().blockType(blockName);
}


Block::TextureId Block::blockTextureId(const Block::Face& blockFace) const
{
    return mBlockType->textureId.at(blockFace);
}

std::string Block::blockId() const
{
    return mBlockType->blockFileName;
}

bool Block::isTransparent() const
{
    return mBlockType->transparent;
}
