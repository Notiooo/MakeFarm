#pragma once
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "Block/Block.h"


/**
 * An in-game camera through which the player views gameplay.
 */
class Camera
{
public:
    Camera(const sf::RenderTarget& target, sf::Shader& shader);

    /**
     * \brief Updates the logic of this state
     * \param deltaTime the time that has passed since the game was last updated
     */
    void fixedUpdate(const float& deltaTime);

    /**
     * Updates the logic of the camera which is not dependent on time and is performed every frame
     */
    void update();

    /**
     * Updates ViewProjection inside the game shader to properly display the world
     */
    void updateViewProjection();

    /**
     * \brief Draws the camera to the passed target
     * \param target where it should be drawn to
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

    /**
     * \brief It takes input (event) from the user and interprets it.
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * Returns the view matrix.
     * @return View matrix.
     */
    glm::mat4 view();

    /**
     * Returns projection matrix.
     * @return Projection matrix.
     */
    glm::mat4 projection();

    /**
     * Returns the position at which the tip of the ray is in front of the camera
     * @tparam T Type in which coordinates are returned
     * @param raySize Length of ray.
     * @return Position at which the tip of the ray is in front of the camera
     */
    template <typename T>
    sf::Vector3<T> positionAtTheEndOfRayInfrontOfCamera(const float& raySize) const;

    /**
     * Returns the current position of the camera.
     * @return Current position of the camera.
     */
    glm::vec3 cameraPosition() const;

    /**
     * The maximum ray length to which blocks in front of the camera should be checked.
     */
    static constexpr float MAX_RAY_SIZE = 10 * Block::BLOCK_SIZE;

private:
    const sf::RenderTarget& mRenderTarget;
    sf::Shader& mShader;

    glm::vec3 mCameraPosition = glm::vec3(0, 0, 0);
    glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 mDirection;

    float mYaw = -90.f;
    float mPitch = 0.f;

    float mCameraSpeed = 5.f;
    float mCameraSensitivity = 4.f;
    float mFovCamera = 90.0f;

    glm::mat4 mViewMatrix;
    mutable glm::mat4 mProjectionMatrix;
};

template <typename T>
sf::Vector3<T> Camera::positionAtTheEndOfRayInfrontOfCamera(const float& raySize) const
{
    auto result = mCameraPosition + (mCameraFront * raySize);
    return sf::Vector3<T>(result.x, result.y, result.z);
}
