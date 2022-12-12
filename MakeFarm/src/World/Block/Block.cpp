#include "Block.h"
#include "pch.h"

#include "BlockMap.h"
#include "Renderer3D/BufferLayout.h"


Block::Block()
    : mBlockType(&BlockMap::blockMap().blockType(BlockId::Air))
{
}

Block::Block(const BlockId& blockId)
    : mBlockType(&BlockMap::blockMap().blockType(blockId))
{
}

Block::Block(const Block& rhs)
    : mBlockType(rhs.mBlockType)
{
}

Block::Coordinate Block::Coordinate::coordinateInGivenDirection(Direction direction) const
{
    switch (direction)
    {
        case Direction::Above: return Block::Coordinate{x, y + 1, z};
        case Direction::Below: return Block::Coordinate{x, y - 1, z};
        case Direction::ToTheLeft: return Block::Coordinate{x - 1, y, z};
        case Direction::ToTheRight: return Block::Coordinate{x + 1, y, z};
        case Direction::InFront: return Block::Coordinate{x, y, z + 1};
        case Direction::Behind: return Block::Coordinate{x, y, z - 1};
        default: throw std::runtime_error("Unsupported direction type");
    }
}

sf::Vector3<Block::SizeType> Block::Coordinate::nonBlockMetric() const
{
    return {static_cast<SizeType>(x) * BLOCK_SIZE, static_cast<SizeType>(y) * BLOCK_SIZE,
            static_cast<SizeType>(z) * BLOCK_SIZE};
}

void Block::setBlockType(const BlockId& blockId)
{
    std::scoped_lock guard(mBlockAccessMutex);
    mBlockType = &BlockMap::blockMap().blockType(blockId);
}

Block::TextureId Block::blockTextureId(const Block::Face& blockFace) const
{
    return mBlockType->textureId.at(blockFace);
}

BlockId Block::id() const
{
    // TODO: Often crash here, mBlockType read memory failed (0 of 4 bytes read)
    // Maybe chunk holding this was removed?
    // I think there should be a queue for removing chunk at some points... I guess?
    std::shared_lock guard(mBlockAccessMutex);
    return mBlockType->id;
}

bool Block::isTransparent() const
{
    return mBlockType->transparent;
}

bool Block::isFloral() const
{
    switch (id())
    {
        case BlockId::Leaves:
        case BlockId::SnowyLeaves: return true;
        default: return false;
    }
}
bool Block::isCollidable() const
{
    return mBlockType->collidable;
}

Direction Block::directionOfFace(Block::Face face)
{
    switch (face)
    {
        case Face::Top: return Direction::Above;
        case Face::Left: return Direction::ToTheLeft;
        case Face::Right: return Direction::ToTheRight;
        case Face::Bottom: return Direction::Below;
        case Face::Front: return Direction::InFront;
        case Face::Back: return Direction::Behind;
        default: throw std::runtime_error("Unsupported block face type");
    }
}

std::optional<ItemId> Block::itemItDrops()
{
    return mBlockType->dropItemId;
}
