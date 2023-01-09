#include "AABB.h"
#include "pch.h"

AABB::AABB(sf::Vector3f boxSize)
    : mBoxSize(boxSize)
    , mWireframe(sf::Vector3f{0, 0, 0}, boxSize)
    , mMin(mWireframe.min)
    , mMax(mWireframe.max)
{
}

AABB::AABB(const AABB& rhs)
    : mBoxSize(rhs.mBoxSize)
    , mWireframe(rhs.mWireframe)
    , mMin(mWireframe.min)
    , mMax(mWireframe.max)
{
}

bool AABB::intersect(const AABB& rhs) const
{
    return (mMin.x <= rhs.mMax.x && mMax.x >= rhs.mMin.x && mMin.y <= rhs.mMax.y &&
            mMax.y >= rhs.mMin.y && mMin.z <= rhs.mMax.z && mMax.z >= rhs.mMin.z);
}

WireframeBlock AABB::collisionBox() const
{
    return mWireframe;
}

sf::Vector3f AABB::collisionBoxSize() const
{
    return mBoxSize;
}

bool AABB::operator==(const AABB& rhs) const
{
    return mMin == rhs.mMin && mMax == rhs.mMax;
}
