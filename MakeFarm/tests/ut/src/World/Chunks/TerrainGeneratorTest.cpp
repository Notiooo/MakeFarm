#include "World/Chunks/TerrainGenerator.h"
#include "World/Chunks/MockChunk.h"
#include "gtest/gtest.h"

namespace
{

TEST(TerrainGeneratorTest, RandomSetShouldReturnDifferentValue)
{
    // Okay, I expect that this is an unstable test.
    // But what is the chance that they will be the same? D:
    EXPECT_NE(TerrainGenerator::randomSeed(), TerrainGenerator::randomSeed());
}

TEST(TerrainGeneratorTest, ShouldGenerateTerrainForChunkWithOnlyOneBiome)
{
    ChunkInterface::ChunkBlocks chunkBlocks;
    MockChunk mockChunk;
    TerrainGenerator sut(15);

    testing::InSequence seq;

    for (auto x = 0; x < ChunkInterface::BLOCKS_PER_X_DIMENSION + 2; ++x)
    {
        for (auto z = 0; z < ChunkInterface::BLOCKS_PER_Z_DIMENSION + 2; ++z)
        {
            Block::Coordinate coordinate{x - 1, 0, z - 1};
            EXPECT_CALL(mockChunk, localToGlobalCoordinates(coordinate))
                .WillOnce(testing::Return(coordinate));
        }
    }

    for (auto x = 0; x < ChunkInterface::BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto z = 0; z < ChunkInterface::BLOCKS_PER_Z_DIMENSION; ++z)
        {
            Block::Coordinate coordinate{x, 0, z};
            EXPECT_CALL(mockChunk, localToGlobalCoordinates(coordinate))
                .WillOnce(testing::Return(coordinate));
        }
    }

    EXPECT_CALL(mockChunk, tryToPlaceBlock(testing::_, testing::_, testing::_, testing::_))
        .Times(testing::AtLeast(0));

    sut.generateTerrain(mockChunk, chunkBlocks);

    for (auto x = 0; x < ChunkInterface::BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto y = 0; y < ChunkInterface::BLOCKS_PER_Y_DIMENSION; ++y)
        {
            for (auto z = 0; z < ChunkInterface::BLOCKS_PER_Z_DIMENSION; ++z)
            {
                EXPECT_NE(chunkBlocks[x][y][z], nullptr);
            }
        }
    }
}

TEST(TerrainGeneratorTest, ShouldGenerateTerrainForChunkWithDifferentBiomes)
{
    ChunkInterface::ChunkBlocks chunkBlocks;
    MockChunk mockChunk;
    TerrainGenerator sut(15);

    testing::InSequence seq;

    for (auto x = 0; x < ChunkInterface::BLOCKS_PER_X_DIMENSION + 2; ++x)
    {
        for (auto z = 0; z < ChunkInterface::BLOCKS_PER_Z_DIMENSION + 2; ++z)
        {
            Block::Coordinate coordinateToExpect{x - 1, 0, z - 1};
            Block::Coordinate coordReturn{(x - 1) + ChunkInterface::BLOCKS_PER_X_DIMENSION * 12, 0,
                                          ChunkInterface::BLOCKS_PER_Z_DIMENSION * 12};
            EXPECT_CALL(mockChunk, localToGlobalCoordinates(coordinateToExpect))
                .WillOnce(testing::Return(coordReturn));
        }
    }

    EXPECT_CALL(mockChunk, localToGlobalCoordinates(testing::_))
        .Times(4 * 4)// 4 blocks at 4 corners
        .WillRepeatedly(testing::Return(Block::Coordinate{0, 0, 0}));

    EXPECT_CALL(mockChunk, tryToPlaceBlock(testing::_, testing::_, testing::_, testing::_))
        .Times(testing::AtLeast(0));

    sut.generateTerrain(mockChunk, chunkBlocks);

    for (auto x = 0; x < ChunkInterface::BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto y = 0; y < ChunkInterface::BLOCKS_PER_Y_DIMENSION; ++y)
        {
            for (auto z = 0; z < ChunkInterface::BLOCKS_PER_Z_DIMENSION; ++z)
            {
                EXPECT_NE(chunkBlocks[x][y][z], nullptr);
            }
        }
    }
}

}// namespace