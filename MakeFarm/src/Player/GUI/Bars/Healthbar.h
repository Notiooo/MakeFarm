#pragma once
#include "Player/GUI/Bars/DiscreteBar.h"
#include "Player/GUI/Bars/DiscreteBarValue.h"
#include "Resources/Resources.h"

/**
 * @brief This is the player's health bar, which shows the player how much life he currently has and
 * how much player is short of potential death.
 */
class Healthbar : public DiscreteBar
{
public:
    Healthbar(const TextureManager& textureManager, sf::Vector2f position);

    /**
     * @brief The oxygen level bar contains hearts icons, which level corresponds to the amount of
     * health.
     */
    using Hearts = DiscreteBarValue;

    /**
     * @brief Sets the number of value on the bar
     * @param hearts DiscreteBarValue given in half-hearted accuracy.
     */
    void hearts(Hearts hearts);
};
