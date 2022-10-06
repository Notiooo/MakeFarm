#pragma once
#include "Block.h"


/**
 * It carries with it information about the type of block.
 */
struct BlockType
{
    std::string blockFileName;
    std::string blockName;
    bool transparent = false;
    std::unordered_map<Block::Face, Block::TextureId> textureId;
};
