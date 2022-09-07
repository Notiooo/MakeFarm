#pragma once
#include <gmock/gmock.h>
#include <SFML/Window/Event.hpp>
#include "States/StateStack.h"

class MockStateStack : public StateStack {
public:
    MOCK_METHOD(void, draw, (sf::RenderTarget& target, sf::RenderStates states), (const));
    MOCK_METHOD(void, fixedUpdate, (const float& deltaTime));
    MOCK_METHOD(void, update, ());
    MOCK_METHOD(void, handleEvent, (const sf::Event& event));
    MOCK_METHOD(void, push, (State_ID stateID));
    MOCK_METHOD(void, pop, ());
    MOCK_METHOD(void, clear, ());
    MOCK_METHOD(bool, empty, (), (const, noexcept));
};