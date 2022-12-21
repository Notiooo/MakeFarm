#include "DeathState.h"
#include "Player/Player.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Mouse.h"
#include "Utils/utils.h"
#include "pch.h"
#include <TGUI/Widgets/Button.hpp>

DeathState::DeathState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources,
                       GameSession& gameSession)
    : State(stack)
    , mGameResources(gameResources)
    , mGameWindow(window)
    , mDeathScreenBackground()
    , mDeathText()
    , mGui(window)
    , mPlayer(*gameSession.player)
{
    setupDeathScreenBackground(window);
    setupDeathText(window, gameResources.fontManager);

    auto saveAndGoBackMenuButtonLayout =
        tgui::Layout2d({"(&.width - width) / 2", "(&.height - height) / 2"});
    auto saveAndGoBackMenuButton = createSaveAndGoMenuButton(saveAndGoBackMenuButtonLayout);

    auto respawnButtonLayout =
        tgui::Layout2d({bindLeft(saveAndGoBackMenuButton),
                        bindBottom(saveAndGoBackMenuButton) + GAP_BETWEEN_BUTTONS});
    createRespawnButton(respawnButtonLayout);

    Mouse::unlockMouse(window);
}

void DeathState::setupDeathScreenBackground(const sf::RenderWindow& window)
{
    auto screenColor = sf::Color::Red;
    screenColor.a = 200;
    mDeathScreenBackground.setFillColor(screenColor);
    mDeathScreenBackground.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
}

void DeathState::setupDeathText(const sf::RenderWindow& window, const FontManager& fontManager)
{
    mDeathText.setFont(fontManager.getResourceReference(FontId::ArialNarrow));
    mDeathText.setOutlineThickness(1.f);
    mDeathText.setOutlineColor(sf::Color::Black);
    mDeathText.setCharacterSize(48);
    mDeathText.setString("You died");
    centerOrigin(mDeathText);
    mDeathText.setPosition(window.getSize().x / 2.f, window.getSize().y / 4.f);
}

void DeathState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    SfmlDraw(mDeathScreenBackground, target, states);
    SfmlDraw(mDeathText, target, states);

    target.pushGLStates();
    const_cast<DeathState*>(this)->mGui.draw();
    target.popGLStates();
}

bool DeathState::update(const float& deltaTime)
{
    Mouse::update(deltaTime, mGameWindow);
    mGui.updateTime(static_cast<int>(deltaTime * 1000.f));
    return false;
}

bool DeathState::fixedUpdate(const float& deltaTime)
{
    return false;
}

bool DeathState::handleEvent(const sf::Event& event)
{
    mGui.handleEvent(event);
    return false;
}

std::shared_ptr<tgui::Button> DeathState::createSaveAndGoMenuButton(
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

std::shared_ptr<tgui::Button> DeathState::createRespawnButton(tgui::Layout2d buttonPosition)
{
    auto respawnButton = tgui::Button::create("RESPAWN");
    respawnButton->setSize(BUTTON_SIZE);
    respawnButton->setPosition(buttonPosition);
    respawnButton->onPress(
        [this]
        {
            requestPop();
            Mouse::lockMouseAtCenter(mGameWindow);
            mPlayer.respawn();
        });
    mGui.add(respawnButton);
    return respawnButton;
}
