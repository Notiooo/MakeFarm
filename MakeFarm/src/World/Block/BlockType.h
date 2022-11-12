#pragma once
#include "Block.h"


/**
 * It carries with it information about the type of block.
 */
struct BlockType
{
    BlockId blockId;
    std::string blockFileName;
    std::string blockName;
    bool transparent = false;
    bool collidable = true;
    std::unordered_map<Block::Face, Block::TextureId> textureId;
};
