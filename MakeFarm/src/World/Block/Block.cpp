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
    : mBlockType(&BlockMap::blockMap().blockType(BlockId::Air))
{
}

Block::Block(const BlockId& blockId)
    : mBlockType(&BlockMap::blockMap().blockType(blockId))
{
}

void Block::setBlockType(const BlockId& blockId)
{
    mBlockType = &BlockMap::blockMap().blockType(blockId);
}


Block::TextureId Block::blockTextureId(const Block::Face& blockFace) const
{
    return mBlockType->textureId.at(blockFace);
}

BlockId Block::blockId() const
{
    return mBlockType->blockId;
}

bool Block::isTransparent() const
{
    return mBlockType->transparent;
}

bool Block::isFloral() const
{
    switch (blockId())
    {
        case BlockId::Leaves: return true;
        default: return false;
    }
}
bool Block::isCollidable() const
{
    return mBlockType->collidable;
}
