#pragma once

#include "Resources/Resources.h"
#include "States/State.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <TGUI/Backends/SFML/GuiSFML.hpp>

namespace tgui
{
class Button;
}// namespace tgui

class StateStack;

/**
 * @brief The state in which the player pauses the game
 */
class PauseState : public State
{
public:
    PauseState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources);

    static const inline auto BUTTON_SIZE = tgui::Layout2d(250.f, 40.f);
    static constexpr auto GAP_BETWEEN_BUTTONS = 20.f;

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
     * @brief Configures the pause text that displays on the screen when a player pauses.
     * @param window The window into which the text is drawn
     * @param fontManager Font manager from which fonts are read
     */
    void setupPauseText(const sf::RenderWindow& window, const FontManager& fontManager);

    /**
     * @brief Configures the background of pause, for example, dark colored background.
     * @param window The window to which the pause background is drawn.
     */
    void setupPauseScreenBackground(const sf::RenderWindow& window);

    /**
     * @brief Creates save and quit the game button
     * @param buttonPosition The position on which the button should be placed
     * @return Returns the button that saves and quit the game
     */
    std::shared_ptr<tgui::Button> createSaveAndQuitButton(const tgui::Layout2d& buttonPosition);

    /**
     * @brief Creates save and go back to menu button
     * @param buttonPosition The position on which the button should be placed
     * @return Returns the button that saves and go back to the main menu
     */
    std::shared_ptr<tgui::Button> createSaveAndGoMenuButton(const tgui::Layout2d& buttonPosition);

private:
    sf::RenderWindow& mGameWindow;
    GameResources& mGameResources;
    tgui::GuiSFML mGui;
    sf::RectangleShape mPauseScreenBackground;
    sf::Text mPauseText;
};