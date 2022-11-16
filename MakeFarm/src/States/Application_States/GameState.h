#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics/Shader.hpp>

#include "Player/Player.h"
#include "Renderer3D/Renderer3D.h"
#include "Resources/Resources.h"
#include "Resources/TexturePack.h"
#include "States/State.h"
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
    GameState(StateStack& stack, sf::RenderWindow& window);

    /**
     * \brief Draws only this state to the passed target
     * \param target where it should be drawn to
     * \param states provides information about rendering process (coordinateInGivenDirection,
     * shader, blend mode)
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
    /** Rendering */
    sf::RenderWindow& mGameWindow;//!< Window to which this status is displayed
    sf::Shader m3DWorldRendererShader;
    Renderer3D mGameRenderer;

    /** Settings */
    TexturePack mTexturePack;
    Settings mGameSettings;

    /** Utils */
    ChunkManager mChunkManager;
    Player mPlayer;
};


#endif