#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

class RenderTargetStub : public sf::RenderTarget
{
public:
    sf::Vector2u getSize() const override;
};