#include "pch.h"
#include "Game.h"

#include "Utils/Mouse.h"
#include "States/States.h"
#include "States/Application_States/GameState.h"

const sf::Time Game::TIME_PER_FRAME = sf::seconds(1.f / 60.f);
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

    mGameWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                                                     "Minecraft Clone", sf::Style::Titlebar | sf::Style::Close, settings);

	// Limit the framerate to 60 frames per second
	mGameWindow->setFramerateLimit(60);
	mGameWindow->setActive(true);

	// ImGui setup
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
	mAppStack.saveState<GameState>(State_ID::GameState, *mGameWindow);

	// load resources
	loadResources();

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
		frameTimeElapsed += clock.restart();
		
		while (frameTimeElapsed > TIME_PER_FRAME)
		{
			// Update world no more than 60 frames per seconds
			frameTimeElapsed -= TIME_PER_FRAME;
			processEvents();
			fixedUpdate(TIME_PER_FRAME);
		}

		#ifdef _DEBUG
		ImGui::SFML::Update(*mGameWindow, frameTimeElapsed);
		#endif
		update();

		render();
	}

	mGameWindow->close();
	ImGui::SFML::Shutdown();
}

void Game::processEvents()
{
	sf::Event event;
	while (mGameWindow->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			isGameRunning = false;

		#ifdef _DEBUG
		ImGui::SFML::ProcessEvent(event);
		#endif
		
		mAppStack.handleEvent(event);
	}
}

void Game::fixedUpdate(sf::Time deltaTime)
{
	auto deltaTimeInSeconds = deltaTime.asSeconds();

    Mouse::fixedUpdate(deltaTimeInSeconds, *mGameWindow);
	mAppStack.fixedUpdate(deltaTimeInSeconds);
}

void Game::update()
{
	mAppStack.update();
}

void Game::render()
{
	// before drawing anything clean
	// the previous frame
	//gameWindow->clear();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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
	mFonts.storeResource(Fonts_ID::ArialNarrow, "resources/fonts/arial_narrow.ttf");
}