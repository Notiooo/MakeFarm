#include "CoordinatesAroundOriginGetter.h"
#include "Utils/Direction.h"
#include "pch.h"

CoordinatesAroundOriginGetter::CoordinatesAroundOriginGetter(sf::Vector3i origin)
    : mCurrentChunkPosition(origin)
{
}

void CoordinatesAroundOriginGetter::resetToStart()
{
    mChangedPosition = {0, 0, 0};
    mCurrentLineLength = 0;
    mCurrentDirection = Direction::ToTheRight;
}

sf::Vector3i CoordinatesAroundOriginGetter::nextValue()
{
    if (mCurrentLineLength == 0)
    {
        mCurrentLineLength = 1;
        return mCurrentChunkPosition;
    }

    switch (mCurrentDirection)
    {
        case Direction::ToTheRight:
            mChangedPosition.x += 1;
            if (mChangedPosition.x == mCurrentLineLength)
            {
                mCurrentDirection = Direction::InFront;
            }
            break;

        case Direction::InFront:
            mChangedPosition.z += 1;
            if (mChangedPosition.z == mCurrentLineLength)
            {
                mCurrentDirection = Direction::ToTheLeft;
            }
            break;

        case Direction::ToTheLeft:
            mChangedPosition.x -= 1;
            if (mChangedPosition.x == -mCurrentLineLength)
            {
                mCurrentDirection = Direction::Behind;
            }
            break;

        case Direction::Behind:
            mChangedPosition.z -= 1;
            if (mChangedPosition.z == -mCurrentLineLength)
            {
                mCurrentDirection = Direction::ToTheRight;
                ++mCurrentLineLength;
            }
            break;
    }
    return mCurrentChunkPosition + mChangedPosition;
}
