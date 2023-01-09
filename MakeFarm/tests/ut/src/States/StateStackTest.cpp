#include "States/StateStack.h"
#include "States/States.h"
#include "TestUtils/SFML/EventEqualityOperator.h"
#include "TestUtils/SFML/Stubs/RenderTargetStub.h"
#include "gtest/gtest.h"
#include <States/MockState.h>

namespace
{

class UglyTestableStateStack : public StateStack
{
public:
    template<typename State, typename... Args>
    State& forcePushStateAndReturn(State_ID stateID, Args&&... args)
    {
        auto state = std::make_unique<State>(*this, std::forward<Args>(args)...);
        auto& stateRef = *state;
        mStack.push_back({stateID, std::move(state)});
        return stateRef;
    }
};

TEST(StateStackTest, StateStackShouldBeEmptyOnDefault)
{
    StateStack s;
    EXPECT_TRUE(s.empty());
}

TEST(StateStackTest, StateStackShouldBeEmptyAfterOnlySaveState)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    EXPECT_TRUE(s.empty());
}

TEST(StateStackTest, StateShouldNotBePushedBeforeUpdateOrHandleEvent)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.push(State_ID::GameState);
    EXPECT_TRUE(s.empty());
}

TEST(StateStackTest, StateShouldBePushedAfterUpdate)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.push(State_ID::GameState);
    s.update(0);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), State_ID::GameState);
}

TEST(StateStackTest, StateShouldBePushedAfterFixedUpdate)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.push(State_ID::GameState);
    s.fixedUpdate(0);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), State_ID::GameState);
}

TEST(StateStackTest, StateShouldBePushedAfterHandleEvent)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.push(State_ID::GameState);
    s.handleEvent(sf::Event());
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), State_ID::GameState);
}

TEST(StateStackTest, CorrectStateIdShouldBeOnTopAfterPushTwice)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.saveState<State>(State_ID::PauseState);
    s.push(State_ID::GameState);
    s.push(State_ID::PauseState);
    s.update(0);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), State_ID::PauseState);
}

TEST(StateStackTest, StateStackCorrectPop)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.push(State_ID::GameState);
    s.update(0);
    EXPECT_FALSE(s.empty());
    s.pop();
    EXPECT_FALSE(s.empty());
    s.update(0);
    EXPECT_TRUE(s.empty());
}

TEST(StateStackTest, StateStackCorrectClear)
{
    StateStack s;
    s.saveState<State>(State_ID::GameState);
    s.push(State_ID::GameState);
    s.push(State_ID::GameState);
    s.update(0);
    EXPECT_FALSE(s.empty());
    s.clear();
    EXPECT_FALSE(s.empty());
    s.update(0);
    EXPECT_TRUE(s.empty());
}

TEST(StateStackTest, StateStackCorrectUpdate)
{
    UglyTestableStateStack s;
    auto& mock = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    EXPECT_CALL(mock, update(15));
    s.update(15);
}

TEST(StateStackTest, StateStackUpdateBlockingStateBelow)
{
    UglyTestableStateStack s;
    auto& mock1 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    auto& mock2 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    EXPECT_CALL(mock2, update(15)).WillOnce(testing::Return(false));
    EXPECT_CALL(mock1, update(15)).Times(0);
    s.update(15);
}

TEST(StateStackTest, StateStackUpdateNotBlockingStateBelow)
{
    UglyTestableStateStack s;
    auto& mock1 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    auto& mock2 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    EXPECT_CALL(mock2, update(15)).WillOnce(testing::Return(true));
    EXPECT_CALL(mock1, update(15));
    s.update(15);
}

TEST(StateStackTest, StateStackCorrectFixedUpdate)
{
    UglyTestableStateStack s;
    auto& mock = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    EXPECT_CALL(mock, fixedUpdate(15));
    s.fixedUpdate(15);
}

TEST(StateStackTest, StateStackFixedUpdateBlockingStateBelow)
{
    UglyTestableStateStack s;
    auto& mock1 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    auto& mock2 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    EXPECT_CALL(mock2, fixedUpdate(15)).WillOnce(testing::Return(false));
    EXPECT_CALL(mock1, fixedUpdate(15)).Times(0);
    s.fixedUpdate(15);
}

TEST(StateStackTest, StateStackFixedUpdateNotBlockingStateBelow)
{
    UglyTestableStateStack s;
    auto& mock1 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    auto& mock2 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    EXPECT_CALL(mock2, fixedUpdate(15)).WillOnce(testing::Return(true));
    EXPECT_CALL(mock1, fixedUpdate(15));
    s.fixedUpdate(15);
}

TEST(StateStackTest, StateStackCorrectHandleEvent)
{
    UglyTestableStateStack s;
    auto& mock = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;

    EXPECT_CALL(mock, handleEvent(event));
    s.handleEvent(event);
}

TEST(StateStackTest, StateStackDrawIsCalled)
{
    UglyTestableStateStack s;
    auto& mock = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    RenderTargetStub renderTargetStub;
    sf::RenderStates renderStates;
    EXPECT_CALL(mock, draw(testing::_, testing::_));
    s.draw(renderTargetStub, renderStates);
}

TEST(StateStackTest, StateStackHandleEventBlockingStateBelow)
{
    UglyTestableStateStack s;
    auto& mock1 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    auto& mock2 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;

    EXPECT_CALL(mock2, handleEvent(event)).WillOnce(testing::Return(false));
    EXPECT_CALL(mock1, handleEvent(event)).Times(0);
    s.handleEvent(event);
}

TEST(StateStackTest, StateStackHandleEventNotBlockingStateBelow)
{
    UglyTestableStateStack s;
    auto& mock1 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    auto& mock2 = s.forcePushStateAndReturn<MockState>(State_ID::GameState);
    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;

    EXPECT_CALL(mock2, handleEvent(event)).WillOnce(testing::Return(true));
    EXPECT_CALL(mock1, handleEvent(event));
    s.handleEvent(event);
}


}// namespace