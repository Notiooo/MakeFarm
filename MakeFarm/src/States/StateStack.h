#ifndef STATESTACK_H
#define STATESTACK_H

#include <functional>
#include <map>
#include <vector>

#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/System/NonCopyable.hpp"
#include "State.h"
#include "States.h"

/**
 * \brief It allows you to create a certain flow of states inside application.
 *
 *
 * Statestack can control the flow of the game.
 * Change between states not only in the context of the application itself,
 * but also, for example, in the context of a single object.
 *
 *
 * |           |      |           |      |           |
 * |           |pop() |           |push()|           |
 * |           |+---->|           |+---->|           |
 * |           |      |           |      |           |
 * |Title state|      |           |      |Game  state|
 * +-----------+      +-----------+      +-----------+
 *  Example of changing state from one to the other
 *
 *
 *  StateStack allows states at the top to cover states below.
 *  Thus, for example, a "Game State" can be covered by a "Pause State",
 *  which will block it from updating game logic and displaying itself.
 *  Of course, the pop() operation will remove the "Pause State" and
 *  restore the covered "Game State" perfectly from when it was covered.
 *  States, however, have the option of marking individual functions as
 *  "transparent", i.e. not blocking the layer below. The transparent layer
 *  (state) is the state that returns "true".
 *
 */
class StateStack : sf::NonCopyable
{
public:
    StateStack() = default;

    /**
     * \brief All actions that may be performed on the StateStack.
     */
    enum class Perform
    {
        Push,
        Pop,
        Clear,
    };

    /**
     * \brief Creates callable function that generate the State
     * object providing the given args to its constructor
     *
     * \tparam State The state (class) to be created in a future call.
     * \tparam Args Arguments to be passed to the constructor of the specified class.
     * \param stateID The identifier that will be used to create the class object.
     *
     * A factory that allows to create any state that Identifier is passed to this function.
     * This allows to not create all states at once and thanks to this avoid unnecesserily
     * big containers holding all those states that might be never used.
     *
     * By default it of course just pass an textInformation to the factory how to create such
     * an object nothing is yet created
     */
    template<typename State, typename... Args>
    void saveState(State_ID stateID, Args&&...);

    //  === Typical cyclic functions in NodeScene system === //


    /**
     * \brief Updates the game logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     *
     * Updates the state game logic at the top of the stack. If the state is
     * transparent (returns true) then it also updates the state logic below it.
     * The state below it is also checked for transparency and the process repeats itself.
     */
    void fixedUpdate(const float& deltaTime);

    /**
     * \brief Updates the game logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     *
     * Updates the state game logic at the top of the stack. If the state is
     * transparent (returns true) then it also updates the state logic below it.
     * The state below it is also checked for transparency and the process repeats itself.
     */
    void update(const float& deltaTime);

    /**
     * \brief Draws the states in the stack to the given target with given states.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     *
     * Draws the states at the top of the stack. If the state is transparent (returns true)
     * then it also draw the state below it. The state below it is also checked for
     * transparency and the process repeats itself.
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;


    /**
     * \brief It takes input (event) from the user and sends it to the states on the stack.
     * \param event user input
     *
     * It takes input (event) from the user and then passes it to the state on top of the stack.
     * If the state is transparent (returns true) then it also passes the state below it.
     * The state below it is also checked for transparency and the process repeats itself.
     *
     * All of the states can process user input (event) on their own.
     */
    void handleEvent(const sf::Event& event);

    // ==== Operations typical for a stack ==== //
    // ==== Designed for states to use ==== //

    /**
     * \brief Pushes a state with a given identifier onto the stack
     * \param stateID The identifier to which the assigned state will be pushed onto the stack.
     *
     * Previously, the StateStack must have saved the specified identifier via the saveState
     * function. This function just uses the identifier to create an object of the given state class
     * using a factory.
     */
    void push(State_ID stateID);


    /**
     * \brief Removes the state at the top of the stack.
     *
     * Executing this function on an empty stack leads to unpredictable behaviour.
     */
    void pop();


    /**
     * \brief Clears the stack by removing all the states in it.
     */
    void clear();


    /**
     * \brief Checks that there are no states on the stack.
     * \return True if the stack is empty, false if there is a state on it.
     */
    [[nodiscard]] bool empty() const noexcept;

private:
    /**
     * \brief The queue that holds the operations to be performed on the stack.
     *
     * A queue is used so that individual operations are not performed immediately.
     * You can imagine a state that wants to delete itself and then push out another state.
     * It cannot do this if it does not have this queue, because it would not have time
     * to perform any operations after removing itself from the stack.
     *
     * The queue executes and clears at the beginning of each new iteration.
     */
    struct Change
    {
        Perform operation;//!< Operation to be performed on the stack
        State_ID stateID; //!< Identifier of the state that should be pushed on the stack
    };


    /**
     * \brief Creates a state with the given identifier
     * \param stateID Identifier previously assigned to a specific state
     * \return A pointer to a newly created state from the factory.
     *
     * It checks if a state with the given identifier has been saved,
     * then uses the factory to generate it and returns a pointer to the newly created object.
     */
    auto createState(const State_ID& stateID);

    /**
     * \brief Performs operations on the stack, that are waiting in the queue.
     *
     * Operations are performed in FIFO order. When all queued operations
     * have completed, the queue is empty.
     */
    void applyChanges();

    /**
     * \brief The stack on which the current states are located. It holds the states.
     *
     * Its implementation is a vector, since the implementation of
     * "transparent" states requires the ability to iterate over states.
     */
    std::vector<std::unique_ptr<State>> mStack;

    /**
     * \brief A FIFO queue that holds pending operations for execution on the stack.
     */
    std::vector<Change> mChangesQueue;


    /**
     * \brief A factory that creates states objects with the given identifiers.
     */
    std::map<State_ID, std::function<std::unique_ptr<State>()>> mFactory;
};

template<typename State, typename... Args>
void StateStack::saveState(State_ID stateID, Args&&... args)
{
    // args holds all the variables and data that state
    // might need to use
    mFactory[stateID] = [&args..., this]()
    {
        return std::unique_ptr<State>(std::make_unique<State>(*this, std::forward<Args>(args)...));
    };
}

#endif
