#pragma once
#include "DesertBiome.h"

DesertBiome::DesertBiome(int seed)
    : Biome(seed)
    , rd()
    , gen(rd())
    , mCactusDecider(0.003)
    , mCactusLengthGenerator(2, 5)
{
    mBasicTerrain.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mBasicTerrain.SetFrequency(0.03);
    mBasicTerrain.SetFractalOctaves(3);
    mBasicTerrain.SetSeed(seed);
}

int DesertBiome::surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ)
{
    auto basicTerrainNoise = mBasicTerrain.GetNoise(static_cast<float>(blockCoordinateX),
                                                    static_cast<float>(blockCoordinateZ));

    auto heightOfBlocks = basicTerrainNoise * BASIC_TERRAIN_SQUASHING_FACTOR;

    heightOfBlocks = (heightOfBlocks + 1 + BASIC_TERRAIN_SQUASHING_FACTOR) /
                     (1 + BASIC_TERRAIN_SQUASHING_FACTOR + 1 + BASIC_TERRAIN_SQUASHING_FACTOR);

    auto surfaceLevel = static_cast<int>(
        ((heightOfBlocks) * (MAX_HEIGHT_MAP - MINIMAL_TERRAIN_LEVEL)) + MINIMAL_TERRAIN_LEVEL);

    return surfaceLevel;
}

void DesertBiome::generateColumnOfBlocks(ChunkInterface::ChunkBlocks& chunkBlocks, int surfaceLevel,
                                         int blockCoordinateX, int blockCoordinateZ)
{
    auto& x = blockCoordinateX;
    auto& z = blockCoordinateZ;

    for (auto y = 0; y < ChunkInterface::BLOCKS_PER_Y_DIMENSION; ++y)
    {
        if (y == surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Sand);
        }
        else if (y < surfaceLevel - 5)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Stone);
        }
        else if (y < surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Sand);
        }
        else if (y < SEA_LEVEL)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Water);
        }
        else if (y == surfaceLevel + 1 && mCactusDecider(gen))
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Cactus);
            mCactusToPlace.emplace_back(x, y, z);
        }
        else
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Air);
        }
    }
}

void DesertBiome::postGenerationPlacements(ChunkInterface& chunk)
{
    placeCactuses(chunk);
}

void DesertBiome::placeCactuses(ChunkInterface& chunk)
{
    for (auto& coordinateToPlaceCactus: mCactusToPlace)
    {
        auto cactusLength = mCactusLengthGenerator(gen);
        for (int i = 0; i < cactusLength; ++i)
        {
            auto& block = coordinateToPlaceCactus;
            chunk.tryToPlaceBlock(BlockId::Cactus, {block.x, block.y + i, block.z},
                                  {BlockId::AllBlocks}, RebuildOperation::None);
        }
    }
}

BiomeId DesertBiome::biomeId()
{
    return BiomeId::Desert;
}