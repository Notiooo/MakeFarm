#include "World/Biomes/ForestBiome.h"
#include "TestUtils/BiomeTestUtils.h"
#include "gtest/gtest.h"

namespace
{

class UglyTestableForestBiome : public ForestBiome
{
public:
    using ForestBiome::ForestBiome;

    void chanceToGetTree(int chance)
    {
        mTreeDecider = std::bernoulli_distribution(chance);
    }
};

TEST(ForestBiomeTest, ShouldReturnProperSeed)
{
    constexpr auto seed = 15;
    ForestBiome sut(seed);

    EXPECT_EQ(sut.seed(), seed);
}

TEST(ForestBiomeTest, GeneratesDifferentTerrainOnDifferentSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    ForestBiome sut1(15);
    ForestBiome sut2(16);

    EXPECT_NE(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(ForestBiomeTest, GeneratesSameTerrainOnSameSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    ForestBiome sut1(15);
    ForestBiome sut2(15);

    EXPECT_EQ(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(ForestBiomeTest, ReturnsCorrectBiomeId)
{
    constexpr auto seed = 15;
    ForestBiome sut(seed);

    EXPECT_EQ(sut.biomeId(), BiomeId::Forest);
}

TEST(ForestBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksAboveSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = ForestBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableForestBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::Grass, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(ForestBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksOneBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = ForestBiome::SEA_LEVEL - 1;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableForestBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::Sand, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(ForestBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = ForestBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableForestBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks = std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::Sand,
                                                 BlockId::Stone, BlockId::Water};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

// TODO: Tree tests should check using MockChunk if cactus is placed for real

TEST(ForestBiomeTest, LogOfTreeShouldBePlacedAboveSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = ForestBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableForestBiome sut(15);
    sut.chanceToGetTree(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_TRUE(setOfBlocks.contains(BlockId::Log));
}

TEST(ForestBiomeTest, LogOfTreeShouldNotBePlacedBelowSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = ForestBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableForestBiome sut(15);
    sut.chanceToGetTree(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_FALSE(setOfBlocks.contains(BlockId::Log));
}


}// namespace