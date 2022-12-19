#pragma once

#include "Player/GUI/Bars/DiscreteBar.h"
#include "Player/GUI/Bars/DiscreteBarValue.h"
#include "Resources/Resources.h"

/**
 * @brief This is an airbar/oxygenbar that displays when the player is underwater. It indicates the
 * current oxygen level.
 */
class Oxygenbar : public DiscreteBar
{
public:
    Oxygenbar(const TextureManager& textureManager, sf::Vector2f position);

    /**
     * @brief The oxygen level bar contains bubble icons, which level corresponds to the amount of
     * oxygen.
     */
    using Bubbles = DiscreteBarValue;

    /**
     * @brief Sets the number of value on the bar
     * @param bubbles DiscreteBarValue given in half-bubble accuracy.
     */
    void oxygen(Bubbles bubbles);
};