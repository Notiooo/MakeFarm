#include "World/Biomes/SnowyBiome.h"
#include "TestUtils/BiomeTestUtils.h"
#include "gtest/gtest.h"

namespace
{

class UglyTestableSnowyBiome : public SnowyBiome
{
public:
    using SnowyBiome::SnowyBiome;

    void chanceToGetTree(int chance)
    {
        mTreeDecider = std::bernoulli_distribution(chance);
    }
};

TEST(SnowyBiomeTest, ShouldReturnProperSeed)
{
    constexpr auto seed = 15;
    SnowyBiome sut(seed);

    EXPECT_EQ(sut.seed(), seed);
}

TEST(SnowyBiomeTest, GeneratesDifferentTerrainOnDifferentSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    SnowyBiome sut1(15);
    SnowyBiome sut2(16);

    EXPECT_NE(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(SnowyBiomeTest, GeneratesSameTerrainOnSameSeeds)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    SnowyBiome sut1(15);
    SnowyBiome sut2(15);

    EXPECT_EQ(sut1.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z),
              sut2.surfaceLevelAtGivenPosition(coordinate.x, coordinate.z));
}

TEST(SnowyBiomeTest, ReturnsCorrectBiomeId)
{
    constexpr auto seed = 15;
    SnowyBiome sut(seed);

    EXPECT_EQ(sut.biomeId(), BiomeId::Snowy);
}

TEST(SnowyBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksAboveSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = SnowyBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableSnowyBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::SnowyGrass, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(SnowyBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksOneBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = SnowyBiome::SEA_LEVEL - 1;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableSnowyBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks =
        std::set<BlockId>{BlockId::Air, BlockId::Dirt, BlockId::SnowyGrass, BlockId::Stone};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

TEST(SnowyBiomeTest, GeneratedTerrainContainsCorrectTypeOfBlocksBelowSeaLevel)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = SnowyBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableSnowyBiome sut(15);
    sut.chanceToGetTree(0);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    auto expectedSetOfBlocks = std::set<BlockId>{BlockId::Air,   BlockId::Dirt,  BlockId::Sand,
                                                 BlockId::Stone, BlockId::Water, BlockId::Ice};
    EXPECT_EQ(setOfBlocks, expectedSetOfBlocks);
}

// TODO: Tree tests should check using MockChunk if cactus is placed for real

TEST(SnowyBiomeTest, LogOfTreeShouldBePlacedAboveSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = SnowyBiome::SEA_LEVEL + 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableSnowyBiome sut(15);
    sut.chanceToGetTree(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_TRUE(setOfBlocks.contains(BlockId::SpruceLog));
}

TEST(SnowyBiomeTest, LogOfTreeShouldNotBePlacedBelowSea)
{
    auto coordinate = sf::Vector3f(5, 0, 6);
    auto surfaceLevel = SnowyBiome::SEA_LEVEL - 10;
    Chunk::ChunkBlocks chunkBlocks;
    UglyTestableSnowyBiome sut(15);
    sut.chanceToGetTree(1);

    sut.generateColumnOfBlocks(chunkBlocks, surfaceLevel, coordinate.x, coordinate.z);
    auto setOfBlocks = allBlocksInGivenColumnOfChunk(chunkBlocks, coordinate.x, coordinate.z);
    EXPECT_FALSE(setOfBlocks.contains(BlockId::SpruceLog));
}


}// namespace