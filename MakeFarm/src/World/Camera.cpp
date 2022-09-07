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
	: renderTarget(target)
    , shader(shader)
{

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    auto targetSize = target.getSize();
    projectionMatrix = glm::perspective(glm::radians(0.0f), static_cast<float>(targetSize.x / targetSize.y), 1.f, 100.f);
}

void Camera::update()
{
    ImGui::Begin("Camera");

    ImGui::SliderFloat3("Translation", &cameraPosition.x, 0.0f, 960.0f);

    ImGui::End();
}

void Camera::updateViewProjection()
{
	sf::Shader::bind(&shader);
	glm::mat4 vp = getProjection() * getView();
	shader.setUniform("u_ViewProjection", sf::Glsl::Mat4(&vp[0][0]));
	sf::Shader::bind(nullptr);
}

void Camera::fixedUpdate(const float& deltaTime)
{
    updateViewProjection();

    float cameraSpeed = this->cameraSpeed;

    // Allow player to move faster when press lshift
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        cameraSpeed *= 2;
    }
	
    // Moving a camera
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        cameraPosition += cameraSpeed * cameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        cameraPosition -= cameraSpeed * cameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        cameraPosition -= cameraSpeed * cameraUp * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        cameraPosition += cameraSpeed * cameraUp * deltaTime;
    }

    // Looking with camera
    if (Mouse::isMouseLocked())
    {
        auto mouseOffset = static_cast<sf::Vector2f>(Mouse::getMouseOffset());

        mouseOffset.x *= cameraSensitivity * deltaTime;
        mouseOffset.y *= cameraSensitivity * deltaTime * -1;

        yaw += mouseOffset.x;
        pitch += mouseOffset.y;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }
    viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    float width = renderTarget.getSize().x;
    float height = renderTarget.getSize().y;
    projectionMatrix = glm::perspective(glm::radians(fovCamera), width / height, 0.1f, 10000.f);
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
	        fovCamera -= event.mouseWheel.delta;
	        break;
    }
}

glm::mat4 Camera::getView()
{
    return viewMatrix;
}

glm::mat4 Camera::getProjection()
{
    return projectionMatrix;
}

glm::vec3 Camera::getCameraPosition() const
{
    return cameraPosition;
}

