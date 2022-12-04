#pragma once

#include "Resources/Resources.h"
#include "States/State.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

class StateStack;

/**
 * @brief The state in which the player wants to close the game
 */
class ExitGameState : public State
{
public:
    explicit ExitGameState(StateStack& stack);
};