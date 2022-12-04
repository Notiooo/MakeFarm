#pragma once

#include "Resources/Resources.h"
#include "States/State.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <TGUI/Backends/SFML/GuiSFML.hpp>

namespace tgui
{
class Button;
class EditBox;
class Picture;
class ScrollablePanel;
}// namespace tgui

class StateStack;

/**
 * @brief The state of the main menu in which the player can create a new world, or select an
 * existing one.
 */
class MainMenuState : public State
{
public:
    MainMenuState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources,
                  GameSession& gameSession);

    static const inline auto BUTTON_SIZE = tgui::Layout2d(250.f, 40.f);
    static constexpr auto LEVEL_SELECTION_PADDING = 100.f;
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
     * @brief Displays the main menu screen
     */
    void showMainMenu();

    /**
     * @brief Displays a screen for selecting, or creating, a new world
     */
    void showWorldSelection();

    /**
     * @brief Configures and sets the status background of the main menu
     * @param window The window to which the background is to be set and will be drawn
     */
    void setupBackground(const sf::RenderWindow& window);

    /**
     * @brief Creates single player mode launch button
     * @param buttonPosition The position on which the button should be placed
     * @return Returns the button that activates single player mode
     */
    std::shared_ptr<tgui::Button> createSinglePlayerButton(const tgui::Layout2d& buttonPosition);

    /**
     * @brief Creates button that quits the game when pressed.
     * @param buttonPosition The position on which the button should be placed
     * @return Returns the button that quits the game when pressed.
     */
    std::shared_ptr<tgui::Button> createQuitGameButton(const tgui::Layout2d& buttonPosition);

    /**
     * @brief Creates a scrollable panel that displays available world saves.
     * @return Scrollable panel that displays available world saves.
     */
    std::shared_ptr<tgui::ScrollablePanel> createWorldSelectionScrollablePanel();

    /**
     * @brief Creates a button to return to the main menu, which when clicked, results in the
     * display of the main menu
     * @param buttonPosition The position on which the button should be placed
     */
    void createGoBackToMainMenuButton(const tgui::Layout2d& buttonPosition);

    /**
     * @brief Creates a field into which the name of the game world the player wants to create can
     * be entered
     * @param boxPosition The position on which the box should be placed
     * @return Field into which the name of the game world the player wants to create can be entered
     */
    std::shared_ptr<tgui::EditBox> createEditWorldNameBox(const tgui::Layout2d& boxPosition);

    /**
     * @brief Creates a button that is responsible for creating a new world based on the world name
     * specified in editBox
     * @param worldNameBox Editbox from which the name of the new world read.
     * @param boxPosition The position on which the box should be placed
     * @return Button that is responsible for creating a new world.
     */
    std::shared_ptr<tgui::Button> createNewWorldButton(std::shared_ptr<tgui::EditBox> worldNameBox,
                                                       const tgui::Layout2d& boxPosition);

    /**
     * @brief It reads the saved worlds and puts them inside a scrollable panel.
     * @param panel Scrollable panel that displays available worlds
     */
    void loadSavesIntoWorldSelectionScrollablePanel(
        std::shared_ptr<tgui::ScrollablePanel> panel) const;

    /**
     * @brief Checks if the world with the given name exists
     * @param worldName Name the world to check
     * @return True if the world exists, false otherwise
     */
    bool doesGivenWorldExist(const std::string& worldName) const;

    /**
     * @brief It makes the name of the world certainly unique and there is no such in the folder
     * with worlds
     * @param worldName The name of the world, which should be transformed into a unique
     * @return A unique world name that does not yet exist.
     */
    std::string makeWorldNameUnique(const std::string& worldName) const;


    sf::RenderWindow& mGameWindow;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackground;
    tgui::GuiSFML mGui;
    GameSession& mGameSession;
};