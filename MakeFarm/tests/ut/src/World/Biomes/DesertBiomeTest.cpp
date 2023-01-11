#include "World/Biomes/DesertBiome.h"
#include "TestUtils/BiomeTestUtils.h"
#include "gtest/gtest.h"

namespace
{

class UglyTestableDesertBiome : public DesertBiome
{
public:
    using DesertBiome::DesertBiome;

    void chanceToGetCactus(int chance)
    {
        mCactusDecider = std::bernoulli_distribution(chance);
    }
};

TEST(DesertBiomeTest, ShouldReturnProperSeed)
{
    constexpr auto seed = 15;
    DesertBiome sut(seed);

    EXPECT_EQ(sut.seed(), seed);
}

TEST(DesertBiomeTest, GeneratesDifferentTerrainOnDifferentSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    DesertBiome sut1(15);
    DesertBiome sut2(16);

    EXPECT_NE(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(DesertBiomeTest, GeneratesSameTerrainOnSameSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    DesertBiome sut1(15);
    DesertBiome sut2(15);

    EXPECT_EQ(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(DesertBiomeTest, ReturnsCorrectBiomeId)
{
    constexpr auto seed = 15;
    DesertBiome sut(seed);

    EXPECT_EQ(sut.biomeId(), BiomeId::Desert);
}

TEST(DesertBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksAboveSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = DesertBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableDesertBiome sut(15);
    sut.chanceToGetCactus(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks = std::set<BlockId>{BlockId::Air, BlockId::Sand, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(DesertBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = DesertBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableDesertBiome sut(15);
    sut.chanceToGetCactus(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Sand, BlockId::Stone, BlockId::Water};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

// TODO: Cactus tests should check using MockChunk if cactus is placed for real

TEST(DesertBiomeTest, CactusShouldBePlacedAboveSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = DesertBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableDesertBiome sut(15);
    sut.chanceToGetCactus(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_TRUE(setOfBlocks.contains(BlockId::Cactus));
}

TEST(DesertBiomeTest, CactusShouldNotBePlacedBelowSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = DesertBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableDesertBiome sut(15);
    sut.chanceToGetCactus(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_FALSE(setOfBlocks.contains(BlockId::Cactus));
}


}// namespace