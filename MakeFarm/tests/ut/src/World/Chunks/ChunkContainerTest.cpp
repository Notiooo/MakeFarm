#include "World/Chunks/ChunkContainer.h"
#include "World/Chunks/MockChunk.h"
#include "gtest/gtest.h"

namespace
{

class ChunkContainerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mockChunk1 = std::make_shared<MockChunk>();
        mockChunk2 = std::make_shared<MockChunk>();
        sut.emplace(ChunkContainer::Coordinate(0, 0, 0), mockChunk1);
        sut.emplace(ChunkContainer::Coordinate(0, 0, 1), mockChunk2);
    }

    std::shared_ptr<MockChunk> mockChunk1;
    std::shared_ptr<MockChunk> mockChunk2;
    ChunkContainer sut;
    Renderer3D renderer3D;
    sf::Shader shader;
};

TEST_F(ChunkContainerTest, drawTerrainShouldDrawTerrainOfAllChunks)
{
    EXPECT_CALL(*mockChunk1, drawTerrain(testing::_, testing::_));
    EXPECT_CALL(*mockChunk2, drawTerrain(testing::_, testing::_));
    sut.drawTerrain(renderer3D, shader);
}

TEST_F(ChunkContainerTest, drawLiquidsShouldDrawLiquidsOfAllChunks)
{
    EXPECT_CALL(*mockChunk1, drawLiquids(testing::_, testing::_));
    EXPECT_CALL(*mockChunk2, drawLiquids(testing::_, testing::_));
    sut.drawLiquids(renderer3D, shader);
}

TEST_F(ChunkContainerTest, drawFloralsShouldDrawFloralsOfAllChunks)
{
    EXPECT_CALL(*mockChunk1, drawFlorals(testing::_, testing::_));
    EXPECT_CALL(*mockChunk2, drawFlorals(testing::_, testing::_));
    sut.drawFlorals(renderer3D, shader);
}

// TODO: Update Test

TEST_F(ChunkContainerTest, ShouldReturnExistingWorldBlockAtOriginChunk)
{
    auto blockCoordinate = Block::Coordinate(1, 5, 2);
    Block sampleBlock;
    EXPECT_CALL(*mockChunk1, globalToLocalCoordinates(blockCoordinate))
        .WillOnce(testing::Return(blockCoordinate));
    EXPECT_CALL(testing::Const(*mockChunk1), localBlock(blockCoordinate))
        .WillOnce(testing::ReturnRef(sampleBlock));
    auto worldBlock = sut.worldBlock(blockCoordinate);
    EXPECT_EQ(worldBlock, &sampleBlock);
}

TEST_F(ChunkContainerTest, ShouldReturnExistingWorldBlockAtNotOriginChunk)
{
    auto blockCoordinate = Block::Coordinate(1, 5, 16);
    auto localBlockCoordinate = Block::Coordinate(1, 5, 0);
    Block sampleBlock;
    EXPECT_CALL(*mockChunk2, globalToLocalCoordinates(blockCoordinate))
        .WillOnce(testing::Return(localBlockCoordinate));
    EXPECT_CALL(testing::Const(*mockChunk2), localBlock(localBlockCoordinate))
        .WillOnce(testing::ReturnRef(sampleBlock));
    auto worldBlock = sut.worldBlock(blockCoordinate);
    EXPECT_NE(worldBlock, nullptr);
}

TEST_F(ChunkContainerTest, ShouldNotReturnNonExistingWorldBlock)
{
    auto blockCoordinate = Block::Coordinate(-1, 5, 0);
    auto worldBlock = sut.worldBlock(blockCoordinate);
    EXPECT_EQ(worldBlock, nullptr);
}

TEST_F(ChunkContainerTest, ShouldCorrectlyIndicateThatWorldBlockExist)
{
    EXPECT_TRUE(sut.doesWorldBlockExist(Block::Coordinate(1, 5, 2)));
    EXPECT_TRUE(sut.doesWorldBlockExist(Block::Coordinate(1, 5, 16)));
}

TEST_F(ChunkContainerTest, ShouldCorrectlyIndicateThatWorldBlockDoesNotExist)
{
    EXPECT_FALSE(sut.doesWorldBlockExist(Block::Coordinate(-1, 5, 2)));
    EXPECT_FALSE(sut.doesWorldBlockExist(Block::Coordinate(1, 5, -1)));
}

TEST_F(ChunkContainerTest, ShouldReturnCorrectChunkFromBlockPosition)
{
    EXPECT_EQ(sut.blockPositionToChunk(Block::Coordinate(1, 5, 2)), mockChunk1);
    EXPECT_EQ(sut.blockPositionToChunk(Block::Coordinate(1, 5, 16)), mockChunk2);
}

TEST_F(ChunkContainerTest, ShouldReturnNullptrWhenGivenNonExistingBlockPosition)
{
    EXPECT_EQ(sut.blockPositionToChunk(Block::Coordinate(-1, 5, 2)), nullptr);
    EXPECT_EQ(sut.blockPositionToChunk(Block::Coordinate(1, 5, -1)), nullptr);
}

TEST_F(ChunkContainerTest, ShouldCorrectlyRemoveWorldBlockAtOrigin)
{
    auto blockCoordinate = Block::Coordinate(1, 5, 2);
    EXPECT_CALL(*mockChunk1, globalToLocalCoordinates(blockCoordinate))
        .WillOnce(testing::Return(blockCoordinate));
    EXPECT_CALL(*mockChunk1, removeLocalBlock(blockCoordinate));
    EXPECT_CALL(*mockChunk1, directionOfBlockFacesInContactWithOtherChunk(blockCoordinate))
        .WillOnce(testing::Return(std::vector<Direction>{Direction::Above, Direction::ToTheLeft}));
    EXPECT_CALL(*mockChunk1, localNearbyBlockPosition(blockCoordinate, Direction::Above))
        .WillOnce(testing::Return(Block::Coordinate(1, 6, 2)));
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(1, 6, 2)))
        .WillOnce(testing::Return(Block::Coordinate(1, 6, 2)));
    EXPECT_CALL(*mockChunk1, localNearbyBlockPosition(blockCoordinate, Direction::ToTheLeft))
        .WillOnce(testing::Return(Block::Coordinate(0, 5, 2)));
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(0, 5, 2)))
        .WillOnce(testing::Return(Block::Coordinate(0, 5, 2)));
    EXPECT_CALL(*mockChunk1, rebuildFast()).Times(2);

    sut.removeWorldBlock(blockCoordinate);
}

TEST_F(ChunkContainerTest, ShouldCorrectlyRemoveWorldBlockSomewhereElseThanOrigin)
{
    auto blockCoordinate = Block::Coordinate(1, 5, 16);
    EXPECT_CALL(*mockChunk2, globalToLocalCoordinates(blockCoordinate))
        .WillOnce(testing::Return(blockCoordinate));
    EXPECT_CALL(*mockChunk2, removeLocalBlock(blockCoordinate));
    EXPECT_CALL(*mockChunk2, directionOfBlockFacesInContactWithOtherChunk(blockCoordinate))
        .WillOnce(testing::Return(std::vector<Direction>{}));

    sut.removeWorldBlock(blockCoordinate);
}

TEST_F(ChunkContainerTest, ShouldCorrectlyNotRemoveWorldBlockWhenChunkDoesNotExist)
{
    auto blockCoordinate = Block::Coordinate(-1, 5, 2);
    EXPECT_CALL(*mockChunk1, removeLocalBlock(blockCoordinate)).Times(0);
    EXPECT_CALL(*mockChunk2, removeLocalBlock(blockCoordinate)).Times(0);
    sut.removeWorldBlock(blockCoordinate);
}

TEST_F(ChunkContainerTest, ShouldReturnCorrectNearbyChunk)
{
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(-1, 0, 0)))
        .WillOnce(testing::Return(Block::Coordinate(-1, 0, 0)));
    EXPECT_EQ(sut.chunkNearby(*mockChunk1, Direction::ToTheLeft), nullptr);

    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(16, 0, 0)))
        .WillOnce(testing::Return(Block::Coordinate(16, 0, 0)));
    EXPECT_EQ(sut.chunkNearby(*mockChunk1, Direction::ToTheRight), nullptr);

    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(0, 0, 16)))
        .WillOnce(testing::Return(Block::Coordinate(0, 0, 16)));
    EXPECT_EQ(sut.chunkNearby(*mockChunk1, Direction::InFront), mockChunk2);

    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(0, 0, -1)))
        .WillOnce(testing::Return(Block::Coordinate(0, 0, -1)));
    EXPECT_EQ(sut.chunkNearby(*mockChunk1, Direction::Behind), nullptr);
}

TEST_F(ChunkContainerTest, ShouldThrowOnIncorrectDirection)
{
    EXPECT_THROW(sut.chunkNearby(*mockChunk1, Direction::Counter), std::runtime_error);
}

// TODO: Cover there more testcases in rebuilding
TEST_F(ChunkContainerTest, ShouldRebuildChunksAround)
{
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(-1, 0, 0)))
        .WillOnce(testing::Return(Block::Coordinate(-1, 0, 0)));
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(16, 0, 0)))
        .WillOnce(testing::Return(Block::Coordinate(16, 0, 0)));
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(0, 0, 16)))
        .WillOnce(testing::Return(Block::Coordinate(0, 0, 16)));
    EXPECT_CALL(*mockChunk1, localToGlobalCoordinates(Block::Coordinate(0, 0, -1)))
        .WillOnce(testing::Return(Block::Coordinate(0, 0, -1)));
    EXPECT_CALL(*mockChunk2, rebuildSlow());
    sut.rebuildChunksAround(ChunkContainer::Coordinate(0, 0, 0));
}

TEST_F(ChunkContainerTest, ShouldPlaceBlockToExistingChunk)
{
    auto blockId = BlockId::Grass;
    auto blockCoordinate = Block::Coordinate(1, 5, 2);
    auto blockOverplaced = std::vector<BlockId>{BlockId::AllBlocks};
    auto rebuildOperation = RebuildOperation::None;

    EXPECT_CALL(*mockChunk1, globalToLocalCoordinates(blockCoordinate))
        .WillOnce(testing::Return(blockCoordinate));
    EXPECT_CALL(*mockChunk1,
                tryToPlaceBlock(blockId, blockCoordinate, blockOverplaced, rebuildOperation));

    sut.tryToPlaceBlock(blockId, blockCoordinate, blockOverplaced, rebuildOperation);
}

TEST_F(ChunkContainerTest, ShouldNotPlaceBlockToNonExistingChunk)
{
    auto blockId = BlockId::Grass;
    auto blockCoordinate = Block::Coordinate(-1, 5, 2);
    auto blockOverplaced = std::vector<BlockId>{BlockId::AllBlocks};
    auto rebuildOperation = RebuildOperation::None;

    EXPECT_CALL(*mockChunk1,
                tryToPlaceBlock(blockId, blockCoordinate, blockOverplaced, rebuildOperation))
        .Times(0);

    sut.tryToPlaceBlock(blockId, blockCoordinate, blockOverplaced, rebuildOperation);
}

TEST_F(ChunkContainerTest, ShouldReturnProperSize)
{
    EXPECT_EQ(sut.size(), 2);
}

TEST_F(ChunkContainerTest, ShouldReturnZeroSize)
{
    ChunkContainer newSut;
    EXPECT_EQ(newSut.size(), 0);
}

TEST_F(ChunkContainerTest, ShouldNotBeEmpty)
{
    EXPECT_FALSE(sut.isEmpty());
}

TEST_F(ChunkContainerTest, ShouldBeEmpty)
{
    ChunkContainer newSut;
    EXPECT_TRUE(newSut.isEmpty());
}


}// namespace