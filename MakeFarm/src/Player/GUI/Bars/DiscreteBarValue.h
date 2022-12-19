#pragma once


/**
 * @brief Values on the bar that are a representation of life/food/oxygen etc in the game. Half
 * values can occur, so for example, they can be numbers: 0.0, 0.5, 1.0, 1.5 ... where values are
 * not negative!
 */
class DiscreteBarValue
{
public:
    /**
     * @brief A discrete integer value with optional half value, so for example 0.0, 0.5, 1.0, 1.5
     */
    using DiscreteNumber = float;

    DiscreteBarValue(const DiscreteNumber& value);

    /**
     * @brief It checks whether half of value has occurred, so for example, there is 4.5.
     * @return True if there is a half-value occurrence, false otherwise
     */
    [[nodiscard]] bool containsHalf() const;

    /**
     * @brief Checks if there is at least half a discrete value or more
     * @return True if there is at least half a discrete value or more, false otherwise.
     */
    [[nodiscard]] bool atLeastHalf() const;

    DiscreteBarValue& operator+=(const DiscreteNumber& value);
    DiscreteBarValue& operator-=(const DiscreteNumber& value);
    DiscreteBarValue& operator=(const DiscreteNumber& value);
    operator float() const;

private:
    /**
     * @brief Converts floating point values to discrete values, that is, for example, to
     * 0.5, 1.0, 1.5, 2.0, 2.5 ....
     * @param value Number to convert to discrete values
     * @return Number of discrete values, that is integer value, with optional a half values, so for
     * example 0.5, 1.0, 1.5 ...
     */
    static DiscreteNumber toDiscrete(float value);

    DiscreteNumber mDiscreteNumber;
};