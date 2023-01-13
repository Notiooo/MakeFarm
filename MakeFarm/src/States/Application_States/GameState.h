#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics/Shader.hpp>

#include "Player/Player.h"
#include "Renderer3D/Renderer3D.h"
#include "Resources/Resources.h"
#include "Resources/TexturePack.h"
#include "States/State.h"
#include "Utils/Serializer.h"
#include "Utils/Settings.h"
#include "World/Block/HighlightedBlock.h"
#include "World/Chunks/Chunk.h"
#include "World/Chunks/ChunkManager.h"

class StateStack;

/**
 * \brief The game state in which the game world is created,
 * all objects are placed and the processes inside the game world are controlled.
 */
class GameState : public State
{
public:
    GameState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources,
              GameSession& gameSession);

    ~GameState();

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
     * \brief Updates the status/logic of the ImGui Debug Menu
     */
    void updateDebugMenu();

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
     * @brief It checks whether the player is still alive and switches to the death screen if player
     * is dead.
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void checkIfPlayerIsDead(const float& deltaTime);

    /**
     * @brief Path to the file where the game data status is saved
     * @return Character string representing the game data path to the file
     */
    std::string gameDataSaveFilePath() const;

    /**
     * @brief Saves the state of the game data to a file
     */
    void saveGameDataToFile();

    /**
     * @brief Reads the saved state of the gamestate from the file
     * @warning Nothing happens when the save file is not there
     */
    void loadSavedGameData();

    /**
     * @brief Generates chunks under grac and waits for them to be generated (blocks the thread).
     */
    void waitForAndGenerateChunksBelowPlayer();

private:
    /** Rendering */
    sf::RenderWindow& mGameWindow;//!< Window to which this status is displayed
    sf::Shader m3DWorldRendererShader;
    Renderer3D mGameRenderer;

    /** Settings */
    GameResources& mGameResources;
    Settings mGameSettings;
    Serializer mSerializer;
    int mWorldSeed;
    const std::string& mSavedWorldName;

    /** Utils */
    ChunkManager mChunkManager;
    Player mPlayer;
    bool isDebugMenuActivated = false;
};


#endif