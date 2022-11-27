#pragma once
#include "Block.h"
#include "World/Item/ItemId.h"
#include <optional>


/**
 * It carries with it information about the type of block.
 */
struct BlockType
{
    BlockId id;
    std::string fileName;
    std::string name;
    bool transparent = false;
    bool collidable = true;
    std::optional<ItemId> dropItemId;
    std::unordered_map<Block::Face, Block::TextureId> textureId;
};
