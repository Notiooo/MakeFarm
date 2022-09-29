#pragma once
#include "BlockType.h"

/**
 * A map containing access to information about the blocks in the game.
 */
class BlockMap
{
public:
    /**
     * Returns an instance of the block map
     * @return
     */
	const static BlockMap& blockMap();

    /**
     * Returns the type of block, which is information about the block
     * @param blockName The name of the cfg file
     * @return Block information set
     */
	const BlockType& blockType(const std::string& blockName) const;

private:
	BlockMap();

    /**
     * It searches the folder for block settings.
     * @param directoryName
     */
	void parseDirectory(const std::string& directoryName);
	
	/**
	 * Maps filename to specific BlockType
	 */
	std::unordered_map<std::string, BlockType> mBlockMap;
};
