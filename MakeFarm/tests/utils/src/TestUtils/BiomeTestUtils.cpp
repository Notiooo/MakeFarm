#include "BiomeTestUtils.h"

std::set<BlockId> allBlocksInGivenColumnOfChunk(ChunkInterface::ChunkBlocks& chunkBlocks, int x,
                                                int z)
{
    std::set<BlockId> setOfBlocks;
    for (auto y = 0; y < ChunkInterface::BLOCKS_PER_Y_DIMENSION; ++y)
    {
        setOfBlocks.insert(chunkBlocks[x][y][z]->id());
    }
    return setOfBlocks;
}