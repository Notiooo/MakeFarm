#include "GameState.h"
#include "pch.h"


#include <SFML/Graphics/RenderWindow.hpp>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include "States/StateStack.h"
#include "Utils/Mouse.h"
#include "Utils/Settings.h"
#include "World/Block/BlockMap.h"

GameState::GameState(StateStack& stack, sf::RenderWindow& window)
    : State(stack)
    , mGameWindow(window)
    , mGameCamera(mGameWindow, mShader)
    , mGameSettings("settings.cfg")
    , mTexturePack("defaultTextures")
    , mChunkManager(mTexturePack)
    , mSelectedBlock(mTexturePack)
{
    Mouse::lockMouseAtCenter(mGameWindow);
    mShader.loadFromFile("resources/shaders/vertexShader.shader",
                         "resources/shaders/FragmentShader.shader");

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    sf::Shader::bind(&mShader);
    mShader.setUniform("u_ViewProjection", sf::Glsl::Mat4(sf::Transform::Identity));
    sf::Shader::bind(nullptr);

    auto test = BlockMap::blockMap();
}


bool GameState::handleEvent(const sf::Event& event)
{
    Mouse::handleFirstPersonBehaviour(event, mGameWindow);
    mGameCamera.handleEvent(event);

    switch (event.type)
    {
        case sf::Event::MouseButtonPressed:
        {
            if (event.key.code == sf::Mouse::Button::Left)
            {
                if (mSelectedBlock.isAnyBlockSelected())
                {
                    mChunkManager.chunks().removeWorldBlock(mSelectedBlock.blockPosition());
                }
            }
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

    mGameCamera.fixedUpdate(deltaTime);

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
                     std::filesystem::directory_iterator{"resources/textures"})
                {
                    if (texturePackDir.is_directory())
                    {
                        auto texturePackFolder = texturePackDir.path().filename().string();
                        if (ImGui::MenuItem(texturePackFolder.c_str()))
                        {
                            mTexturePack.loadTexturePack(texturePackFolder);
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
    mGameCamera.update(deltaTime);
    mSelectedBlock.markFacedBlock(mGameCamera, mChunkManager);

    mChunkManager.update(deltaTime);
    mChunkManager.generateChunksAround(mGameCamera);
    mChunkManager.clearFarAwayChunks(mGameCamera);

    updateDebugMenu();

    return true;
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mChunkManager.draw(mGameRenderer, mShader);
    mSelectedBlock.draw(mGameRenderer, mShader);
}
