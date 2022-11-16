#include "WireframeBlock.h"
#include "pch.h"

WireframeBlock::WireframeBlock(const sf::Vector3f& min, const sf::Vector3f& max)
    : min(min)
    , max(max)
{
}
WireframeBlock::WireframeBlock(const glm::vec3& min, const glm::vec3& max)
    : min({min.x, min.y, min.z})
    , max({max.x, max.y, max.z})
{
}
