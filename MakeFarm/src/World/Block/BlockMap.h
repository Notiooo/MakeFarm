#pragma once
#include "BlockType.h"

class BlockMap
{
public:
	const static BlockMap& getBlockMap();
	const BlockType& getBlockType(const std::string& blockName) const;

private:
	BlockMap();
	void parseDirectory(const std::string& directoryName);
	
	// Maps filename to specific BlockType
	std::unordered_map<std::string, BlockType> blockMap;
};
