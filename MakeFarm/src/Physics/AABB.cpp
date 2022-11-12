#include "AABB.h"
#include "pch.h"

AABB::AABB(sf::Vector3f boxSize)
    : mBoxSize(boxSize)
{
}

void AABB::updatePosition(const glm::vec3& position, const AABB::RelativeTo& relativeTo)
{
    switch (relativeTo)
    {
        case RelativeTo::LeftBottomBack:
            mMin.x = position.x;
            mMax.x = position.x + mBoxSize.x;
            mMin.y = position.y;
            mMax.y = position.y + mBoxSize.y;
            mMin.z = position.z;
            mMax.z = position.z + mBoxSize.z;
            break;
        case RelativeTo::Center:
            mMin.x = position.x - (mBoxSize.x / 2.f);
            mMax.x = position.x + (mBoxSize.x / 2.f);
            mMin.y = position.y - (mBoxSize.y / 2.f);
            mMax.y = position.y + (mBoxSize.y / 2.f);
            mMin.z = position.z - (mBoxSize.z / 2.f);
            mMax.z = position.z + (mBoxSize.z / 2.f);
            break;
        case RelativeTo::UpperCenter:
            mMin.x = position.x - (mBoxSize.x / 2.f);
            mMax.x = position.x + (mBoxSize.x / 2.f);
            mMin.y = position.y - (mBoxSize.y);
            mMax.y = position.y;
            mMin.z = position.z - (mBoxSize.z / 2.f);
            mMax.z = position.z + (mBoxSize.z / 2.f);
            break;
        case RelativeTo::BottomCenter:
            mMin.x = position.x - (mBoxSize.x / 2.f);
            mMax.x = position.x + (mBoxSize.x / 2.f);
            mMin.y = position.y;
            mMax.y = position.y + (mBoxSize.y);
            mMin.z = position.z - (mBoxSize.z / 2.f);
            mMax.z = position.z + (mBoxSize.z / 2.f);
            break;
    }
}

bool AABB::intersect(const AABB& rhs) const
{
    return (mMin.x <= rhs.mMax.x && mMax.x >= rhs.mMin.x && mMin.y <= rhs.mMax.y &&
            mMax.y >= rhs.mMin.y && mMin.z <= rhs.mMax.z && mMax.z >= rhs.mMin.z);
}

std::pair<sf::Vector3f, sf::Vector3f> AABB::collisionBox() const
{
    return std::make_pair(mMin, mMax);
}

sf::Vector3f AABB::collisionBoxSize() const
{
    return mBoxSize;
}

bool AABB::operator==(const AABB& rhs) const
{
    return mMin == rhs.mMin && mMax == rhs.mMax;
}
