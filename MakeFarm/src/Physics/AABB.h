#pragma once
#include "World/Block/WireframeBlock.h"
#include <SFML/System/Vector3.hpp>

/**
 * @brief Axis-aligned bounding boxes allows to determine whether two classes of type AABB
 * collide with each other. They represent cubes in 3D space.
 */
class AABB
{
public:
    /**
     * @brief Determines how the cube is relative to the given position.
     */
    enum class RelativeTo
    {
        LeftBottomBack,
        Center,
        UpperCenter,
        BottomCenter
    };

    /**
     * @brief Creates an AABB with the given dimensions. The object is
     * initially located at position {0,0,0}.
     * @param boxSize The dimensions of the cube.
     */
    AABB(sf::Vector3f boxSize);

    /**
     * @brief Creates a cube with empty values not yet filled in.
     */
    AABB(const AABB&);

    /**
     * @brief It compares whether the two AABBs are the same, that is, whether the sizes of the
     * cubes are the same and whether their position is the same.
     * @param rhs The second AABB with which we are comparing this object.
     * @return
     */
    bool operator==(const AABB& rhs) const;

    /**
     * @brief Updates the position of the cube
     * @tparam Vector3 A vector of three dimensions "x, y, z"
     * @param position New position of the cube in space
     * @param relativeTo Relative to what to position the cube in space.
     */
    template<typename Vector3>
    void updatePosition(const Vector3& position, const RelativeTo& relativeTo);

    /**
     * @brief Checks if there is a collision between this cube and the other given cube.
     * @param rhs The cube with which the collision is checked
     * @return True if the collision occurred, false otherwise
     */
    [[nodiscard]] bool intersect(const AABB& rhs) const;

    /**
     * @brief Collision box defining two vertices of a cube arranged in space.
     * @return Two vertices of the cube (extreme minimum and extreme maximum) forming a cube in
     * space. The coordinates of the vertices are given on a non-metric scale.
     */
    [[nodiscard]] WireframeBlock collisionBox() const;

    /**
     * @brief Returns the size of the cube (in x, y, z).
     * @return Size of the cube (in x, y, z).
     */
    [[nodiscard]] sf::Vector3f collisionBoxSize() const;

private:
    sf::Vector3f mBoxSize;
    WireframeBlock mWireframe;

    sf::Vector3f& mMin;
    sf::Vector3f& mMax;
};

template<typename Vector3>
void AABB::updatePosition(const Vector3& position, const AABB::RelativeTo& relativeTo)
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