#include "Oxygenbar.h"
#include "pch.h"

Oxygenbar::Oxygenbar(const TextureManager& textureManager, sf::Vector2f position)
    : DiscreteBar(textureManager.getResourceReference(TextureManagerId::GUI_Oxygenbar_EmptyOxygen),
                  textureManager.getResourceReference(TextureManagerId::GUI_Oxygenbar_HalfOxygen),
                  textureManager.getResourceReference(TextureManagerId::GUI_Oxygenbar_FullOxygen),
                  position)
{
}
void Oxygenbar::oxygen(Bubbles bubbles)
{
    value(bubbles);
}