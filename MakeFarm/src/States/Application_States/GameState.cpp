#include "GameState.h"
#include "pch.h"


#include "World/Item/ItemMap.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include "States/StateStack.h"
#include "Utils/Mouse.h"
#include "Utils/Settings.h"
#include "World/Block/BlockMap.h"
#include "World/Chunks/TerrainGenerator.h"

GameState::GameState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources,
                     GameSession& gameSession)
    : State(stack)
    , mGameWindow(window)
    , mGameResources(gameResources)
    , mWorldSeed(TerrainGenerator::randomSeed())
    , mSavedWorldName(gameSession.currentlyPlayedWorld.value())
    , mChunkManager(mGameResources.texturePack, mSavedWorldName, mWorldSeed)
    , mPlayer(mChunkManager.calculateSpawnPoint(), mGameWindow, m3DWorldRendererShader,
              mChunkManager, mGameResources, mSavedWorldName)
    , mGameSettings("settings.cfg")
{
    Mouse::lockMouseAtCenter(mGameWindow);
    m3DWorldRendererShader.loadFromFile("resources/shaders/3DWorldRenderer/VertexShader.shader",
                                        "resources/shaders/3DWorldRenderer/FragmentShader.shader");

    // TODO: Probably it is not the best approach to pass variables between states
    gameSession.player = &mPlayer;

    // TODO: This stuff works just because seed is passed to ChunkManager by reference!
    // Maybe chunk manager costruction should be delayed using unique_ptr?
    loadSavedGameData();

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void GameState::loadSavedGameData()
{
    std::ifstream file(gameDataSaveFilePath(), std::ios::binary);
    if (file.is_open())
    {
        mSerializer.readSerialized(file, mWorldSeed);
    }
}

bool GameState::handleEvent(const sf::Event& event)
{
    mPlayer.handleEvent(event);

    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape: requestPush(State_ID::PauseState); break;
            case sf::Keyboard::F1: Mouse::unlockMouse(mGameWindow); break;
            case sf::Keyboard::F4: requestClear(); break;
        }
    }

    /*
     * Set this state to transparent -- in other words
     * allow States below in stack to be rendered.
     */
    return true;
}

bool GameState::fixedUpdate(const float& deltaTime)
{
    /*
     * Send deltaTime to the gameWorld
     * so it can update itself with proper time
     * Because some for example moving object
     * works according to
     * d = st (distance = speed * time)
     */

    mPlayer.fixedUpdate(deltaTime, mChunkManager.chunks());
    // mPlayer.updateCollision(mChunkManager.chunks());

    /*
     * Set this state to transparent -- in other words
     * allow States below in stack to be rendered.
     */
    return true;
}

void GameState::updateDebugMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("OpenGL"))
        {
            if (ImGui::MenuItem("Switch Wireframe (on/off)"))
            {
                std::unique_ptr<int[]> rastMode(new int[2]);
                GLCall(glGetIntegerv(GL_POLYGON_MODE, rastMode.get()));

                if (rastMode[1] == GL_FILL)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("MakeFarm"))
        {
            if (ImGui::BeginMenu("Texture Packs"))
            {
                for (auto const& texturePackDir:
                     std::filesystem::directory_iterator{"resources/textures/texturePacks"})
                {
                    if (texturePackDir.is_directory())
                    {
                        auto texturePackFolder = texturePackDir.path().filename().string();
                        if (ImGui::MenuItem(texturePackFolder.c_str()))
                        {
                            mGameResources.texturePack.loadTexturePack(texturePackFolder);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // FPS Counter
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << ImGui::GetIO().Framerate << " FPS ";
        auto windowWidth = ImGui::GetWindowSize().x;
        auto fpsString = ss.str();
        auto textWidth = ImGui::CalcTextSize(fpsString.c_str()).x;

        ImGui::SetCursorPosX(windowWidth - textWidth);
        ImGui::Text(fpsString.c_str());
        ImGui::EndMainMenuBar();
    }
}

bool GameState::update(const float& deltaTime)
{
    mPlayer.update(deltaTime);

    mChunkManager.update(deltaTime, mPlayer.camera());
    mChunkManager.generateChunksAround(mPlayer.position());
    mChunkManager.clearFarAwayChunks(mPlayer.position());
    updateDebugMenu();
    checkIfPlayerIsDead(deltaTime);

    return true;
}

void GameState::checkIfPlayerIsDead(const float& deltaTime)
{
    if (mPlayer.isDead())
    {
        mPlayer.camera().rotation(45);
        mPlayer.camera().update(deltaTime);
        requestPush(State_ID::DeathState);
    }
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mChunkManager.draw(mGameRenderer, m3DWorldRendererShader);
    mPlayer.draw(mGameRenderer, target, states);
}


std::string GameState::gameDataSaveFilePath() const
{
    return mSavedWorldName + "/game.bin";
}

void GameState::saveGameDataToFile()
{
    mSerializer.serialize(mWorldSeed);
    mSerializer.saveToFile(gameDataSaveFilePath());
}

GameState::~GameState()
{
    saveGameDataToFile();
}