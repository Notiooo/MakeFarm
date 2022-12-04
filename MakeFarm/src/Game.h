#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics/RenderWindow.hpp>

#include "Resources/Resources.h"
#include "States/StateStack.h"

/**
 * \brief The main game class that controls the entire flow of the application.
 *
 * The whole task of this class is the run() function,
 * which ensures that the game runs. It runs the processes
 * of displaying the game (image), capturing player input
 * and updating the game logic.
 */
class Game
{
public:
    Game();
    /**
     * \brief The main loop that controls the operation of the game in the loop.
     *
     * Updates the game logic, displays the game image and captures the
     * player inputs to the various parts of the program.
     */
    void run();

private:
    /**
     * \brief Intercepts player inputs and passes them to processes inside the game.
     */
    void processEvents();

    /**
     * \brief Updates the game logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     *
     * This function is very aggravating and highly inefficient. In a certain amount of time it
     * executes an equal number of times by which it is ideal, for example, for collision
     * recalculation. Thus, in case of lag, there are no situations where the player can run through
     * a wall. Intermediate intervals regardless of the frame rate are always recalculated!
     *
     * Updates the game logic by passing the time that has elapsed
     * since the previous interval. This allows objects to move independently
     * of the speed at which subsequent iterations of the program are executed.
     * (distance = speed * time)
     */
    void fixedUpdate(const sf::Time& deltaTime);

    /**
     * Performs fixedUpdate at least a minimum number of times to avoid behavior where, due to high
     * lag, a character is moved off the wall avoiding collision checking.
     *
     * @param frameTimeElapsed the time that has passed since the game was last updated.
     */
    void performFixedUpdateAtLeastMinimalNumberOfTimes(sf::Time& frameTimeElapsed);

    /**
     * \brief Updates the game logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     *
     * It is not recommended to use this feature for physics, or general movement.
     * Updates the game logic by passing the time that has elapsed since the previous
     * update call. This allows objects to move independently of the speed at which
     * subsequent frames of the program are executed.
     * (distance = speed * time)
     */
    void update(const sf::Time& deltaTime);

    /**
     * \brief Displays the game on the image of the game window
     *
     * It collects all the game elements that you want to display in the current
     * iteration and displays them on the screen in the order they appear in the code.
     * It clears the screen with a black image before displaying a new frame.
     */
    void render();


    /**
     * \brief Loads the resources needed by the processes inside the game.
     *
     * Not all available resources are listed here, as some are loaded on the fly
     * inside the game. This is because for example there is no need to keep the whole
     * game in memory when the player is really only inside the game's main menu.
     */
    void loadResources();

    /**
     * @brief Loads the textures needed for the inventory displaying.
     * @param guiTexturesFolder Folder that stores textures for the GUI
     */
    void loadInventoryTextures(const std::string& guiTexturesFolder);

    /**
     * @brief Loads the textures needed for the healthbar displaying.
     * @param guiTexturesFolder Folder that stores textures for the GUI
     */
    void loadHealthbarTextures(const std::string& guiTexturesFolder);

    /**
     * @brief The time it takes for one game frame to be generated.
     */
    static const sf::Time TIME_PER_FRAME;

    /**
     * @brief Minimum time between one fixed update and another
     */
    static const sf::Time MINIMAL_TIME_PER_FIXED_UPDATE;

    /**
     * @brief Default game window width
     */
    static const int SCREEN_WIDTH;

    /**
     * @brief Default game window height
     */
    static const int SCREEN_HEIGHT;

    /**
     * @brief The window to which the game image should be drawn.
     */
    std::unique_ptr<sf::RenderWindow> mGameWindow;

    bool isGameRunning = true;

    /**
     * \brief An object that holds loaded fonts that can be used inside the game.
     *
     * This saves the memory used so as not to load the same font multiple times in multiple places.
     */
    FontManager mFonts;

    /**
     * @brief Any game assets from textures or fonts
     */
    GameResources mGameResources;

    /**
     * @brief Structure hold information about the current game session
     */
    GameSession mGameSession;

    /**
     * \brief Stores and manages in-game states.
     *
     * This allows you to create some application flow between application states.
     * Among other things, it allows to go from the main menu of the game to the game
     * itself, as well as to pause the game.
     */
    StateStack mAppStack;
};


#endif
