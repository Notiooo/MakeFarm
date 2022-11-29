#include "Hearts.h"
#include "pch.h"

Hearts::Hearts(const Hearts::HeartsNumber& heartsNumber)
    : mHearts(toHearts(heartsNumber))
{
}

Hearts::operator float() const
{
    return mHearts;
}

Hearts::HeartsNumber Hearts::toHearts(float heartsNumber)
{
    if (heartsNumber < 0)
    {
        return 0;
    }

    heartsNumber *= 2;
    heartsNumber = std::round(heartsNumber);
    heartsNumber /= 2;
    return heartsNumber;
}

Hearts& Hearts::operator+=(const Hearts::HeartsNumber& heartsNumber)
{
    mHearts += toHearts(heartsNumber);
    return *this;
}

Hearts& Hearts::operator-=(const Hearts::HeartsNumber& heartsNumber)
{
    mHearts -= toHearts(heartsNumber);

    if (mHearts < 0)
    {
        mHearts = 0;
    }
    return *this;
}

Hearts& Hearts::operator=(const Hearts::HeartsNumber& heartsNumber)
{
    mHearts = toHearts(heartsNumber);
    return *this;
}

bool Hearts::containsHalfOfHeart() const
{
    return (static_cast<int>(mHearts) != mHearts);
}

bool Hearts::atLeastHalfHeart() const
{
    return mHearts;
}
