#include "PauseState.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Mouse.h"
#include "Utils/utils.h"
#include "pch.h"
#include <TGUI/Widgets/Button.hpp>

PauseState::PauseState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources)
    : State(stack)
    , mGameResources(gameResources)
    , mGameWindow(window)
    , mPauseScreenBackground()
    , mPauseText()
    , mGui(window)
    , mWasMouseLocked(Mouse::isMouseLocked())
{
    setupPauseScreenBackground(window);
    setupPauseText(window, gameResources.fontManager);

    auto saveAndQuitButtonPosition =
        tgui::Layout2d({"(&.width - width) / 2", "(&.height - height) / 2"});
    auto saveAndQuitButton = createSaveAndQuitButton(saveAndQuitButtonPosition);

    auto saveAndGoBackMenuButton = tgui::Layout2d(
        {bindLeft(saveAndQuitButton), bindBottom(saveAndQuitButton) + GAP_BETWEEN_BUTTONS});
    createSaveAndGoMenuButton(saveAndGoBackMenuButton);

    Mouse::unlockMouse(window);
}

void PauseState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    SfmlDraw(mPauseScreenBackground, target, states);
    SfmlDraw(mPauseText, target, states);

    target.pushGLStates();
    const_cast<PauseState*>(this)->mGui.draw();
    target.popGLStates();
}

void PauseState::setupPauseScreenBackground(const sf::RenderWindow& window)
{
    auto screenColor = sf::Color::Black;
    screenColor.a = 200;
    mPauseScreenBackground.setFillColor(screenColor);
    mPauseScreenBackground.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
}

void PauseState::setupPauseText(const sf::RenderWindow& window, const FontManager& fontManager)
{
    mPauseText.setFont(fontManager.getResourceReference(FontId::ArialNarrow));
    mPauseText.setOutlineThickness(1.f);
    mPauseText.setOutlineColor(sf::Color::Black);
    mPauseText.setCharacterSize(48);
    mPauseText.setString("Paused");
    centerOrigin(mPauseText);
    mPauseText.setPosition(window.getSize().x / 2.f, window.getSize().y / 4.f);
}

bool PauseState::update(const float& deltaTime)
{
    Mouse::update(deltaTime, mGameWindow);
    mGui.updateTime(static_cast<int>(deltaTime * 1000.f));
    return false;
}

bool PauseState::fixedUpdate(const float& deltaTime)
{
    return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
    mGui.handleEvent(event);
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape:
                if (mWasMouseLocked)
                {
                    Mouse::lockMouseAtCenter(mGameWindow);
                }
                requestPop();
                break;
        }
    }
    return false;
}

std::shared_ptr<tgui::Button> PauseState::createSaveAndQuitButton(
    const tgui::Layout2d& buttonPosition)
{
    auto saveAndQuitButton = tgui::Button::create("SAVE AND QUIT");
    saveAndQuitButton->setSize(BUTTON_SIZE);
    saveAndQuitButton->setPosition(buttonPosition);
    saveAndQuitButton->onPress(
        [this]
        {
            requestClear();
            requestPush(State_ID::ExitGameState);
        });
    mGui.add(saveAndQuitButton);
    return saveAndQuitButton;
}

std::shared_ptr<tgui::Button> PauseState::createSaveAndGoMenuButton(
    const tgui::Layout2d& buttonPosition)
{
    auto saveAndGoBackMenuButton = tgui::Button::create("SAVE AND GO BACK TO MENU");
    saveAndGoBackMenuButton->setSize(BUTTON_SIZE);
    saveAndGoBackMenuButton->setPosition(buttonPosition);
    saveAndGoBackMenuButton->onPress(
        [this]
        {
            requestClear();
            requestPush(State_ID::MainMenuState);
        });
    mGui.add(saveAndGoBackMenuButton);
    return saveAndGoBackMenuButton;
}
