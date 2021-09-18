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
		lockedMouse = true;
		window.setMouseCursorVisible(false);
		centerMouse(window);
	}
}

void Mouse::unlockMouse(sf::Window& window)
{
	if (isMouseLocked())
	{
		window.setMouseCursorVisible(true);
		lockedMouse = false;
	}
}

bool Mouse::isMouseLocked()
{
	return lockedMouse;
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
		mouseOffset = sf::Mouse::getPosition(window) - windowCenter;
		centerMouse(window);
	}
}

void Mouse::handleFirstPersonBehaviour(const sf::Event& event, sf::RenderWindow& gameWindow)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
			Mouse::unlockMouse(gameWindow);
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.key.code == sf::Mouse::Left)
		{
			#ifdef _DEBUG
			if (!ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsAnyItemActive())
			{
				Mouse::lockMouseAtCenter(gameWindow);
			}
			#endif
		}
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		Mouse::lockMouseAtCenter(gameWindow);
	}
}
