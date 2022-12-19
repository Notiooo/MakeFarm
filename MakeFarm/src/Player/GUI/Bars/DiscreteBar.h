#pragma once
#include "Player/GUI/Bars/DiscreteBarValue.h"

/**
 * @brief This is the base class for creating interface bars representing different player states
 * like life, oxygen level, armor level, hunger level and the like.
 */
class DiscreteBar
{
public:
    DiscreteBar(const sf::Texture& emptyIcon, const sf::Texture& halfIcon,
                const sf::Texture& fullIcon, sf::Vector2f position);

    static constexpr auto DISCRETEBAR_SCALE = 2.f;
    static constexpr auto GAP_BETWEEN_ICONS = 2.f;
    static constexpr auto OFFSET_Y = -3.f;

    /**
     * @brief Maximum number of icons on the bar.
     */
    static constexpr auto MAX_VALUES = 10;

    /**
     * \brief Draws the discretebar to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * \brief Updates the discretebar logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * @brief Returns the size of the DiscreteBar.
     * @return Size of the DiscreteBar.
     */
    sf::Vector2f size() const;

    /**
     * @brief Sets a new position for the DiscreteBar.
     * @param newPosition New position for the DiscreteBar.
     */
    void position(const sf::Vector2f& newPosition);

protected:
    /**
     * @brief Sets the number of value on the bar
     * @param discreteValue DiscreteBarValue given in half-value accuracy.
     */
    void value(DiscreteBarValue discreteValue);

private:
    /**
     * @brief Assigns the given texture in a given range.
     * @param start The first index of the icon, which will be assigned a texture.
     * @param end The first index of the icon to which the texture will not be assigned.
     * @param iconTexture The texture of the icon, which will be assigned in the specified range.
     */
    void assignTextureInGivenRange(int start, int end, const sf::Texture& iconTexture);

    /**
     * @brief Assigns full or half icon depending on whether a half is present in the value
     * @param value Number of value
     * @param iconIndex Icon index which will be a full, or half icon
     */
    void assignTextureOfHalfOrFullValue(const DiscreteBarValue& value, int iconIndex);

    /**
     * @brief Prepares the icon sprite for use by setting the most important values like opacity or
     * position.
     * @param iconIndex The number/index of sprite to configure
     * @param iconSprite Sprite of value to configure
     */
    void setupIconSprite(int iconIndex, sf::Sprite& iconSprite) const;

    /**
     * @brief Configures icon sprite settings, such as position and opacity.
     */
    void setupIconSprites();

    /**
     * @brief Configures the position of the indicated icon.
     * @param iconIndex Number/index of icon to set its position.
     * @param iconSprite Sprite of the icon, whose position is set.
     */
    void setupPosition(int iconIndex, sf::Sprite& iconSprite) const;

    /**
     * @brief Configures the transparency of the indicated icon
     * @param iconSprite Sprite value to configure
     */
    void setupSpriteOpacity(sf::Sprite& iconSprite) const;


    std::array<sf::Sprite, MAX_VALUES> mSprites;
    sf::Vector2f mRenderPosition;

    const sf::Texture& mEmptyIcon;
    const sf::Texture& mHalfIcon;
    const sf::Texture& mFullIcon;
};