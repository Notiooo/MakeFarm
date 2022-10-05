#include "pch.h"
#include "Camera.h"

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Utils/Mouse.h"

Camera::Camera(const sf::RenderTarget& target, sf::Shader& shader)
	: mRenderTarget(target)
    , mShader(shader)
{

    mDirection.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mDirection.y = sin(glm::radians(mPitch));
    mDirection.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mCameraUp);

    auto targetSize = target.getSize();
    mProjectionMatrix = glm::perspective(glm::radians(0.0f), static_cast<float>(targetSize.x / targetSize.y), 1.f, 100.f);
}

void Camera::update(const float& deltaTime)
{
    updateDebugMenu();
    handleMouseInputs(deltaTime);

    auto width = static_cast<float>(mRenderTarget.getSize().x);
    auto height = static_cast<float>(mRenderTarget.getSize().y);
    mProjectionMatrix = glm::perspective(glm::radians(mFovCamera), width / height, 0.1f, 10000.f);
    mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mCameraUp);
    updateViewProjection();
}

void Camera::updateViewProjection()
{
	sf::Shader::bind(&mShader);
	glm::mat4 vp = projection() * view();
	mShader.setUniform("u_ViewProjection", sf::Glsl::Mat4(&vp[0][0]));
	sf::Shader::bind(nullptr);
}

void Camera::fixedUpdate(const float& deltaTime)
{
    handleKeyboardInputs(deltaTime);
}

void Camera::handleMouseInputs(const float& deltaTime)
{
    if (Mouse::isMouseLocked())
    {
        calculateCameraAngles(deltaTime);
        keepNaturalPitchRanges();
        calculateCameraDirectionVector();
    }
}

void Camera::calculateCameraDirectionVector()
{
    mDirection.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mDirection.y = sin(glm::radians(mPitch));
    mDirection.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mCameraFront = glm::normalize(mDirection);
}

void Camera::calculateCameraAngles(const float& deltaTime)
{
    auto mouseOffset = static_cast<sf::Vector2f>(Mouse::mouseOffset());

    mouseOffset.x *= mCameraSensitivity * deltaTime;
    mouseOffset.y *= mCameraSensitivity * deltaTime * -1;

    mYaw += mouseOffset.x;
    mPitch += mouseOffset.y;
}

void Camera::keepNaturalPitchRanges()
{
    if (mPitch > 89.0f)
    {
        mPitch = 89.0f;
    }
    if (mPitch < -89.0f)
    {
        mPitch = -89.0f;
    }
}

void Camera::handleKeyboardInputs(const float& deltaTime)
{
    auto cameraSpeed = this->mCameraSpeed;
    cameraSpeed = applyAdditionalCameraAcceleration(cameraSpeed);
    handleCameraMovement(deltaTime, cameraSpeed);
}

void Camera::handleCameraMovement(const float &deltaTime, float cameraSpeed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        mCameraPosition += cameraSpeed * mCameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        mCameraPosition -= cameraSpeed * mCameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        mCameraPosition += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        mCameraPosition -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        mCameraPosition -= cameraSpeed * mCameraUp * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        mCameraPosition += cameraSpeed * mCameraUp * deltaTime;
    }
}

float Camera::applyAdditionalCameraAcceleration(float cameraSpeed) const
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        cameraSpeed *= 2;
    }
    return cameraSpeed;
}

void Camera::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Nothing to draw yet
}

void Camera::handleEvent(const sf::Event& event)
{
    switch(event.type)
    {
	    case sf::Event::MouseWheelMoved:
            mFovCamera -= event.mouseWheel.delta;
	        break;
    }
}

glm::mat4 Camera::view()
{
    return mViewMatrix;
}

glm::mat4 Camera::projection()
{
    return mProjectionMatrix;
}

glm::vec3 Camera::cameraPosition() const
{
    return mCameraPosition;
}

void Camera::updateDebugMenu()
{
    ImGui::Begin("Camera");
    ImGui::SliderFloat3("Translation", &mCameraPosition.x, 0.0f, 960.0f);
    ImGui::End();
}

