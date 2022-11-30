#pragma once

#include "Resources/Resources.h"
#include "States/State.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

class StateStack;

/**
 * @brief The state in which the player dies and sees the screen of death
 */
class DeathState : public State
{
public:
    DeathState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources);

    /**
     * \brief Draws only this state to the passed target
     * \param target where it should be drawn to
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    /**
     * \brief Updates the state logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    bool fixedUpdate(const float& deltaTime) override;

    /**
     * \brief Updates the game logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    bool update(const float& deltaTime) override;

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    bool handleEvent(const sf::Event& event) override;

private:
    /**
     * @brief Configures the death text that displays on the screen when a player dies.
     * @param window The window into which the text is drawn
     * @param fontManager Font manager from which fonts are read
     */
    void setupDeathText(const sf::RenderWindow& window, const FontManager& fontManager);

    /**
     * @brief Configures the background of death, for example, red colored background game.
     * @param window The window to which the death background is drawn.
     */
    void setupDeathScreenBackground(const sf::RenderWindow& window);


    sf::RenderWindow& mGameWindow;
    GameResources& mGameResources;
    sf::RectangleShape mDeathScreenBackground;
    sf::Text mDeathText;
};