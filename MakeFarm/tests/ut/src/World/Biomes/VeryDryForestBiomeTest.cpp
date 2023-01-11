#include "World/Biomes/VeryDryForestBiome.h"
#include "TestUtils/BiomeTestUtils.h"
#include "gtest/gtest.h"

namespace
{

class UglyTestableVeryDryForestBiome : public VeryDryForestBiome
{
public:
    using VeryDryForestBiome::VeryDryForestBiome;

    void chanceToGetTree(int chance)
    {
        mDryTreeDecider = std::bernoulli_distribution(chance);
    }
};

TEST(VeryDryForestBiomeTest, ShouldReturnProperSeed)
{
    constexpr auto seed = 15;
    VeryDryForestBiome sut(seed);

    EXPECT_EQ(sut.seed(), seed);
}

TEST(VeryDryForestBiomeTest, GeneratesDifferentTerrainOnDifferentSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    VeryDryForestBiome sut1(15);
    VeryDryForestBiome sut2(16);

    EXPECT_NE(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(VeryDryForestBiomeTest, GeneratesSameTerrainOnSameSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    VeryDryForestBiome sut1(15);
    VeryDryForestBiome sut2(15);

    EXPECT_EQ(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(VeryDryForestBiomeTest, ReturnsCorrectBiomeId)
{
    constexpr auto seed = 15;
    VeryDryForestBiome sut(seed);

    EXPECT_EQ(sut.biomeId(), BiomeId::VeryDryForest);
}

TEST(VeryDryForestBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksAboveSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = VeryDryForestBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableVeryDryForestBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::DryGrass, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(VeryDryForestBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksOneBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = VeryDryForestBiome::SEA_LEVEL - 1;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableVeryDryForestBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::Sand, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(VeryDryForestBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = VeryDryForestBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableVeryDryForestBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks = std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::Sand,
                                                 BlockId::Stone, BlockId::Water};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

// TODO: Tree tests should check using MockChunk if cactus is placed for real

TEST(VeryDryForestBiomeTest, LogOfTreeShouldBePlacedAboveSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = VeryDryForestBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableVeryDryForestBiome sut(15);
    sut.chanceToGetTree(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_TRUE(setOfBlocks.contains(BlockId::Log));
}

TEST(VeryDryForestBiomeTest, LogOfTreeShouldNotBePlacedBelowSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = VeryDryForestBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableVeryDryForestBiome sut(15);
    sut.chanceToGetTree(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_FALSE(setOfBlocks.contains(BlockId::Log));
}


}// namespace