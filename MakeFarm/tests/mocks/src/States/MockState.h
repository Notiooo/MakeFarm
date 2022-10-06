#pragma once
#include "States/State.h"
#include <SFML/Window/Event.hpp>
#include <gmock/gmock.h>

class MockState : public State
{
public:
    using State::State;

    MOCK_METHOD(void, draw, (sf::RenderTarget & target, sf::RenderStates states),
                (const, override));
    MOCK_METHOD(bool, fixedUpdate, (const float& deltaTime), (override));
    MOCK_METHOD(bool, update, (), (override));
    MOCK_METHOD(bool, handleEvent, (const sf::Event& event), (override));
};