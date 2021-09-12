#include "pch.h"
#include "GameState.h"



#include <iomanip>
#include <sstream>
#include <SFML/Graphics/RenderWindow.hpp>

#include "States/StateStack.h"
#include "Utils/Mouse.h"

GameState::GameState(StateStack& stack, sf::RenderWindow& window):
	State(stack),
	gameWindow(window),
	gameCamera(gameWindow, shader)
{
	Mouse::lockMouseAtCenter(gameWindow);
	shader.loadFromFile("resources/Shaders/VertexShader.shader", "resources/Shaders/FragmentShader.shader");
	
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	sf::Shader::bind(&shader);
	shader.setUniform("u_MVP", sf::Glsl::Mat4(sf::Transform::Identity));
	sf::Shader::bind(nullptr);

	testChunk.createChunk();
}


bool GameState::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
			Mouse::unlockMouse(gameWindow);
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.key.code == sf::Mouse::Left)
		{
			if (!ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsAnyItemActive())
			{
				Mouse::lockMouseAtCenter(gameWindow);
			}
		}
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		Mouse::lockMouseAtCenter(gameWindow);
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

	gameCamera.fixedUpdate(deltaTime);
	
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

				if(rastMode[1] == GL_FILL)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

bool GameState::update()
{
	gameCamera.update();

	updateDebugMenu();

	return true;
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//gameRenderer.clear();
	testChunk.draw(gameRenderer, shader);
}
