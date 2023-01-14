#include "World/Block/Block.h"
#include "gtest/gtest.h"

namespace
{

TEST(BlockCoordinateTest, CoordinatesAtOriginShouldReturnSameNonBlockCoordinates)
{
    Block::Coordinate coordinate{0, 0, 0};
    EXPECT_EQ(coordinate.nonBlockMetric(), sf::Vector3i(0, 0, 0));
}

TEST(BlockCoordinateTest, OperatorOfAdditionShouldWorkProperly)
{
    Block::Coordinate coordinateOrigin{0, 0, 0};
    EXPECT_EQ(coordinateOrigin + Block::Coordinate(1, 1, 1), Block::Coordinate(1, 1, 1));
    EXPECT_EQ(coordinateOrigin + Block::Coordinate(-1, -1, -1), Block::Coordinate(-1, -1, -1));

    Block::Coordinate coordinatePositive{1, 1, 1};
    EXPECT_EQ(coordinatePositive + Block::Coordinate(1, 1, 1), Block::Coordinate(2, 2, 2));
    EXPECT_EQ(coordinatePositive + Block::Coordinate(-1, -1, -1), Block::Coordinate(0, 0, 0));

    Block::Coordinate coordinateNegative{-1, -1, -1};
    EXPECT_EQ(coordinateNegative + Block::Coordinate(1, 1, 1), Block::Coordinate(0, 0, 0));
    EXPECT_EQ(coordinateNegative + Block::Coordinate(-1, -1, -1), Block::Coordinate(-2, -2, -2));
}

TEST(BlockCoordinateTest, OperatorOfSubtractionShouldWorkProperly)
{
    Block::Coordinate coordinateOrigin{0, 0, 0};
    EXPECT_EQ(coordinateOrigin - Block::Coordinate(1, 1, 1), Block::Coordinate(-1, -1, -1));
    EXPECT_EQ(coordinateOrigin - Block::Coordinate(-1, -1, -1), Block::Coordinate(1, 1, 1));

    Block::Coordinate coordinatePositive{1, 1, 1};
    EXPECT_EQ(coordinatePositive - Block::Coordinate(1, 1, 1), Block::Coordinate(0, 0, 0));
    EXPECT_EQ(coordinatePositive - Block::Coordinate(-1, -1, -1), Block::Coordinate(2, 2, 2));

    Block::Coordinate coordinateNegative{-1, -1, -1};
    EXPECT_EQ(coordinateNegative - Block::Coordinate(1, 1, 1), Block::Coordinate(-2, -2, -2));
    EXPECT_EQ(coordinateNegative - Block::Coordinate(-1, -1, -1), Block::Coordinate(0, 0, 0));
}

TEST(BlockCoordinateTest, OperatorOfAssignmentShouldWorkProperly)
{
    Block::Coordinate coordinate{0, 0, 0};
    coordinate = Block::Coordinate(1, 1, 1);

    EXPECT_EQ(coordinate, Block::Coordinate(1, 1, 1));
}

TEST(BlockCoordinateTest, ComparisonShouldWorkProperly)
{
    EXPECT_NE(Block::Coordinate(0, 0, 0), Block::Coordinate(1, 1, 1));
    EXPECT_NE(Block::Coordinate(-1, -1, -1), Block::Coordinate(1, 1, 1));
    EXPECT_NE(Block::Coordinate(222, 223, 221), Block::Coordinate(221, 223, 221));
    EXPECT_NE(Block::Coordinate(222, 223, 221), Block::Coordinate(222, 222, 221));
    EXPECT_NE(Block::Coordinate(222, 223, 221), Block::Coordinate(222, 223, 220));

    EXPECT_EQ(Block::Coordinate(1, 1, 1), Block::Coordinate(1, 1, 1));
    EXPECT_EQ(Block::Coordinate(-1, -1, -1), Block::Coordinate(-1, -1, -1));
    EXPECT_EQ(Block::Coordinate(0, 0, 0), Block::Coordinate(0, 0, 0));
    EXPECT_EQ(Block::Coordinate(222, 223, 221), Block::Coordinate(222, 223, 221));
}

TEST(BlockCoordinateTest, NonBlockCoordinatesShouldBeCorrect)
{
    Block::Coordinate coordinate{12, 11, 10};
    EXPECT_EQ(coordinate.nonBlockMetric(), sf::Vector3i(12, 11, 10));
}

TEST(BlockCoordinateTest, NonBlockNegativeCoordinatesShouldBeCorrect)
{
    Block::Coordinate coordinate{-12, -11, -10};
    EXPECT_EQ(coordinate.nonBlockMetric(), sf::Vector3i(-12, -11, -10));
}

TEST(BlockCoordinateTest, NonBlockToBlockMetricShouldBeTheSameAtOrigin)
{
    sf::Vector3f blockCoordinate{0, 0, 0};
    auto expectedCoordinate = Block::Coordinate(0, 0, 0);
    EXPECT_EQ(Block::Coordinate::nonBlockToBlockMetric(blockCoordinate), expectedCoordinate);
}

TEST(BlockCoordinateTest, CoordinatesAtEdgeOfTheBlockShouldReturnCorrectBlockCoordinates)
{
    EXPECT_EQ(Block::Coordinate::nonBlockToBlockMetric(sf::Vector3f{0.99, 0.99, 0.99}),
              Block::Coordinate(0, 0, 0));

    EXPECT_EQ(Block::Coordinate::nonBlockToBlockMetric(sf::Vector3f{-0.01, -0.01, -0.01}),
              Block::Coordinate(-1, -1, -1));
}

TEST(BlockCoordinateTest, CoordinatesAtGivenDirectionsShouldBeCorrect)
{
    auto sampleCoordinate = Block::Coordinate(0, 0, 0);
    EXPECT_EQ(sampleCoordinate.coordinateInGivenDirection(Direction::Behind),
              Block::Coordinate(0, 0, -1));

    EXPECT_EQ(sampleCoordinate.coordinateInGivenDirection(Direction::InFront),
              Block::Coordinate(0, 0, 1));

    EXPECT_EQ(sampleCoordinate.coordinateInGivenDirection(Direction::Below),
              Block::Coordinate(0, -1, 0));

    EXPECT_EQ(sampleCoordinate.coordinateInGivenDirection(Direction::ToTheRight),
              Block::Coordinate(1, 0, 0));

    EXPECT_EQ(sampleCoordinate.coordinateInGivenDirection(Direction::ToTheLeft),
              Block::Coordinate(-1, 0, 0));

    EXPECT_EQ(sampleCoordinate.coordinateInGivenDirection(Direction::Above),
              Block::Coordinate(0, 1, 0));
}

TEST(BlockCoordinateTest, FarAwayCoordinatesShouldReturnCorrectBlockCoordinates)
{
    sf::Vector3f coordinate{220.5, 60.6, 200.7};
    auto expectedCoordinate = Block::Coordinate(220, 60, 200);
    EXPECT_EQ(Block::Coordinate::nonBlockToBlockMetric(coordinate), expectedCoordinate);
}

TEST(BlockCoordinateTest, FarAwayNegativeCoordinatesShouldReturnCorrectBlockCoordinates)
{
    sf::Vector3f coordinate{-220.5, -0.7, -200.8};
    auto expectedCoordinate = Block::Coordinate(-221, -1, -201);
    EXPECT_EQ(Block::Coordinate::nonBlockToBlockMetric(coordinate), expectedCoordinate);
}

}// namespace