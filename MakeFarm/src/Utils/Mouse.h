#pragma once
#include <SFML/Window/Mouse.hpp>

class Mouse : public sf::Mouse
{
public:
	static void centerMouse(const sf::Window& window);

	static void lockMouseAtCenter(sf::Window& window);
	static void unlockMouse(sf::Window& window);

	static bool isMouseLocked();
	static sf::Vector2i getMouseOffset();
	static void update(const float& deltaTime, const sf::Window& window);

private:
	static inline bool lockMouse = false;
	inline static sf::Vector2i mouseOffset;
};
