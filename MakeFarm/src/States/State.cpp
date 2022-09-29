#include "pch.h"
#include "State.h"

#include "StateStack.h"

State::State(StateStack& stack): mStack(stack)
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
	mStack.push(stateID);
}

void State::requestPop() const noexcept
{
	mStack.pop();
}

void State::requestClear() const noexcept
{
	mStack.clear();
}
