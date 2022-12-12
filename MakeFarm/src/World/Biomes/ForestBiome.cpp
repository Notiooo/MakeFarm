#pragma once
#include "ForestBiome.h"
#include "World/Chunks/CoordinatesAroundOriginGetter.h"

ForestBiome::ForestBiome(int seed)
    : Biome(seed)
    , rd()
    , gen(rd())
    , mTreeDecider(0.01)
    , mTreeLengthGenerator(5, 9)
{
    mBasicTerrain.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mBasicTerrain.SetFrequency(0.04);
    mBasicTerrain.SetFractalOctaves(3);
    mBasicTerrain.SetSeed(seed);

    mHillsAndValleys.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mHillsAndValleys.SetFrequency(0.01);
    mHillsAndValleys.SetFractalOctaves(3);
    mHillsAndValleys.SetSeed(seed);
}
int ForestBiome::surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ)
{
    auto basicTerrainNoise = mBasicTerrain.GetNoise(static_cast<float>(blockCoordinateX),
                                                    static_cast<float>(blockCoordinateZ));

    auto hillsNoise = mHillsAndValleys.GetNoise(static_cast<float>(blockCoordinateX),
                                                static_cast<float>(blockCoordinateZ));

    auto heightOfBlocks = (basicTerrainNoise * BASIC_TERRAIN_SQUASHING_FACTOR) + hillsNoise;

    heightOfBlocks = (heightOfBlocks + 1 + BASIC_TERRAIN_SQUASHING_FACTOR) /
                     (1 + BASIC_TERRAIN_SQUASHING_FACTOR + 1 + BASIC_TERRAIN_SQUASHING_FACTOR);

    auto surfaceLevel = static_cast<int>(
        ((heightOfBlocks) * (MAX_HEIGHT_MAP - MINIMAL_TERRAIN_LEVEL)) + MINIMAL_TERRAIN_LEVEL);

    return surfaceLevel;
}
void ForestBiome::generateColumnOfBlocks(Chunk::ChunkBlocks& chunkBlocks, int surfaceLevel,
                                         int blockCoordinateX, int blockCoordinateZ)
{
    auto& x = blockCoordinateX;
    auto& z = blockCoordinateZ;

    for (auto y = 0; y < Chunk::BLOCKS_PER_Y_DIMENSION; ++y)
    {
        if (y == surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Grass);
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
            if (chunkBlocks[x][y - 1][z]->id() == BlockId::Grass)
            {
                chunkBlocks[x][y - 1][z]->setBlockType(BlockId::Sand);
            }
        }
        else if (y < SEA_LEVEL)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Water);
        }
        else if (y == surfaceLevel + 1 && mTreeDecider(gen))
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Log);
            mTreesToPlace.emplace_back(x, y, z);
        }
        else
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Air);
        }
    }
}
void ForestBiome::postGenerationPlacements(Chunk& chunk)
{
    placeTrees(chunk);
    mTreesToPlace.clear();
}
void ForestBiome::placeTrees(Chunk& chunk)
{
    for (auto& coordinateToPlaceTree: mTreesToPlace)
    {
        placeTree(chunk, coordinateToPlaceTree);
    }
}
void ForestBiome::placeTree(Chunk& chunk, const Block::Coordinate& block)
{
    auto treeLength = mTreeLengthGenerator(gen);
    placeLogOfTheTree(chunk, block, treeLength);
    placeTopPartOfTreeTopMadeOfLeaves(chunk, block, treeLength);
    placeBottomPartOfTreeTopMadeOfLeaves(chunk, block, treeLength);
}
void ForestBiome::placeBottomPartOfTreeTopMadeOfLeaves(Chunk& chunk, const Block::Coordinate& block,
                                                       int treeLength) const
{
    for (int treeLevel = -3; treeLevel < -1; ++treeLevel)
    {
        auto treeTop = sf::Vector3i(block.x, block.y + treeLength + treeLevel, block.z);
        CoordinatesAroundOriginGetter coordinatesAround(treeTop);
        for (int i = 0; i < 24; ++i)
        {
            chunk.tryToPlaceBlock(BlockId::Leaves, coordinatesAround.nextValue(), {BlockId::Air},
                                  Chunk::RebuildOperation::None);
        }
    }
}
void ForestBiome::placeTopPartOfTreeTopMadeOfLeaves(Chunk& chunk, const Block::Coordinate& block,
                                                    int treeLength) const
{
    auto treeTop = sf::Vector3i(block.x, block.y + treeLength, block.z);
    chunk.tryToPlaceBlock(BlockId::Leaves, {treeTop.x, treeTop.y, treeTop.z});
    for (int i = -1; i < 1; ++i)
    {
        chunk.tryToPlaceBlock(BlockId::Leaves, {treeTop.x + 1, treeTop.y + i, treeTop.z},
                              {BlockId::Air}, Chunk::RebuildOperation::None);
        chunk.tryToPlaceBlock(BlockId::Leaves, {treeTop.x - 1, treeTop.y + i, treeTop.z},
                              {BlockId::Air}, Chunk::RebuildOperation::None);
        chunk.tryToPlaceBlock(BlockId::Leaves, {treeTop.x, treeTop.y + i, treeTop.z + 1},
                              {BlockId::Air}, Chunk::RebuildOperation::None);
        chunk.tryToPlaceBlock(BlockId::Leaves, {treeTop.x, treeTop.y + i, treeTop.z - 1},
                              {BlockId::Air}, Chunk::RebuildOperation::None);
    }
}
void ForestBiome::placeLogOfTheTree(Chunk& chunk, const Block::Coordinate& block,
                                    int treeLength) const
{
    for (int i = 0; i < treeLength; ++i)
    {
        chunk.tryToPlaceBlock(BlockId::Log, {block.x, block.y + i, block.z}, {BlockId::AllBlocks},
                              Chunk::RebuildOperation::None);
    }
}
BiomeId ForestBiome::biomeId()
{
    return BiomeId::Forest;
}