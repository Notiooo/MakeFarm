#include "pch.h"
#include "Mouse.h"

#include <SFML/Window/Window.hpp>

void Mouse::centerMouse(const sf::Window& window)
{
	sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
}

void Mouse::lockMouseAtCenter(sf::Window& window)
{
	if (!isMouseLocked())
	{
		lockMouse = true;
		window.setMouseCursorVisible(false);
		centerMouse(window);
	}
}

void Mouse::unlockMouse(sf::Window& window)
{
	if (isMouseLocked())
	{
		window.setMouseCursorVisible(true);
		lockMouse = false;
	}
}

bool Mouse::isMouseLocked()
{
	return lockMouse;
}

sf::Vector2i Mouse::getMouseOffset()
{
	return mouseOffset;
}

void Mouse::update(const float& deltaTime, const sf::Window& window)
{
	if (isMouseLocked())
	{
		const auto windowCenter = sf::Vector2i(window.getSize().x / 2.f, window.getSize().y / 2.f);
		mouseOffset = windowCenter - sf::Mouse::getPosition(window);
		centerMouse(window);
	}
}
