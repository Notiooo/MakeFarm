#pragma once
#include <glm/vec3.hpp>

/**
 * @brief Determines the block of minimum and maximum coordination, which forms a cube in 3D space.
 */
struct WireframeBlock
{
    WireframeBlock() = default;
    WireframeBlock(const sf::Vector3f& min, const sf::Vector3f& max);
    WireframeBlock(const glm::vec3& min, const glm::vec3& max);

    sf::Vector3f min;
    sf::Vector3f max;
};
