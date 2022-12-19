#include "Healthbar.h"
#include "pch.h"


Healthbar::Healthbar(const TextureManager& textureManager, sf::Vector2f position)
    : DiscreteBar(textureManager.getResourceReference(TextureManagerId::GUI_Healthbar_EmptyHeart),
                  textureManager.getResourceReference(TextureManagerId::GUI_Healthbar_HalfHeart),
                  textureManager.getResourceReference(TextureManagerId::GUI_Healthbar_FullHeart),
                  position)
{
}

void Healthbar::hearts(Hearts hearts)
{
    DiscreteBar::value(hearts);
}
