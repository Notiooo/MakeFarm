#include "World/Chunks/ChunkContainer.h"
#include "gtest/gtest.h"

namespace
{

TEST(ChunkContainerCoordinateTest, CoordinatesAtOriginShouldReturnSameNonChunkCoordinates)
{
    ChunkContainer::Coordinate coordinate{0, 0, 0};
    EXPECT_EQ(coordinate.nonChunkMetric(), sf::Vector3i(0, 0, 0));
}

TEST(ChunkContainerCoordinateTest, NonChunkCoordinatesShouldBeCorrect)
{
    ChunkContainer::Coordinate coordinate{12, 11, 10};
    EXPECT_EQ(coordinate.nonChunkMetric(), sf::Vector3i(12 * 16, 11 * 127, 10 * 16));
}

TEST(ChunkContainerCoordinateTest, BlockToChunkMetricShouldBeTheSameAtOrigin)
{
    Block::Coordinate blockCoordinate{0, 0, 0};
    auto expectedCoordinate = ChunkContainer::Coordinate(0, 0, 0);
    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(blockCoordinate), expectedCoordinate);
}

TEST(ChunkContainerCoordinateTest, BlockInsideChunkAtOriginShouldReturnOriginChunkCoordinates)
{
    Block::Coordinate blockCoordinate{12, 11, 10};
    auto expectedCoordinate = ChunkContainer::Coordinate(0, 0, 0);
    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(blockCoordinate), expectedCoordinate);
}

TEST(ChunkContainerCoordinateTest, BlockInsideEdgeOfChunkShouldReturnCorrectChunkCoordinates)
{
    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(Block::Coordinate{15, 125, 15}),
              ChunkContainer::Coordinate(0, 0, 0));

    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(Block::Coordinate{-1, 125, -1}),
              ChunkContainer::Coordinate(-1, 0, -1));

    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(Block::Coordinate{-16, 125, -16}),
              ChunkContainer::Coordinate(-1, 0, -1));
}

TEST(ChunkContainerCoordinateTest, BlockInAwayChunkShouldReturnCorrectChunkCoordinates)
{
    Block::Coordinate blockCoordinate{220, 60, 200};
    auto expectedCoordinate = ChunkContainer::Coordinate(13, 0, 12);
    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(blockCoordinate), expectedCoordinate);
}

TEST(ChunkContainerCoordinateTest,
     BlockInAwayChunkAtNegativeCoordinatesShouldReturnCorrectChunkCoordinates)
{
    Block::Coordinate blockCoordinate{-220, 60, -200};
    auto expectedCoordinate = ChunkContainer::Coordinate(-14, 0, -13);
    EXPECT_EQ(ChunkContainer::Coordinate::blockToChunkMetric(blockCoordinate), expectedCoordinate);
}

TEST(ChunkContainerCoordinateTest, OperatorOfAdditionShouldWorkProperly)
{
    ChunkContainer::Coordinate coordinateOrigin{0, 0, 0};
    EXPECT_EQ(coordinateOrigin + ChunkContainer::Coordinate(1, 1, 1),
              ChunkContainer::Coordinate(1, 1, 1));
    EXPECT_EQ(coordinateOrigin + ChunkContainer::Coordinate(-1, -1, -1),
              ChunkContainer::Coordinate(-1, -1, -1));

    ChunkContainer::Coordinate coordinatePositive{1, 1, 1};
    EXPECT_EQ(coordinatePositive + ChunkContainer::Coordinate(1, 1, 1),
              ChunkContainer::Coordinate(2, 2, 2));
    EXPECT_EQ(coordinatePositive + ChunkContainer::Coordinate(-1, -1, -1),
              ChunkContainer::Coordinate(0, 0, 0));

    ChunkContainer::Coordinate coordinateNegative{-1, -1, -1};
    EXPECT_EQ(coordinateNegative + ChunkContainer::Coordinate(1, 1, 1),
              ChunkContainer::Coordinate(0, 0, 0));
    EXPECT_EQ(coordinateNegative + ChunkContainer::Coordinate(-1, -1, -1),
              ChunkContainer::Coordinate(-2, -2, -2));
}

TEST(ChunkContainerCoordinateTest, OperatorOfSubtractionShouldWorkProperly)
{
    ChunkContainer::Coordinate coordinateOrigin{0, 0, 0};
    EXPECT_EQ(coordinateOrigin - ChunkContainer::Coordinate(1, 1, 1),
              ChunkContainer::Coordinate(-1, -1, -1));
    EXPECT_EQ(coordinateOrigin - ChunkContainer::Coordinate(-1, -1, -1),
              ChunkContainer::Coordinate(1, 1, 1));

    ChunkContainer::Coordinate coordinatePositive{1, 1, 1};
    EXPECT_EQ(coordinatePositive - ChunkContainer::Coordinate(1, 1, 1),
              ChunkContainer::Coordinate(0, 0, 0));
    EXPECT_EQ(coordinatePositive - ChunkContainer::Coordinate(-1, -1, -1),
              ChunkContainer::Coordinate(2, 2, 2));

    ChunkContainer::Coordinate coordinateNegative{-1, -1, -1};
    EXPECT_EQ(coordinateNegative - ChunkContainer::Coordinate(1, 1, 1),
              ChunkContainer::Coordinate(-2, -2, -2));
    EXPECT_EQ(coordinateNegative - ChunkContainer::Coordinate(-1, -1, -1),
              ChunkContainer::Coordinate(0, 0, 0));
}

TEST(ChunkContainerCoordinateTest, OperatorOfAssignmentShouldWorkProperly)
{
    ChunkContainer::Coordinate coordinate{0, 0, 0};
    coordinate = ChunkContainer::Coordinate(1, 1, 1);

    EXPECT_EQ(coordinate, ChunkContainer::Coordinate(1, 1, 1));
}

TEST(ChunkContainerCoordinateTest, ComparisonShouldWorkProperly)
{
    EXPECT_NE(ChunkContainer::Coordinate(0, 0, 0), ChunkContainer::Coordinate(1, 1, 1));
    EXPECT_NE(ChunkContainer::Coordinate(-1, -1, -1), ChunkContainer::Coordinate(1, 1, 1));
    EXPECT_NE(ChunkContainer::Coordinate(222, 223, 221), ChunkContainer::Coordinate(221, 223, 221));
    EXPECT_NE(ChunkContainer::Coordinate(222, 223, 221), ChunkContainer::Coordinate(222, 222, 221));
    EXPECT_NE(ChunkContainer::Coordinate(222, 223, 221), ChunkContainer::Coordinate(222, 223, 220));

    EXPECT_EQ(ChunkContainer::Coordinate(1, 1, 1), ChunkContainer::Coordinate(1, 1, 1));
    EXPECT_EQ(ChunkContainer::Coordinate(-1, -1, -1), ChunkContainer::Coordinate(-1, -1, -1));
    EXPECT_EQ(ChunkContainer::Coordinate(0, 0, 0), ChunkContainer::Coordinate(0, 0, 0));
    EXPECT_EQ(ChunkContainer::Coordinate(222, 223, 221), ChunkContainer::Coordinate(222, 223, 221));
}

}// namespace