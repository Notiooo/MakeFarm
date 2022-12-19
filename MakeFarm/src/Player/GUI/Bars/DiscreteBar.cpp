#include "DiscreteBar.h"
#include "Renderer3D/Renderer3D.h"
#include "Resources/Resources.h"
#include "pch.h"

DiscreteBar::DiscreteBar(const sf::Texture& emptyIcon, const sf::Texture& halfIcon,
                         const sf::Texture& fullIcon, sf::Vector2f position)
    : mEmptyIcon(emptyIcon)
    , mHalfIcon(halfIcon)
    , mFullIcon(fullIcon)
    , mRenderPosition(position)
{
    setupIconSprites();
}

void DiscreteBar::setupIconSprites()
{
    int heartNumber = 0;
    for (auto& heartSprite: mSprites)
    {
        setupIconSprite(heartNumber, heartSprite);
        ++heartNumber;
    }
}

void DiscreteBar::setupIconSprite(int iconIndex, sf::Sprite& iconSprite) const
{
    iconSprite.setTexture(mFullIcon);
    iconSprite.setScale(DISCRETEBAR_SCALE, DISCRETEBAR_SCALE);
    setupSpriteOpacity(iconSprite);
    setupPosition(iconIndex, iconSprite);
}

void DiscreteBar::setupSpriteOpacity(sf::Sprite& iconSprite) const
{
    auto currentSpriteColor = iconSprite.getColor();
    currentSpriteColor.a = 200;
    iconSprite.setColor(currentSpriteColor);
}

void DiscreteBar::setupPosition(int iconIndex, sf::Sprite& iconSprite) const
{
    const auto& heartLeftPadding = iconIndex * GAP_BETWEEN_ICONS * DISCRETEBAR_SCALE;
    const auto& heartPositionX = iconIndex * mFullIcon.getSize().x * DISCRETEBAR_SCALE;
    const auto& heartSizeY = mFullIcon.getSize().y * DISCRETEBAR_SCALE;
    const auto& offsetY = OFFSET_Y * DISCRETEBAR_SCALE;
    iconSprite.setPosition(mRenderPosition.x + heartLeftPadding + heartPositionX,
                           mRenderPosition.y - heartSizeY + offsetY);
}

void DiscreteBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& sprite: mSprites)
    {
        SfmlDraw(sprite, target, states);
    }
}

void DiscreteBar::update(const float& deltaTime)
{
}

void DiscreteBar::value(DiscreteBarValue discreteValue)
{
    discreteValue = ((discreteValue > MAX_VALUES) ? DiscreteBarValue(MAX_VALUES) : discreteValue);

    if (discreteValue == 0)
    {
        assignTextureInGivenRange(0, MAX_VALUES, mEmptyIcon);
        return;
    }

    auto finalHeartIndex = ceil(discreteValue) - 1;
    assignTextureOfHalfOrFullValue(discreteValue, finalHeartIndex);
    assignTextureInGivenRange(0, finalHeartIndex, mFullIcon);
    assignTextureInGivenRange(finalHeartIndex + 1, MAX_VALUES, mEmptyIcon);
}

void DiscreteBar::assignTextureOfHalfOrFullValue(const DiscreteBarValue& value, int iconIndex)
{
    if (value.containsHalf())
    {
        mSprites.at(iconIndex).setTexture(mHalfIcon);
    }
    else
    {
        mSprites.at(iconIndex).setTexture(mFullIcon);
    }
}

void DiscreteBar::assignTextureInGivenRange(int start, int end, const sf::Texture& iconTexture)
{
    assert(&iconTexture == &mFullIcon || &iconTexture == &mEmptyIcon);
    for (auto i = start; i < end; ++i)
    {
        mSprites.at(i).setTexture(iconTexture);
    }
}

sf::Vector2f DiscreteBar::size() const
{
    return sf::Vector2f{static_cast<float>(mFullIcon.getSize().x * MAX_VALUES),
                        static_cast<float>(mFullIcon.getSize().y)} *
           DISCRETEBAR_SCALE;
}

void DiscreteBar::position(const sf::Vector2f& newPosition)
{
    mRenderPosition = newPosition;

    int spriteNumber = 0;
    for (auto& sprite: mSprites)
    {
        setupPosition(spriteNumber, sprite);
        ++spriteNumber;
    }
}