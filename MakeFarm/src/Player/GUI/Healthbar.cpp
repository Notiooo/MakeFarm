#include "Healthbar.h"
#include "Renderer3D/Renderer3D.h"
#include "pch.h"

Healthbar::Healthbar(const TextureManager& textureManager, sf::Vector2f position)
    : mEmptyHeart(textureManager.getResourceReference(TextureManagerId::GUI_Healthbar_EmptyHeart))
    , mHalfHeart(textureManager.getResourceReference(TextureManagerId::GUI_Healthbar_HalfHeart))
    , mFullHeart(textureManager.getResourceReference(TextureManagerId::GUI_Healthbar_FullHeart))
    , mRenderPosition(position)
{
    setupHeartSprites();
}

void Healthbar::setupHeartSprites()
{
    int heartNumber = 0;
    for (auto& heartSprite: mHeartSprites)
    {
        setupHeartSprite(heartNumber, heartSprite);
        ++heartNumber;
    }
}

void Healthbar::setupHeartSprite(int heartNumber, sf::Sprite& heartSprite) const
{
    heartSprite.setTexture(mFullHeart);
    heartSprite.setScale(HEALTHBAR_SCALE, HEALTHBAR_SCALE);
    setupHeartOpacity(heartSprite);
    setupHeartPosition(heartNumber, heartSprite);
}

void Healthbar::setupHeartOpacity(sf::Sprite& heartSprite) const
{
    auto currentSpriteColor = heartSprite.getColor();
    currentSpriteColor.a = 200;
    heartSprite.setColor(currentSpriteColor);
}

void Healthbar::setupHeartPosition(int heartNumber, sf::Sprite& heartSprite) const
{
    const auto& heartLeftPadding = heartNumber * GAP_BETWEEN_HEARTS * HEALTHBAR_SCALE;
    const auto& heartPositionX = heartNumber * mFullHeart.getSize().x * HEALTHBAR_SCALE;
    const auto& heartSizeY = mFullHeart.getSize().y * HEALTHBAR_SCALE;
    const auto& offsetY = OFFSET_Y * HEALTHBAR_SCALE;
    heartSprite.setPosition(mRenderPosition.x + heartLeftPadding + heartPositionX,
                            mRenderPosition.y - heartSizeY + offsetY);
}

void Healthbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& heart: mHeartSprites)
    {
        SfmlDraw(heart, target, states);
    }
}

void Healthbar::update(const float& deltaTime)
{
}

void Healthbar::hearts(Hearts hearts)
{
    hearts = ((hearts > MAX_HEARTS) ? Hearts(MAX_HEARTS) : hearts);

    if (hearts == 0)
    {
        assignTextureHeartInRange(0, MAX_HEARTS, mEmptyHeart);
        return;
    }

    auto finalHeartIndex = std::ceil(hearts) - 1;
    assignTextureOfHalfOrFullHeart(hearts, finalHeartIndex);
    assignTextureHeartInRange(0, finalHeartIndex, mFullHeart);
    assignTextureHeartInRange(finalHeartIndex + 1, MAX_HEARTS, mEmptyHeart);
}

void Healthbar::assignTextureOfHalfOrFullHeart(const Hearts& hearts, int heartIndex)
{
    if (hearts.containsHalfOfHeart())
    {
        mHeartSprites.at(heartIndex).setTexture(mHalfHeart);
    }
    else
    {
        mHeartSprites.at(heartIndex).setTexture(mFullHeart);
    }
}

void Healthbar::assignTextureHeartInRange(int start, int end, const sf::Texture& heartTexture)
{
    assert(&heartTexture == &mFullHeart || &heartTexture == &mEmptyHeart);
    for (auto i = start; i < end; ++i)
    {
        mHeartSprites.at(i).setTexture(heartTexture);
    }
}
