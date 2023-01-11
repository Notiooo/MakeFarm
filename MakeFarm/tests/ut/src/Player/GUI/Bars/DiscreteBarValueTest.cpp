#include "Player/GUI/Bars/DiscreteBarValue.h"
#include "gtest/gtest.h"

namespace
{

TEST(DiscreteBarValueTest, ReturnsProperFloatWhenInitializedAsInteger)
{
    DiscreteBarValue discreteBarValue(10);
    EXPECT_EQ(static_cast<float>(discreteBarValue), 10.f);
}

TEST(DiscreteBarValueTest, ReturnsProperFloatWhenInitializedWithHalf)
{
    DiscreteBarValue discreteBarValue(10.5f);
    EXPECT_EQ(static_cast<float>(discreteBarValue), 10.5f);

    DiscreteBarValue discreteBarValue2(0.5f);
    EXPECT_EQ(static_cast<float>(discreteBarValue2), 0.5f);
}

TEST(DiscreteBarValueTest, RoundsDownWhenInitializedWithSlighlyAboveHalf)
{
    DiscreteBarValue discreteBarValue(10.733333f);
    EXPECT_EQ(static_cast<float>(discreteBarValue), 10.5f);
}

TEST(DiscreteBarValueTest, RoundsUpWhenInitializedWithCloserToFullThanHalf)
{
    DiscreteBarValue discreteBarValue(10.767777f);
    EXPECT_EQ(static_cast<float>(discreteBarValue), 11.f);
}

TEST(DiscreteBarValueTest, ReturnsProperFloatWhenInitializedNegative)
{
    DiscreteBarValue discreteBarValue(-3.5f);
    EXPECT_EQ(static_cast<float>(discreteBarValue), 0.f);
}

TEST(DiscreteBarValueTest, ContainsHalfShouldReturnTrueOnHalfValue)
{
    DiscreteBarValue discreteBarValue(10.5f);
    DiscreteBarValue discreteBarValue2(0.5f);

    EXPECT_TRUE(discreteBarValue.containsHalf());
    EXPECT_TRUE(discreteBarValue2.containsHalf());
}

TEST(DiscreteBarValueTest, ContainsHalfShouldReturnFalseOnFull)
{
    DiscreteBarValue discreteBarValue(10.f);
    DiscreteBarValue discreteBarValue2(0.f);

    EXPECT_FALSE(discreteBarValue.containsHalf());
    EXPECT_FALSE(discreteBarValue2.containsHalf());
}

TEST(DiscreteBarValueTest, AtLeastHalfShouldReturnFalseBelowHalf)
{
    DiscreteBarValue discreteBarValue(0.f);
    DiscreteBarValue discreteBarValue2(0.22f);

    EXPECT_FALSE(discreteBarValue.atLeastHalf());
    EXPECT_FALSE(discreteBarValue2.atLeastHalf());
}

TEST(DiscreteBarValueTest, AtLeastHalfShouldReturnTrueAboveHalf)
{
    DiscreteBarValue discreteBarValue(0.5f);
    DiscreteBarValue discreteBarValue2(1.0f);
    DiscreteBarValue discreteBarValue3(10.5f);
    DiscreteBarValue discreteBarValue4(11.f);

    EXPECT_TRUE(discreteBarValue.atLeastHalf());
    EXPECT_TRUE(discreteBarValue2.atLeastHalf());
    EXPECT_TRUE(discreteBarValue3.atLeastHalf());
    EXPECT_TRUE(discreteBarValue4.atLeastHalf());
}

TEST(DiscreteBarValueTest, TwoTheSameDiscreteBarValuesShouldBeEqual)
{
    DiscreteBarValue discreteBarValue(10.5f);
    DiscreteBarValue discreteBarValue2(10.5f);
    EXPECT_EQ(discreteBarValue, discreteBarValue2);
    discreteBarValue = 10.f;
    discreteBarValue2 = 10.f;
    EXPECT_EQ(discreteBarValue, discreteBarValue2);
    discreteBarValue = 0.f;
    discreteBarValue2 = 0.f;
    EXPECT_EQ(discreteBarValue, discreteBarValue2);
    discreteBarValue = 0.5f;
    discreteBarValue2 = 0.5f;
    EXPECT_EQ(discreteBarValue, discreteBarValue2);
}

TEST(DiscreteBarValueTest, AssignOperatorShouldWorkCorrectly)
{
    DiscreteBarValue discreteBarValue(10.5f);
    EXPECT_EQ(static_cast<float>(discreteBarValue), 10.5f);
    discreteBarValue = 10.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 10.f);
    discreteBarValue = 0.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 0.f);
    discreteBarValue = 0.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 0.5f);
}

TEST(DiscreteBarValueTest, AdditionOperatorShouldWorkCorrectly)
{
    DiscreteBarValue discreteBarValue(0.0f);
    discreteBarValue += 0.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 0.5f);
    discreteBarValue += 2.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 2.5f);
    discreteBarValue += 9.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 12.f);
}

TEST(DiscreteBarValueTest, AdditionOperatorShouldNotAllowForNegativeValues)
{
    DiscreteBarValue discreteBarValue(5.5f);
    discreteBarValue += -0.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 5.5f);
    discreteBarValue += -2.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 5.5f);
    discreteBarValue += -5.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 5.5f);
}

TEST(DiscreteBarValueTest, SubtractionOperatorShouldWorkCorrectly)
{
    DiscreteBarValue discreteBarValue(12.0f);
    discreteBarValue -= 0.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 11.5f);
    discreteBarValue -= 2.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 9.5f);
    discreteBarValue -= 9.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 0.f);
    discreteBarValue -= 3.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 0.f);
}

TEST(DiscreteBarValueTest, SubtractionOperatorShouldNotAllowForNegativeValues)
{
    DiscreteBarValue discreteBarValue(12.0f);
    discreteBarValue -= -0.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 12.0f);
    discreteBarValue -= -2.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 12.0f);
    discreteBarValue -= -9.5f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 12.0f);
    discreteBarValue -= -3.f;
    EXPECT_EQ(static_cast<float>(discreteBarValue), 12.0f);
}


}// namespace