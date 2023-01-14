#pragma once
#include "VeryDryForestBiome.h"

VeryDryForestBiome::VeryDryForestBiome(int seed)
    : Biome(seed)
    , rd()
    , gen(rd())
    , mDryTreeDecider(0.003)
    , mDryTreeLengthGenerator(2, 5)
{
    mBasicTerrain.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mBasicTerrain.SetFrequency(0.03);
    mBasicTerrain.SetFractalOctaves(3);
    mBasicTerrain.SetSeed(seed);
}

int VeryDryForestBiome::surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ)
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

void VeryDryForestBiome::generateColumnOfBlocks(ChunkInterface::ChunkBlocks& chunkBlocks,
                                                int surfaceLevel, int blockCoordinateX,
                                                int blockCoordinateZ)
{
    auto& x = blockCoordinateX;
    auto& z = blockCoordinateZ;

    for (auto y = 0; y < ChunkInterface::BLOCKS_PER_Y_DIMENSION; ++y)
    {
        if (y == surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::DryGrass);
        }
        else if (y < surfaceLevel - 5)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Stone);
        }
        else if (y < surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Dirt);
        }
        else if (y < SEA_LEVEL + 1 && y < surfaceLevel + 2)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Sand);

            // TODO: Change it to more sophisticated system
            if (chunkBlocks[x][y - 1][z]->id() == BlockId::DryGrass)
            {
                chunkBlocks[x][y - 1][z]->setBlockType(BlockId::Sand);
            }
        }
        else if (y < SEA_LEVEL)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Water);
        }
        else if (y == surfaceLevel + 1 && mDryTreeDecider(gen))
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Log);
            mDryTreesToPlace.emplace_back(x, y, z);
        }
        else
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Air);
        }
    }
}

void VeryDryForestBiome::postGenerationPlacements(ChunkInterface& chunk)
{
    placeDryTrees(chunk);
    mDryTreesToPlace.clear();
}

BiomeId VeryDryForestBiome::biomeId()
{
    return BiomeId::VeryDryForest;
}

void VeryDryForestBiome::placeDryTrees(ChunkInterface& chunk)
{
    for (auto& coordinateToPlaceTree: mDryTreesToPlace)
    {
        auto treeLength = mDryTreeLengthGenerator(gen);
        auto& block = coordinateToPlaceTree;
        for (int i = 0; i < treeLength; ++i)
        {
            chunk.tryToPlaceBlock(BlockId::Log, {block.x, block.y + i, block.z},
                                  {BlockId::AllBlocks}, RebuildOperation::None);
        }
    }
}