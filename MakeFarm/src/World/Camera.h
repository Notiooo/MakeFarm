#pragma once
#include <SFML/Graphics/RenderStates.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

#include "Block/Block.h"


/**
 * An in-game camera through which the player views gameplay.
 */
class Camera
{
public:
    Camera(const sf::RenderTarget& target, sf::Shader& shader);

    /**
     * \brief Updates the camera logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    void fixedUpdate(const float& deltaTime);

    /**
     * Updates the camera logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * Updates ViewProjection inside the game shader to properly display the world
     */
    void updateViewProjection();

    /**
     * Updates ViewProjection inside the game shader to properly display the world
     * @param shader Shader with which view projection should be calculated
     */
    void updateViewProjection(sf::Shader& shader) const;

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
    glm::mat4 view() const;

    /**
     * Returns projection matrix.
     * @return Projection matrix.
     */
    glm::mat4 projection() const;

    /**
     * Returns the position at which the tip of the ray is in front of the camera
     * @tparam T Type in which coordinates are returned
     * @param raySize Length of ray.
     * @return Position at which the tip of the ray is in front of the camera
     */
    template<typename T>
    sf::Vector3<T> positionAtTheEndOfRayInfrontOfCamera(const float& raySize) const;

    /**
     * Returns the current position of the camera.
     * @return Current position of the camera.
     */
    glm::vec3 cameraPosition() const;

    /**
     * @brief Sets the camera position
     * @param newPosition The new position on which the camera should be placed.
     */
    void cameraPosition(const glm::vec3& newPosition);

    /**
     * Camera viewing direction (front).
     * @return Returns front camera viewing direction.
     */
    glm::vec3 direction() const;

    /**
     * Returns the direction of the camera (front) without taking into account the pitch, i.e. the
     * up-down rotation of the camera.
     * @return Returns front camera viewing direction without pitch (no up-down rotation).
     */
    glm::vec3 directionWithoutPitch() const;

    /**
     * @brief Returns the right direction of the camera, which is the cross product of the front and
     * upward vectors.
     * @return Right direction of the camera.
     */
    glm::vec3 rightDirection() const;

    /**
     * @brief Returns the right direction of the camera without pitch, which is the cross product of
     * the front and upward vectors without respecting pitch, that is, the up-and-down rotation of
     * the camera.
     * @return Right direction of the camera without pitch (without up-and-down rotation of camera)
     */
    glm::vec3 rightDirectionWithoutPitch() const;

    /**
     * @brief Returns the upward direction of the camera
     * @return Upward direction of the camera.
     */
    glm::vec3 upwardDirection() const;

private:
    /**
     * \brief Updates the status/logic of the ImGui Debug Menu
     */
    void updateDebugMenu();

    /**
     * Handle keyboard behavior such as moving the camera inside the game
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void handleKeyboardInputs(const float& deltaTime);

    /**
     * It changes speed depending on whether the player has pressed
     * the button responsible for accelerating the camera.
     *
     * @param cameraSpeed Base camera speed
     * @return Accelerated or base camera speed
     */
    float applyAdditionalCameraAcceleration(float cameraSpeed) const;

    /**
     * Handles keyboard behavior for the camera, such as moving the camera.
     * @param deltaTime the time that has passed since the game was last updated.
     * @param cameraSpeed Base camera speed
     */
    void handleCameraMovement(const float& deltaTime, float cameraSpeed);

    /**
     * Handles mouse behavior for the camera, such as rotating the camera.
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void handleMouseInputs(const float& deltaTime);

    /**
     * It keeps an eye on the intervals for pitch, so that moving the camera is natural
     * and takes place within the intervals known by the players. For example, it is
     * impossible to perform a 360-degree backflip.
     */
    void keepNaturalPitchRanges();

    /**
     * Calculates yaw and pitch for the camera
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void calculateCameraAngles(const float& deltaTime);

    /**
     * Calculates the camera's directional vector, which is the final\
     * vector responsible for the camera's rotation.
     */
    void calculateCameraDirectionVector();

    const sf::RenderTarget& mRenderTarget;
    sf::Shader& mShader;

    glm::vec3 mCameraPosition = glm::vec3(0, 0, 0);
    glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 mCameraFrontWithoutPitch = glm::vec3(0.0f, 0.0f, -1.0f);

    float mYaw = -90.f;
    float mPitch = 0.f;

    float mCameraSpeed = 5.f;
    float mCameraSensitivity = 4.f;
    float mFovCamera = 90.0f;

    glm::mat4 mViewMatrix;
    mutable glm::mat4 mProjectionMatrix;
};

template<typename T>
sf::Vector3<T> Camera::positionAtTheEndOfRayInfrontOfCamera(const float& raySize) const
{
    auto result = mCameraPosition + (mCameraFront * raySize);
    return sf::Vector3<T>(result.x, result.y, result.z);
}
