#include "pch.h"
#include "GameState.h"


#include <filesystem>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics/RenderWindow.hpp>

#include "States/StateStack.h"
#include "Utils/Mouse.h"
#include "Utils/Settings.h"
#include "World/Block/BlockMap.h"

GameState::GameState(StateStack& stack, sf::RenderWindow& window) :
	State(stack),
	gameWindow(window),
	gameCamera(gameWindow, shader),
	gameSettings("settings.cfg"),
	texturePack("defaultTextures"),
	testChunk(texturePack)
{
	Mouse::lockMouseAtCenter(gameWindow);
	shader.loadFromFile("resources/Shaders/VertexShader.shader", "resources/Shaders/FragmentShader.shader");

	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	sf::Shader::bind(&shader);
	shader.setUniform("u_ViewProjection", sf::Glsl::Mat4(sf::Transform::Identity));
	sf::Shader::bind(nullptr);
	std::cout << "Texture pack loaded: " << gameSettings.get<std::string>("TexturePack") << std::endl;

	auto test = BlockMap::getBlockMap();
}


bool GameState::handleEvent(const sf::Event& event)
{
	Mouse::handleFirstPersonBehaviour(event, gameWindow);
	gameCamera.handleEvent(event);

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

		if (ImGui::BeginMenu("MakeFarm"))
		{
			if(ImGui::BeginMenu("Texture Packs"))
			{
				for (auto const& texturePackDir : std::filesystem::directory_iterator{ "resources/Textures" })
				{
					if (texturePackDir.is_directory())
					{
						auto texturePackFolder = texturePackDir.path().filename().string();
						if (ImGui::MenuItem(texturePackFolder.c_str()))
						{
							texturePack.loadTexturePack(texturePackFolder);
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

bool GameState::update()
{
	gameCamera.update();

	updateDebugMenu();

	return true;
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	testChunk.draw(gameRenderer, shader);
}
