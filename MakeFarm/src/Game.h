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
	 * \brief Updates game logic
	 * \param deltaTime the time that has passed since the game was last updated.
	 *
	 * Updates the game logic by passing the time that has elapsed
	 * since the previous fixedUpdate. This allows objects to move independently
	 * of the speed at which subsequent iterations of the program are executed.
	 * (distance = speed * time)
	 */
	void fixedUpdate(sf::Time deltaTime);

    /**
     * Updates the logic of the game which is not dependent on time and is performed every frame
     */
	void update();

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

	static const sf::Time TIME_PER_FRAME; //!< The time it takes for one game frame to be generated.

	static const int SCREEN_WIDTH; //!< Default game window width
	static const int SCREEN_HEIGHT; //!< Default game window height

	std::unique_ptr<sf::RenderWindow> mGameWindow; //!< The window to which the game image should be drawn.

	bool isGameRunning = true;

	/**
	 * \brief An object that holds loaded fonts that can be used inside the game.
	 *
	 * This saves the memory used so as not to load the same font multiple times in multiple places.
	 */
	FontManager mFonts;

	
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
