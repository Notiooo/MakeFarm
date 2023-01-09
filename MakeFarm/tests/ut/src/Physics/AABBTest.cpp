#include "Physics/AABB.h"
#include "gtest/gtest.h"

namespace
{

TEST(AABBTest, TwoAABBSameSizeSamePlaceShouldBeEqual)
{
    auto boxSize = sf::Vector3f{1, 1, 1};
    AABB aabb1(boxSize);
    AABB aabb2(boxSize);
    EXPECT_EQ(aabb1, aabb2);
}

TEST(AABBTest, TwoAABBSameSizeDifferentPlaceShouldNotBeEqual)
{
    auto boxSize = sf::Vector3f{1, 1, 1};
    AABB aabb1(boxSize);
    AABB aabb2(boxSize);
    aabb2.updatePosition(sf::Vector3f{0.01, 0.01, 0.01});
    EXPECT_NE(aabb1, aabb2);
}

TEST(AABBTest, TwoAABBDifferentSizeSamePlaceShouldNotBeEqual)
{
    AABB aabb1(sf::Vector3f{1, 1, 1});
    AABB aabb2(sf::Vector3f{1, 1, 1.1});
    EXPECT_NE(aabb1, aabb2);
}

TEST(AABBTest, TwoAABBSameSizeSamePlaceShouldIntersect)
{
    auto boxSize = sf::Vector3f{1, 1, 1};
    AABB aabb1(boxSize);
    AABB aabb2(boxSize);
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));
}

TEST(AABBTest, TwoAABBSmallerTotallyInsideTheOtherShouldIntesect)
{
    AABB bigger(sf::Vector3f{1, 1, 1});
    AABB smaller(sf::Vector3f{0.5, 0.5, 0.5});
    smaller.updatePosition(sf::Vector3f{0.25, 0.25, 0.25});
    EXPECT_TRUE(bigger.intersect(smaller));
    EXPECT_TRUE(smaller.intersect(bigger));
}

TEST(AABBTest, TwoAABBSameSizeWithSlightOffsetInPositionShouldIntersect)
{
    AABB aabb1(sf::Vector3f{1, 1, 1});
    AABB aabb2(sf::Vector3f{1, 1, 1});
    aabb2.updatePosition(sf::Vector3f{0.25, 0.25, 0.25});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));

    aabb2.updatePosition(sf::Vector3f{-0.25, -0.25, -0.25});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));

    aabb2.updatePosition(sf::Vector3f{-0.25, 0.25, 0.25});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));

    aabb2.updatePosition(sf::Vector3f{0.25, -0.25, 0.25});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));

    aabb2.updatePosition(sf::Vector3f{0.25, 0.25, -0.25});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));
}

TEST(AABBTest, TwoAABBDifferentSizeWithSlightOffsetInPositionShouldIntersect)
{
    AABB aabb1(sf::Vector3f{1, 1, 1});
    AABB aabb2(sf::Vector3f{2, 2, 2});
    aabb2.updatePosition(sf::Vector3f{0.25, 0.25, 0.25});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));

    aabb2.updatePosition(sf::Vector3f{-1.75, -1.75, -1.75});
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));
}

class AABBPositionAtTheCornersAndSidesFixture : public ::testing::TestWithParam<sf::Vector3f>
{
};

INSTANTIATE_TEST_CASE_P(AABBTest, AABBPositionAtTheCornersAndSidesFixture,
                        ::testing::Values(sf::Vector3f{1, 1, 1}, sf::Vector3f{0, 1, 1},
                                          sf::Vector3f{1, 0, 1}, sf::Vector3f{1, 1, 0},
                                          sf::Vector3f{1, 0, 0}, sf::Vector3f{0, 1, 0},
                                          sf::Vector3f{0, 0, 1}));

TEST_P(AABBPositionAtTheCornersAndSidesFixture,
       TwoAABBExactlyTouchingTheirCornerAndSidesShouldIntersect)
{
    AABB aabb1(sf::Vector3f{1, 1, 1});
    AABB aabb2(sf::Vector3f{1, 1, 1});
    aabb2.updatePosition(GetParam());
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));
}

TEST_P(AABBPositionAtTheCornersAndSidesFixture,
       TwoAABBAlmostTouchingTheirCornerAndSidesShouldNotIntersect)
{
    AABB aabb1(sf::Vector3f{0.999, 0.999, 0.999});
    AABB aabb2(sf::Vector3f{1, 1, 1});
    aabb2.updatePosition(GetParam());
    EXPECT_FALSE(aabb1.intersect(aabb2));
    EXPECT_FALSE(aabb2.intersect(aabb1));
}

TEST_P(AABBPositionAtTheCornersAndSidesFixture,
       TwoAABBAwayInSpaceExactlyTouchingTheirCornerAndSidesShouldIntersect)
{
    auto farAwaySpacePoint = sf::Vector3f{283828.343, 382424.344, 454843.944};
    AABB aabb1(sf::Vector3f{1, 1, 1});
    aabb1.updatePosition(farAwaySpacePoint);
    AABB aabb2(sf::Vector3f{1, 1, 1});
    aabb2.updatePosition(farAwaySpacePoint + GetParam());
    EXPECT_TRUE(aabb1.intersect(aabb2));
    EXPECT_TRUE(aabb2.intersect(aabb1));
}

TEST_P(AABBPositionAtTheCornersAndSidesFixture,
       TwoAABBAwayInSpaceAlmostTouchingTheirCornerAndSidesShouldNotIntersect)
{
    auto farAwaySpacePoint = sf::Vector3f{283828.343, 382424.344, 454843.944};
    AABB aabb1(sf::Vector3f{0.98, 0.98, 0.98});
    aabb1.updatePosition(farAwaySpacePoint);
    AABB aabb2(sf::Vector3f{1, 1, 1});
    aabb2.updatePosition(farAwaySpacePoint + GetParam());
    EXPECT_FALSE(aabb1.intersect(aabb2));
    EXPECT_FALSE(aabb2.intersect(aabb1));
}

TEST(AABBTest, AABBProperBoxSizeReturned)
{
    auto boxSize = sf::Vector3f{1, 1, 1};
    AABB aabb(boxSize);
    EXPECT_EQ(boxSize, aabb.collisionBoxSize());
}

TEST(AABBTest, AABBProperBoxSizeReturnedAfterPositionUpdate)
{
    auto boxSize = sf::Vector3f{1, 1, 1};
    AABB aabb(boxSize);
    aabb.updatePosition(sf::Vector3f{5, 5, 5});
    EXPECT_EQ(boxSize, aabb.collisionBoxSize());
}

TEST(AABBTest, AABBCopiedProperly)
{
    auto boxSize = sf::Vector3f{1, 1, 1};
    AABB aabb1(boxSize);
    AABB aabb2(aabb1);
    EXPECT_EQ(aabb1.collisionBoxSize(), aabb2.collisionBoxSize());
    EXPECT_EQ(aabb1, aabb2);
}

}// namespace