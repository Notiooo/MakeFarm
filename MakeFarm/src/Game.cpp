#include "Game.h"
#include "pch.h"
#include <States/Application_States/MainMenuState.h>

#include "States/Application_States/DeathState.h"
#include "States/Application_States/ExitGameState.h"
#include "States/Application_States/GameState.h"
#include "States/Application_States/MainMenuState.h"
#include "States/Application_States/PauseState.h"
#include "States/States.h"
#include "Utils/Mouse.h"


constexpr int FRAMES_PER_SECOND = 120;

/**
 * @brief I kindly ask that the following variable not be changed, as the
 * physics calculations have been adjusted under the following framerate.
 * Thank you.
 *
 * @warning If you do not comply, the player will move differently and jump lower or higher.
 */
constexpr int MINIMAL_FIXED_UPDATES_PER_FRAME = 120;

const sf::Time Game::TIME_PER_FIXED_UPDATE_CALLS =
    sf::seconds(1.f / MINIMAL_FIXED_UPDATES_PER_FRAME);
const int Game::SCREEN_WIDTH = 1280;
const int Game::SCREEN_HEIGHT = 720;

Game::Game()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    mGameWindow =
        std::make_unique<sf::RenderWindow>(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "MakeFarm",
                                           sf::Style::Titlebar | sf::Style::Close, settings);

    mGameWindow->setFramerateLimit(FRAMES_PER_SECOND);
    mGameWindow->setActive(true);
    loadResources();

#ifdef _DEBUG
    ImGui::SFML::Init(*mGameWindow);
#endif

    // GLEW setup
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    // Setup all application-flow states
    mAppStack.saveState<GameState>(State_ID::GameState, *mGameWindow, mGameResources, mGameSession);
    mAppStack.saveState<DeathState>(State_ID::DeathState, *mGameWindow, mGameResources,
                                    mGameSession);
    mAppStack.saveState<PauseState>(State_ID::PauseState, *mGameWindow, mGameResources);
    mAppStack.saveState<ExitGameState>(State_ID::ExitGameState);
    mAppStack.saveState<MainMenuState>(State_ID::MainMenuState, *mGameWindow, mGameResources,
                                       mGameSession);

    // Initial state of the statestack is TitleState
    mAppStack.push(State_ID::MainMenuState);
}

void Game::run()
{
    // It controls the flow of the game loop
    // So the game is not framerate-dependent
    // so it works the same no matter what
    // performance has the player

    sf::Clock clock;
    auto frameTimeElapsed = sf::Time::Zero;
    mFixedUpdateClock.restart();
    while (isGameRunning)
    {
        frameTimeElapsed = clock.restart();
#ifdef _DEBUG
        ImGui::SFML::Update(*mGameWindow, frameTimeElapsed);
#endif
        update(frameTimeElapsed);
        fixedUpdateAtEqualIntervals();
        processEvents();

        render();
    }

    mGameWindow->close();
    ImGui::SFML::Shutdown();
}

void Game::fixedUpdateAtEqualIntervals()
{
    mTimeSinceLastFixedUpdate += mFixedUpdateClock.restart();
    if (mTimeSinceLastFixedUpdate > TIME_PER_FIXED_UPDATE_CALLS)
    {
        do
        {
            mTimeSinceLastFixedUpdate -= TIME_PER_FIXED_UPDATE_CALLS;
            fixedUpdate(TIME_PER_FIXED_UPDATE_CALLS);
        }
        while (mTimeSinceLastFixedUpdate > TIME_PER_FIXED_UPDATE_CALLS);
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (mGameWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            isGameRunning = false;
        }

#ifdef _DEBUG
        ImGui::SFML::ProcessEvent(event);
#endif

        mAppStack.handleEvent(event);
    }
}

void Game::fixedUpdate(const sf::Time& deltaTime)
{
    auto deltaTimeInSeconds = deltaTime.asSeconds();
    mAppStack.fixedUpdate(deltaTimeInSeconds);
}

void Game::update(const sf::Time& deltaTime)
{
    auto deltaTimeInSeconds = deltaTime.asSeconds();
    Mouse::update(deltaTimeInSeconds, *mGameWindow);

    mAppStack.update(deltaTimeInSeconds);

    if (mAppStack.top() == State_ID::ExitGameState)
    {
        isGameRunning = false;
    }
}

void Game::render()
{
    glClearColor(0.43f, 0.69f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the application
    mAppStack.draw(*mGameWindow, sf::Transform::Identity);

#ifdef _DEBUG
    mGameWindow->pushGLStates();
    ImGui::SFML::Render(*mGameWindow);
    mGameWindow->popGLStates();
#endif

    // display to the window
    mGameWindow->display();
}


void Game::loadResources()
{
    auto& fonts = mGameResources.fontManager;
    auto& texturePack = mGameResources.texturePack;

    fonts.storeResource(FontId::ArialNarrow, "resources/fonts/arial_narrow.ttf");

    std::string guiTexturesFolder = "resources/textures/gui/";
    loadInventoryTextures(guiTexturesFolder);
    loadHealthbarTextures(guiTexturesFolder);
    loadOxygenbarTextures(guiTexturesFolder);

    texturePack.loadTexturePack("defaultTextures");
}

void Game::loadOxygenbarTextures(const std::string& guiTexturesFolder)
{
    auto& textures = mGameResources.textureManager;
    auto oxygenFolder = guiTexturesFolder + "oxygenbar/";
    textures.storeResource(TextureManagerId::GUI_Oxygenbar_EmptyOxygen,
                           oxygenFolder + "empty_oxygen.png");
    textures.storeResource(TextureManagerId::GUI_Oxygenbar_FullOxygen,
                           oxygenFolder + "full_oxygen.png");
    textures.storeResource(TextureManagerId::GUI_Oxygenbar_HalfOxygen,
                           oxygenFolder + "half_oxygen.png");
}

void Game::loadHealthbarTextures(const std::string& guiTexturesFolder)
{
    auto& textures = mGameResources.textureManager;
    textures.storeResource(TextureManagerId::GUI_Healthbar_EmptyHeart,
                           guiTexturesFolder + "healthbar/empty_heart.png");
    textures.storeResource(TextureManagerId::GUI_Healthbar_FullHeart,
                           guiTexturesFolder + "healthbar/full_heart.png");
    textures.storeResource(TextureManagerId::GUI_Healthbar_HalfHeart,
                           guiTexturesFolder + "healthbar/half_heart.png");
}

void Game::loadInventoryTextures(const std::string& guiTexturesFolder)
{
    auto& textures = mGameResources.textureManager;
    textures.storeResource(TextureManagerId::GUI_Inventory_Background,
                           guiTexturesFolder + "inventory_background.png");
    textures.storeResource(TextureManagerId::GUI_Inventory_Slot,
                           guiTexturesFolder + "inventory_slot.png");
    textures.storeResource(TextureManagerId::GUI_Inventory_Selected_Block,
                           guiTexturesFolder + "hotbar/selected_block.png");
    textures.storeResource(TextureManagerId::GUI_Inventory_Unselected_Block,
                           guiTexturesFolder + "hotbar/unselected_block.png");
}
