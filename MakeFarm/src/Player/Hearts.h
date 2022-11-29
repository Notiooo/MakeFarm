#pragma once


/**
 * @brief Hearts that are a representation of life in the game. Half hearts can occur, so for
 * example, they can be numbers: 0.0, 0.5, 1.0, 1.5 ... where hearts are not negative!
 */
class Hearts
{
public:
    /**
     *
     * @brief Number of hearts including half hearts, so for example 0.0, 0.5, 1.0, 1.5 ...
     */
    using HeartsNumber = float;

    Hearts(const HeartsNumber& heartsNumber);

    /**
     * @brief It checks whether half a heart has occurred, so for example, there are 4.5 hearts.
     * @return True if there is a half-hearted occurrence, false otherwise
     */
    [[nodiscard]] bool containsHalfOfHeart() const;

    /**
     * @brief Checks if there is even half a heart or more
     * @return True if there is at least half a heart or more, false otherwise.
     */
    [[nodiscard]] bool atLeastHalfHeart() const;

    Hearts& operator+=(const HeartsNumber& heartsNumber);
    Hearts& operator-=(const HeartsNumber& heartsNumber);
    Hearts& operator=(const HeartsNumber& heartsNumber);
    operator float() const;

private:
    /**
     * @brief Converts floating point values to hearts, that is, for example, to
     * 0.5, 1.0, 1.5, 2.0, 2.5 ....
     * @param heartsNumber Number to convert to hearts
     * @return Number of hearts, including half of heart, so for example 0.5, 1.0, 1.5 ...
     */
    static HeartsNumber toHearts(float heartsNumber);

    HeartsNumber mHearts;
};