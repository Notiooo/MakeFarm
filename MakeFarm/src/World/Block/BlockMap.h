#pragma once
#include "BlockType.h"

/**
 * A map containing access to information about the blocks in the game.
 */
class BlockMap
{
public:
    /**
     * Returns an instance of the coordinateInGivenDirection map
     * @return
     */
    const static BlockMap& blockMap();

    /**
     * Returns the type of block, which is information about the coordinateInGivenDirection
     * @param blockId The identifier of the coordinateInGivenDirection
     * @return Block information set
     */
    const BlockType& blockType(const BlockId& blockId) const;

private:
    BlockMap();

    /**
     * It searches the folder for coordinateInGivenDirection settings.
     * @param directoryName
     */
    void parseDirectory(const std::string& directoryName);

    /**
     * Maps filename to specific BlockType
     */
    std::unordered_map<BlockId, BlockType> mBlockMap;
};
