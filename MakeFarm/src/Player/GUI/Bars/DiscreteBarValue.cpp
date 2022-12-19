#include "DiscreteBarValue.h"
#include "pch.h"

DiscreteBarValue::DiscreteBarValue(const DiscreteBarValue::DiscreteNumber& value)
    : mDiscreteNumber(toDiscrete(value))
{
}

DiscreteBarValue::operator float() const
{
    return mDiscreteNumber;
}

DiscreteBarValue::DiscreteNumber DiscreteBarValue::toDiscrete(float value)
{
    if (value < 0)
    {
        return 0;
    }

    value *= 2;
    value = std::round(value);
    value /= 2;
    return value;
}

DiscreteBarValue& DiscreteBarValue::operator+=(const DiscreteBarValue::DiscreteNumber& value)
{
    mDiscreteNumber += toDiscrete(value);
    return *this;
}

DiscreteBarValue& DiscreteBarValue::operator-=(const DiscreteBarValue::DiscreteNumber& value)
{
    mDiscreteNumber -= toDiscrete(value);

    if (mDiscreteNumber < 0)
    {
        mDiscreteNumber = 0;
    }
    return *this;
}

DiscreteBarValue& DiscreteBarValue::operator=(const DiscreteBarValue::DiscreteNumber& value)
{
    mDiscreteNumber = toDiscrete(value);
    return *this;
}

bool DiscreteBarValue::containsHalf() const
{
    return (static_cast<int>(mDiscreteNumber) != mDiscreteNumber);
}

bool DiscreteBarValue::atLeastHalf() const
{
    return mDiscreteNumber;
}
