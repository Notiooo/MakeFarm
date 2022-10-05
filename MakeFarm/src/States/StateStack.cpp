#include "pch.h"
#include "StateStack.h"

auto StateStack::createState(const State_ID& stateID)
{
    const auto found = mFactory.find(stateID);
    assert(found != mFactory.end());

    // As it find specific recipe in factory that
    // creates a state, then after it is found
    // we call that function to get a pointer to
    // this state, and we return it.
    return found->second();
}

void StateStack::applyChanges()
{
    for (const auto& change : mChangesQueue)
    {
        switch (change.operation)
        {
        case Perform::Push:
            mStack.push_back(createState(change.stateID)); break;

        case Perform::Pop:
            mStack.pop_back(); break;

        case Perform::Clear:
            mStack.clear(); break;
        }
    }
    mChangesQueue.clear();
}

void StateStack::fixedUpdate(const float& deltaTime)
{
    applyChanges();
    // Iterate from the highest state to the lowest state, and stop iterating if
    // any state returns 
    for (auto beg = mStack.rbegin(), end = mStack.rend(); beg != end; ++beg)
    {
        // If a state is of "Transparent" type, then we iterate further
        // But if it returns false, then we stop iterating. 

        // This allow some states to pause states under it.
        // Like pause for example
        if (!(*beg)->fixedUpdate(deltaTime))
            return;
    }
}

void StateStack::update(const float& deltaTime)
{
    applyChanges();
    // Iterate from the highest state to the lowest state, and stop iterating if
    // any state returns 
    for (auto beg = mStack.rbegin(), end = mStack.rend(); beg != end; ++beg)
    {
        // If a state is of "Transparent" type, then we iterate further
        // But if it returns false, then we stop iterating. 

        // This allow some states to pause states under it.
        // Like pause for example
        if (!(*beg)->update(deltaTime))
            return;
    }
}

void StateStack::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Drawing starts from the lowest state to the highest state
    for (const auto& state : mStack)
	    state->draw(target, states);
}

void StateStack::handleEvent(const sf::Event& event)
{
    applyChanges();

    // Iterate from the highest state to the lowest state, and stop iterating if
    // any state returns 
    for (auto beg = mStack.rbegin(), end = mStack.rend(); beg != end; ++beg)
    {
        // If a state is of "Transparent" type, then we iterate further
        // But if it returns false, then we stop iterating. 

        // This allow some states to pause states under it.
        // Like pause for example
        if (!(*beg)->handleEvent(event))
            return;
    }
}


void StateStack::push(State_ID stateID)
{
    mChangesQueue.emplace_back(Change{Perform::Push, stateID });
}

void StateStack::pop()
{
    mChangesQueue.emplace_back(Change{Perform::Pop, State_ID::None });
}

void StateStack::clear()
{
    mChangesQueue.emplace_back(Change{Perform::Clear, State_ID::None });
}

bool StateStack::empty() const noexcept
{
    return mChangesQueue.empty();
}
