#pragma once
#include <SFML/Window/Mouse.hpp>

/**
 * Enhanced mouse behavior allowing much more in the application.
 */
class Mouse : public sf::Mouse
{
public:
    /**
     * Centers the mouse relative to the window
     * @param window Window relative to which the mouse is centered
     */
    static void centerMouse(const sf::Window& window);

    /**
     * Locks the cursor in the center of the screen not allowing it to move.
     * @param window The window against which the cursor is locked.
     */
    static void lockMouseAtCenter(sf::Window& window);

    /**
     * Unlocks the mouse when it is locked in the middle of the screen.
     * @param window Window against which the mouse is locked
     */
    static void unlockMouse(sf::Window& window);

    /**
     * Returns information about whether the cursor is locked relative to window.
     * @return True if blocked, false otherwise
     */
    static bool isMouseLocked();

    /**
     * Offset relative to the center of the window.
     * @return
     */
    static sf::Vector2i mouseOffset();

    /**
     * Updates the game logic dependent, or independent of time, every rendered frame.
     * @param deltaTime the time that has passed since the game was last updated.
     * @param window The window relative to which the cursor is drawn
     */
    static void update(const float& deltaTime, const sf::Window& window);

    /**
     * It handles mouse behavior typical of first-person game controls.
     */
    static void handleFirstPersonBehaviour(const sf::Event& event, sf::RenderWindow& gameWindow);

private:
    static inline bool mLockedMouse = false;
    inline static sf::Vector2i mMouseOffset;
};
