#include "World/Chunks/CoordinatesAroundOriginGetter.h"
#include "gtest/gtest.h"

namespace
{

TEST(CoordinatesAroundOriginGetterTest, ShouldReturnProperCoordinates)
{
    auto origin = sf::Vector3i{0, 0, 0};
    std::vector<sf::Vector3i> nextCoordinates = {
        {0, 0, 0},   {1, 0, 0},   {1, 0, 1},   {0, 0, 1},  {-1, 0, 1}, {-1, 0, 0},  {-1, 0, -1},
        {0, 0, -1},  {1, 0, -1},  {2, 0, -1},  {2, 0, 0},  {2, 0, 1},  {2, 0, 2},   {1, 0, 2},
        {0, 0, 2},   {-1, 0, 2},  {-2, 0, 2},  {-2, 0, 1}, {-2, 0, 0}, {-2, 0, -1}, {-2, 0, -2},
        {-1, 0, -2}, {0, 0, -2},  {1, 0, -2},  {2, 0, -2}, {3, 0, -2}, {3, 0, -1},  {3, 0, 0},
        {3, 0, 1},   {3, 0, 2},   {3, 0, 3},   {2, 0, 3},  {1, 0, 3},  {0, 0, 3},   {-1, 0, 3},
        {-2, 0, 3},  {-3, 0, 3},  {-3, 0, 2},  {-3, 0, 1}, {-3, 0, 0}, {-3, 0, -1}, {-3, 0, -2},
        {-3, 0, -3}, {-2, 0, -3}, {-1, 0, -3}, {0, 0, -3}, {1, 0, -3}, {2, 0, -3},  {3, 0, -3},
        {4, 0, -3},  {4, 0, -2},  {4, 0, -1},  {4, 0, 0},  {4, 0, 1},  {4, 0, 2},   {4, 0, 3}};
    CoordinatesAroundOriginGetter sut(origin);

    for (auto coordinate: nextCoordinates)
    {
        EXPECT_EQ(sut.nextValue(), coordinate);
    }
}

TEST(CoordinatesAroundOriginGetterTest, ShouldReturnProperCoordinatesAfterReset)
{
    auto origin = sf::Vector3i{0, 0, 0};
    std::vector<sf::Vector3i> nextCoordinates = {
        {0, 0, 0},   {1, 0, 0},   {1, 0, 1},   {0, 0, 1},  {-1, 0, 1}, {-1, 0, 0},  {-1, 0, -1},
        {0, 0, -1},  {1, 0, -1},  {2, 0, -1},  {2, 0, 0},  {2, 0, 1},  {2, 0, 2},   {1, 0, 2},
        {0, 0, 2},   {-1, 0, 2},  {-2, 0, 2},  {-2, 0, 1}, {-2, 0, 0}, {-2, 0, -1}, {-2, 0, -2},
        {-1, 0, -2}, {0, 0, -2},  {1, 0, -2},  {2, 0, -2}, {3, 0, -2}, {3, 0, -1},  {3, 0, 0},
        {3, 0, 1},   {3, 0, 2},   {3, 0, 3},   {2, 0, 3},  {1, 0, 3},  {0, 0, 3},   {-1, 0, 3},
        {-2, 0, 3},  {-3, 0, 3},  {-3, 0, 2},  {-3, 0, 1}, {-3, 0, 0}, {-3, 0, -1}, {-3, 0, -2},
        {-3, 0, -3}, {-2, 0, -3}, {-1, 0, -3}, {0, 0, -3}, {1, 0, -3}, {2, 0, -3},  {3, 0, -3},
        {4, 0, -3},  {4, 0, -2},  {4, 0, -1},  {4, 0, 0},  {4, 0, 1},  {4, 0, 2},   {4, 0, 3}};
    CoordinatesAroundOriginGetter sut(origin);

    for (auto coordinate: nextCoordinates)
    {
        sut.nextValue();
    }

    sut.resetToStart();

    for (auto coordinate: nextCoordinates)
    {
        EXPECT_EQ(sut.nextValue(), coordinate);
    }
}

TEST(CoordinatesAroundOriginGetterTest, ShouldReturnProperCoordinateAfterLongTime)
{
    auto origin = sf::Vector3i{0, 0, 0};
    CoordinatesAroundOriginGetter sut(origin);

    for (int i = 0; i < 300; ++i)
    {
        sut.nextValue();
    }
    auto expectedResult = sf::Vector3i{9, 0, 3};
    EXPECT_EQ(sut.nextValue(), expectedResult);
}

TEST(CoordinatesAroundOriginGetterTest, ShouldReturnProperCoordinatesAtDiffrentOrigin)
{
    auto origin = sf::Vector3i{3, 7, 6};
    std::vector<sf::Vector3i> nextCoordinates = {
        {3, 7, 6}, {4, 7, 6}, {4, 7, 7}, {3, 7, 7}, {2, 7, 7}, {2, 7, 6}, {2, 7, 5}, {3, 7, 5},
        {4, 7, 5}, {5, 7, 5}, {5, 7, 6}, {5, 7, 7}, {5, 7, 8}, {4, 7, 8}, {3, 7, 8}, {2, 7, 8},
        {1, 7, 8}, {1, 7, 7}, {1, 7, 6}, {1, 7, 5}, {1, 7, 4}, {2, 7, 4}, {3, 7, 4}, {4, 7, 4},
        {5, 7, 4}, {6, 7, 4}, {6, 7, 5}, {6, 7, 6}, {6, 7, 7}, {6, 7, 8}, {6, 7, 9}, {5, 7, 9},
        {4, 7, 9}, {3, 7, 9}, {2, 7, 9}, {1, 7, 9}, {0, 7, 9}, {0, 7, 8}, {0, 7, 7}, {0, 7, 6},
        {0, 7, 5}, {0, 7, 4}, {0, 7, 3}, {1, 7, 3}, {2, 7, 3}, {3, 7, 3}, {4, 7, 3}, {5, 7, 3},
        {6, 7, 3}, {7, 7, 3}, {7, 7, 4}, {7, 7, 5}, {7, 7, 6}, {7, 7, 7}, {7, 7, 8}, {7, 7, 9}};
    CoordinatesAroundOriginGetter sut(origin);

    for (auto coordinate: nextCoordinates)
    {
        EXPECT_EQ(sut.nextValue(), coordinate);
    }
}

TEST(CoordinatesAroundOriginGetterTest, ShouldReturnProperCoordinatesAfterResetAtDiffrentOrigin)
{
    auto origin = sf::Vector3i{3, 7, 6};
    std::vector<sf::Vector3i> nextCoordinates = {
        {3, 7, 6}, {4, 7, 6}, {4, 7, 7}, {3, 7, 7}, {2, 7, 7}, {2, 7, 6}, {2, 7, 5}, {3, 7, 5},
        {4, 7, 5}, {5, 7, 5}, {5, 7, 6}, {5, 7, 7}, {5, 7, 8}, {4, 7, 8}, {3, 7, 8}, {2, 7, 8},
        {1, 7, 8}, {1, 7, 7}, {1, 7, 6}, {1, 7, 5}, {1, 7, 4}, {2, 7, 4}, {3, 7, 4}, {4, 7, 4},
        {5, 7, 4}, {6, 7, 4}, {6, 7, 5}, {6, 7, 6}, {6, 7, 7}, {6, 7, 8}, {6, 7, 9}, {5, 7, 9},
        {4, 7, 9}, {3, 7, 9}, {2, 7, 9}, {1, 7, 9}, {0, 7, 9}, {0, 7, 8}, {0, 7, 7}, {0, 7, 6},
        {0, 7, 5}, {0, 7, 4}, {0, 7, 3}, {1, 7, 3}, {2, 7, 3}, {3, 7, 3}, {4, 7, 3}, {5, 7, 3},
        {6, 7, 3}, {7, 7, 3}, {7, 7, 4}, {7, 7, 5}, {7, 7, 6}, {7, 7, 7}, {7, 7, 8}, {7, 7, 9}};
    CoordinatesAroundOriginGetter sut(origin);

    for (auto coordinate: nextCoordinates)
    {
        sut.nextValue();
    }

    sut.resetToStart();

    for (auto coordinate: nextCoordinates)
    {
        EXPECT_EQ(sut.nextValue(), coordinate);
    }
}


}// namespace