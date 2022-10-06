#include "Game.h"
#include "pch.h"

#include "States/Application_States/GameState.h"
#include "States/States.h"
#include "Utils/Mouse.h"

constexpr int FRAMES_PER_SECOND = 120;
constexpr int MINIMAL_FIXED_UPDATES_PER_FRAME = 60;

const sf::Time Game::MINIMAL_TIME_PER_FIXED_UPDATE =
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
// clang-format off
    // ImGui setup
    #ifdef _DEBUG
    ImGui::SFML::Init(*mGameWindow);
    #endif
    // clang-format on

    // GLEW setup
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    // Setup all application-flow states
    mAppStack.saveState<GameState>(State_ID::GameState, *mGameWindow);

    // Initial state of the statestack is TitleState
    mAppStack.push(State_ID::GameState);
}

void Game::run()
{
    // It controls the flow of the game loop
    // So the game is not framerate-dependent
    // so it works the same no matter what
    // performance has the player

    sf::Clock clock;
    auto frameTimeElapsed = sf::Time::Zero;
    while (isGameRunning)
    {
        frameTimeElapsed = clock.restart();
// clang-format off
        #ifdef _DEBUG
        ImGui::SFML::Update(*mGameWindow, frameTimeElapsed);
        #endif
        // clang-format on
        update(frameTimeElapsed);
        performFixedUpdateAtLeastMinimalNumberOfTimes(frameTimeElapsed);
        processEvents();

        render();
    }

    mGameWindow->close();
    ImGui::SFML::Shutdown();
}

void Game::performFixedUpdateAtLeastMinimalNumberOfTimes(sf::Time& frameTimeElapsed)
{
    if (frameTimeElapsed > MINIMAL_TIME_PER_FIXED_UPDATE)
    {
        do
        {
            frameTimeElapsed -= MINIMAL_TIME_PER_FIXED_UPDATE;
            fixedUpdate(MINIMAL_TIME_PER_FIXED_UPDATE);
        }
        while (frameTimeElapsed > MINIMAL_TIME_PER_FIXED_UPDATE);
    }
    else
    {
        fixedUpdate(frameTimeElapsed);
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
// clang-format off
        #ifdef _DEBUG
        ImGui::SFML::ProcessEvent(event);
        #endif
        // clang-format on

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
}

void Game::render()
{
    // before drawing anything clean
    // the previous frame
    // gameWindow->clear();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the application
    mAppStack.draw(*mGameWindow, sf::Transform::Identity);

// clang-format off
    #ifdef _DEBUG
    mGameWindow->pushGLStates();
    ImGui::SFML::Render(*mGameWindow);
    mGameWindow->popGLStates();
    #endif
    // clang-format on

    // display to the window
    mGameWindow->display();
}


void Game::loadResources()
{
    mFonts.storeResource(Fonts_ID::ArialNarrow, "resources/fonts/arial_narrow.ttf");
}