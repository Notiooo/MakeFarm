#pragma once
#include "SnowyBiome.h"
#include "World/Chunks/CoordinatesAroundOriginGetter.h"

SnowyBiome::SnowyBiome(int seed)
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

int SnowyBiome::surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ)
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

void SnowyBiome::generateColumnOfBlocks(ChunkInterface::ChunkBlocks& chunkBlocks, int surfaceLevel,
                                        int blockCoordinateX, int blockCoordinateZ)
{
    auto& x = blockCoordinateX;
    auto& z = blockCoordinateZ;

    for (auto y = 0; y < ChunkInterface::BLOCKS_PER_Y_DIMENSION; ++y)
    {
        if (y == surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::SnowyGrass);
        }
        else if (y < surfaceLevel - 5)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Stone);
        }
        else if (y < surfaceLevel)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Dirt);
        }
        else if (y == SEA_LEVEL - 1)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Ice);
        }
        else if (y < SEA_LEVEL - 1)
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Water);

            if (chunkBlocks[x][y - 1][z]->id() == BlockId::SnowyGrass)
            {
                chunkBlocks[x][y - 1][z]->setBlockType(BlockId::Sand);
            }
        }
        else if (y == surfaceLevel + 1 && mTreeDecider(gen))
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::SpruceLog);
            mTreesToPlace.emplace_back(x, y, z);
        }
        else
        {
            chunkBlocks[x][y][z] = std::make_unique<Block>(BlockId::Air);
        }
    }
}

void SnowyBiome::postGenerationPlacements(ChunkInterface& chunk)
{
    placeTrees(chunk);
    mTreesToPlace.clear();
}

void SnowyBiome::placeTrees(ChunkInterface& chunk)
{
    for (auto& coordinateToPlaceTree: mTreesToPlace)
    {
        placeTree(chunk, coordinateToPlaceTree);
    }
}

void SnowyBiome::placeTree(ChunkInterface& chunk, const Block::Coordinate& block)
{
    auto treeLength = mTreeLengthGenerator(gen);
    placeLogOfTheTree(chunk, block, treeLength);
    placeTopPartOfTreeTopMadeOfLeaves(chunk, block, treeLength);
    placeBottomPartOfTreeTopMadeOfLeaves(chunk, block, treeLength);
}

void SnowyBiome::placeBottomPartOfTreeTopMadeOfLeaves(ChunkInterface& chunk,
                                                      const Block::Coordinate& block,
                                                      int treeLength) const
{
    auto summer = [](auto n)
    {
        if (n < 1)
        {
            return 0;
        }

        auto sum = 0;
        for (int i = 1; i < n; ++i)
        {
            sum += i;
        }
        return sum;
    };

    // TODO: Please, simplify
    auto treeLevels = treeLength - 3;
    // Going from bottom to the top
    for (int treeLevel = -treeLevels; treeLevel < -1; ++treeLevel)
    {
        auto levelThickness = std::abs(treeLevel);
        auto treeTop = sf::Vector3i(block.x, block.y + treeLength + treeLevel, block.z);
        CoordinatesAroundOriginGetter coordinatesAround(treeTop);
        coordinatesAround.nextValue();
        auto leavesInThisLevel = 8 * summer((levelThickness / 2) + 1);
        for (int i = 0; i < leavesInThisLevel; ++i)
        {
            chunk.tryToPlaceBlock(BlockId::SnowyLeaves, coordinatesAround.nextValue(),
                                  {BlockId::Air}, RebuildOperation::None);
        }
    }
}

void SnowyBiome::placeTopPartOfTreeTopMadeOfLeaves(ChunkInterface& chunk,
                                                   const Block::Coordinate& block,
                                                   int treeLength) const
{
    auto treeTop = sf::Vector3i(block.x, block.y + treeLength, block.z);
    chunk.tryToPlaceBlock(BlockId::SnowyLeaves, {treeTop.x, treeTop.y, treeTop.z});
    chunk.tryToPlaceBlock(BlockId::SnowyLeaves, {treeTop.x + 1, treeTop.y - 1, treeTop.z},
                          {BlockId::Air}, RebuildOperation::None);
    chunk.tryToPlaceBlock(BlockId::SnowyLeaves, {treeTop.x - 1, treeTop.y - 1, treeTop.z},
                          {BlockId::Air}, RebuildOperation::None);
    chunk.tryToPlaceBlock(BlockId::SnowyLeaves, {treeTop.x, treeTop.y - 1, treeTop.z + 1},
                          {BlockId::Air}, RebuildOperation::None);
    chunk.tryToPlaceBlock(BlockId::SnowyLeaves, {treeTop.x, treeTop.y - 1, treeTop.z - 1},
                          {BlockId::Air}, RebuildOperation::None);
}

void SnowyBiome::placeLogOfTheTree(ChunkInterface& chunk, const Block::Coordinate& block,
                                   int treeLength) const
{
    for (int i = 0; i < treeLength; ++i)
    {
        chunk.tryToPlaceBlock(BlockId::SpruceLog, {block.x, block.y + i, block.z},
                              {BlockId::AllBlocks}, RebuildOperation::None);
    }
}

BiomeId SnowyBiome::biomeId()
{
    return BiomeId::Snowy;
}