#include "MainMenuState.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Mouse.h"
#include "Utils/utils.h"
#include "pch.h"
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>

MainMenuState::MainMenuState(StateStack& stack, sf::RenderWindow& window,
                             GameResources& gameResources, GameSession& gameSession)
    : State(stack)
    , mBackgroundTexture()
    , mBackground()
    , mGameWindow(window)
    , mGui(window)
    , mGameSession(gameSession)
{
    setupBackground(window);
    showMainMenu();

    Mouse::unlockMouse(window);
}

void MainMenuState::setupBackground(const sf::RenderWindow& window)
{
    mBackgroundTexture.loadFromFile("resources/textures/mainMenu/background.png");
    mBackgroundTexture.setRepeated(true);
    mBackground.setScale(3, 3);
    mBackground.setTexture(mBackgroundTexture);
    auto windowSize = static_cast<sf::Vector2i>(window.getSize());
    mBackground.setTextureRect({0, 0, windowSize.x, windowSize.y});
}

void MainMenuState::showMainMenu()
{
    mGui.removeAllWidgets();

    auto singlePlayerButtonPosition =
        tgui::Layout2d({"(&.width - width) / 2", "(&.height - height) / 2"});
    auto singlePlayerButton = createSinglePlayerButton(singlePlayerButtonPosition);

    auto quitGameButtonPosition = tgui::Layout2d(
        {bindLeft(singlePlayerButton), bindBottom(singlePlayerButton) + GAP_BETWEEN_BUTTONS});
    createQuitGameButton(quitGameButtonPosition);

    auto gameLogo = tgui::Picture::create("resources/textures/mainMenu/logo.png");
    gameLogo->setPosition(
        {"(&.width - width) / 2",
         bindTop(singlePlayerButton) - gameLogo->getSize().y - GAP_BETWEEN_BUTTONS});
    gameLogo->showWithEffect(tgui::ShowAnimationType::Fade, 800);
    mGui.add(gameLogo);
}

tgui::Button::Ptr MainMenuState::createQuitGameButton(const tgui::Layout2d& buttonPosition)
{
    auto quitGameButton = tgui::Button::create("QUIT GAME");
    quitGameButton->setSize(BUTTON_SIZE);
    quitGameButton->setPosition(buttonPosition);
    quitGameButton->onPress(
        [this]
        {
            requestClear();
            requestPush(State_ID::ExitGameState);
        });
    quitGameButton->showWithEffect(tgui::ShowAnimationType::Fade, 800);
    mGui.add(quitGameButton);
    return quitGameButton;
}

tgui::Button::Ptr MainMenuState::createSinglePlayerButton(const tgui::Layout2d& buttonPosition)
{
    auto singlePlayerButton = tgui::Button::create("SINGLEPLAYER");
    singlePlayerButton->setSize(BUTTON_SIZE);
    singlePlayerButton->setPosition(buttonPosition);
    singlePlayerButton->onPress(
        [this]
        {
            showWorldSelection();
        });
    singlePlayerButton->showWithEffect(tgui::ShowAnimationType::Fade, 800);
    mGui.add(singlePlayerButton);
    return singlePlayerButton;
}

void MainMenuState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    SfmlDraw(mBackground, target, states);

    target.pushGLStates();
    const_cast<MainMenuState*>(this)->mGui.draw();
    target.popGLStates();
}

bool MainMenuState::update(const float& deltaTime)
{
    Mouse::update(deltaTime, mGameWindow);
    mGui.updateTime(static_cast<int>(deltaTime * 1000.f));
    return false;
}

bool MainMenuState::fixedUpdate(const float& deltaTime)
{
    return false;
}

bool MainMenuState::handleEvent(const sf::Event& event)
{
    mGui.handleEvent(event);
    return false;
}
void MainMenuState::showWorldSelection()
{
    mGui.removeAllWidgets();

    auto panel = createWorldSelectionScrollablePanel();

    auto goBackMainMenuButtonPosition = tgui::Layout2d(
        {"(&.width - width) / 2", bindBottom(panel) + GAP_BETWEEN_BUTTONS * 2.f + BUTTON_SIZE.y});
    createGoBackToMainMenuButton(goBackMainMenuButtonPosition);

    auto editWorldNameBoxPosition =
        tgui::Layout2d({"(&.width / 2) - width", bindBottom(panel) + GAP_BETWEEN_BUTTONS});
    auto editWorldNameBox = createEditWorldNameBox(editWorldNameBoxPosition);

    auto newWorldButtonPosition =
        tgui::Layout2d({bindRight(editWorldNameBox), bindTop(editWorldNameBox)});
    createNewWorldButton(editWorldNameBox, newWorldButtonPosition);

    loadSavesIntoWorldSelectionScrollablePanel(panel);
}

void MainMenuState::loadSavesIntoWorldSelectionScrollablePanel(
    std::shared_ptr<tgui::ScrollablePanel> panel) const
{
    auto entryNumber = 0;
    std::filesystem::create_directories(Resources::SAVE_DIRECTORY_FOLDER);
    for (const auto& entry: std::filesystem::directory_iterator(Resources::SAVE_DIRECTORY_FOLDER))
    {
        auto worldName = entry.path().filename().string();
        auto worldSave = tgui::Button::create(worldName);
        worldSave->setSize(BUTTON_SIZE);
        auto padding = GAP_BETWEEN_BUTTONS * entryNumber;
        auto buttonPosition = entryNumber * worldSave->getSize().y;
        worldSave->setPosition({"(&.width - width) / 2", 20.f + padding + buttonPosition});
        worldSave->onPress(
            [this, worldName]
            {
                mGameSession.currentlyPlayedWorld =
                    Resources::SAVE_DIRECTORY_FOLDER + "/" + worldName;
                requestClear();
                requestPush(State_ID::GameState);
            });
        panel->add(worldSave);

        ++entryNumber;
    }
}

tgui::Button::Ptr MainMenuState::createNewWorldButton(tgui::EditBox::Ptr worldNameBox,
                                                      const tgui::Layout2d& boxPosition)
{
    auto createNewWorldButton = tgui::Button::create("CREATE NEW WORLD");
    createNewWorldButton->setSize(BUTTON_SIZE);
    createNewWorldButton->showWithEffect(tgui::ShowAnimationType::Fade, 800);
    createNewWorldButton->setPosition(boxPosition);
    createNewWorldButton->onPress(
        [this, worldNameBox]
        {
            auto worldName = worldNameBox->getText().toStdString();
            worldName = (worldName.empty() ? "New world" : worldName);
            worldName = makeWorldNameUnique(worldName);
            auto worldPath = Resources::SAVE_DIRECTORY_FOLDER + "/" + worldName;

            std::filesystem::create_directories(worldPath);
            mGameSession.currentlyPlayedWorld = worldPath;
            requestClear();
            requestPush(State_ID::GameState);
        });
    mGui.add(createNewWorldButton);
    return createNewWorldButton;
}

std::string MainMenuState::makeWorldNameUnique(const std::string& worldName) const
{
    auto worldNameToCheck = worldName;
    for (auto worldIndex = 1; doesGivenWorldExist(worldNameToCheck); ++worldIndex)
    {
        worldNameToCheck = worldName + " (" + std::to_string(worldIndex) + ")";
    }
    return worldNameToCheck;
}

bool MainMenuState::doesGivenWorldExist(const std::string& worldName) const
{
    return std::filesystem::exists(Resources::SAVE_DIRECTORY_FOLDER + "/" + worldName);
}

tgui::EditBox::Ptr MainMenuState::createEditWorldNameBox(const tgui::Layout2d& boxPosition)
{
    auto editWorldName = tgui::EditBox::create();
    editWorldName->setSize(BUTTON_SIZE);
    editWorldName->setPosition(boxPosition);
    editWorldName->setDefaultText("New world name...");
    mGui.add(editWorldName);
    return editWorldName;
}

void MainMenuState::createGoBackToMainMenuButton(const tgui::Layout2d& buttonPosition)
{
    auto goBackToMainMenuButton = tgui::Button::create("MAIN MENU");
    goBackToMainMenuButton->setSize(BUTTON_SIZE);
    goBackToMainMenuButton->showWithEffect(tgui::ShowAnimationType::Fade, 800);
    goBackToMainMenuButton->setPosition(buttonPosition);
    goBackToMainMenuButton->onPress(
        [this]
        {
            showMainMenu();
        });
    mGui.add(goBackToMainMenuButton);
}

tgui::ScrollablePanel::Ptr MainMenuState::createWorldSelectionScrollablePanel()
{
    auto panel = tgui::ScrollablePanel::create();
    panel->setSize({BUTTON_SIZE.x + LEVEL_SELECTION_PADDING, "70%"});
    panel->setPosition({"(&.width - width) / 2", "10%"});
    panel->showWithEffect(tgui::ShowAnimationType::Fade, 800);
    mGui.add(panel);
    return panel;
}
