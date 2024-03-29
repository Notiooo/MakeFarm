#ifndef STATE_H
#define STATE_H
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <memory>

#include "States.h"


class StateStack;

/**
 * \brief  The state that is on the stack performs the various
 * functions of updating, drawing and handling user inputs.
 */
class State
{
public:
    explicit State(StateStack& stack);
    State(const State&) = delete;
    State(State&&) = delete;

    State& operator=(const State&) = delete;
    State&& operator=(State&&) = delete;

    virtual ~State() = default;

    /**
     * \brief Draws only this state to the passed target
     * \param target where it should be drawn to
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * \brief Updates the state logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    virtual bool fixedUpdate(const float& deltaTime);

    /**
     * \brief Updates the game logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    virtual bool update(const float& deltaTime);

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    virtual bool handleEvent(const sf::Event& event);

protected:
    /**
     * \brief The state will be pushed out in the next iteration of the stack.
     * \param stateID Identifier of the state to be pushed
     */
    void requestPush(State_ID stateID) const noexcept;


    /**
     * \brief The state on the top of the stack will be removed in
     * the next iteration of the stack.
     */
    void requestPop() const noexcept;


    /**
     * \brief All states on the stack will be removed in the
     * next iteration of the stack.
     */
    void requestClear() const noexcept;

private:
    StateStack& mStack;//!< Pointer to the stack containing this state
};

#endif