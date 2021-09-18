#pragma once
#include "Block.h"


struct BlockType
{
	std::string blockFileName;
	std::string blockName;
	bool transparent = false;
	std::unordered_map<Block::Face, Block::TextureId> textureId;
};
