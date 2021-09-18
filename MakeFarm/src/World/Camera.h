#pragma once
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <SFML/Graphics/RenderStates.hpp>


class Camera
{
public:
    Camera(const sf::RenderTarget& target, sf::Shader& shader);
    void fixedUpdate(const float& deltaTime);
    void update();
    void updateViewProjection();
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    void handleEvent(const sf::Event& event);
    glm::mat4 getView();
    glm::mat4 getProjection();

private:
    const sf::RenderTarget& renderTarget;
    sf::Shader& shader;

    glm::vec3 cameraPosition = glm::vec3(0, 0, 0);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 direction;

    float yaw = -90.f;
    float pitch = 0.f;

    float cameraSpeed = 5.f;
    float cameraSensitivity = 4.f;
    float fovCamera = 90.0f;

    glm::mat4 viewMatrix;
    mutable glm::mat4 projectionMatrix;
};
