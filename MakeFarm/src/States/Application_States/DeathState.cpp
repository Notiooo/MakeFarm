#include "DeathState.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Mouse.h"
#include "Utils/utils.h"
#include "pch.h"

DeathState::DeathState(StateStack& stack, sf::RenderWindow& window, GameResources& gameResources)
    : State(stack)
    , mGameResources(gameResources)
    , mGameWindow(window)
    , mDeathScreenBackground()
    , mDeathText()
{
    setupDeathScreenBackground(window);
    setupDeathText(window, gameResources.fontManager);
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
}

bool DeathState::update(const float& deltaTime)
{
    Mouse::update(deltaTime, mGameWindow);
    return false;
}

bool DeathState::fixedUpdate(const float& deltaTime)
{
    return false;
}

bool DeathState::handleEvent(const sf::Event& event)
{
    return false;
}
