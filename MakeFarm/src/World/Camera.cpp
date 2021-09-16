#include "pch.h"
#include "Camera.h"

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/imgui.h>
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
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);

    auto targetSize = target.getSize();
    proj = glm::perspective(glm::radians(0.0f), static_cast<float>(targetSize.x / targetSize.y), 1.f, 100.f);
}

void Camera::update()
{
    ImGui::Begin("Camera");

    ImGui::SliderFloat3("Translation", &cameraPos.x, 0.0f, 960.0f);

    ImGui::End();
}

void Camera::updateViewProjection()
{
	sf::Shader::bind(&shader);
	glm::mat4 vp = getProjection() * getView();
	shader.setUniform("u_MVP", sf::Glsl::Mat4(&vp[0][0]));
	sf::Shader::bind(nullptr);
}

void Camera::fixedUpdate(const float& deltaTime)
{
    updateViewProjection();
	
    // Moving a camera
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        cameraPos += cameraSpeed * cameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        cameraPos -= cameraSpeed * cameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed * deltaTime;
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
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);

    float width = renderTarget.getSize().x;
    float height = renderTarget.getSize().y;
    proj = glm::perspective(glm::radians(fovCamera), width / height, 0.1f, 10000.f);
}

void Camera::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //float width = target.getSize().x, height = target.getSize().y;
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
    return view;
}

glm::mat4 Camera::getProjection()
{
    return proj;
}

