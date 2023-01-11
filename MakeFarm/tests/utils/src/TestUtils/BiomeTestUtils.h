#pragma once
#include "World/Block/BlockId.h"
#include "World/Chunks/Chunk.h"
#include <set>

std::set<BlockId> allBlocksInGivenColumnOfChunk(Chunk::ChunkBlocks& chunkBlocks, int x, int z);