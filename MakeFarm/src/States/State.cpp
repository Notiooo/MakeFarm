#include "pch.h"
#include "State.h"

#include "StateStack.h"

State::State(StateStack& stack): stack(&stack)
{

}

bool State::fixedUpdate(const float& deltaTime)
{
	/*
	 * Set this state to transparent -- in other words
	 * allow States below in stack to be rendered.
	 */
	return true;
}

bool State::update()
{
	/*
	 * Set this state to transparent -- in other words
	 * allow States below in stack to be rendered.
	 */
	return true;
}

void State::requestPush(State_ID stateID) const noexcept
{
	stack->push(stateID);
}

void State::requestPop() const noexcept
{
	stack->pop();
}

void State::requestClear() const noexcept
{
	stack->clear();
}
