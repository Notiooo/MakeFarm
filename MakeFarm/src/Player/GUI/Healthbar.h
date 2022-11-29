#pragma once
#include "Player/Hearts.h"
#include "Resources/Resources.h"

class Healthbar
{
public:
    Healthbar(const TextureManager& textureManager, sf::Vector2f position);

    static constexpr auto HEALTHBAR_SCALE = 2.f;
    static constexpr auto GAP_BETWEEN_HEARTS = 2.f;
    static constexpr auto OFFSET_Y = -3.f;

    static constexpr auto MAX_HEARTS = 10;

    /**
     * \brief Draws the healthbar to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * \brief Updates the healthbar logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * @brief Sets the number of hearts on the bar
     * @param hearts Hearts given in half-hearted accuracy.
     */
    void hearts(Hearts hearts);

private:
    /**
     * @brief Assigns the given texture in a given range.
     * @param start The first index of the heart, which will be assigned a texture.
     * @param end The first index of the heart to which the texture will not be assigned.
     * @param heartTexture The texture of the heart, which will be assigned in the specified range.
     */
    void assignTextureHeartInRange(int start, int end, const sf::Texture& heartTexture);

    /**
     * @brief Assigns full or half heart depending on whether half heart is present
     * @param hearts Number of hearts
     * @param heartIndex Heart index which will be a full, or half heart
     */
    void assignTextureOfHalfOrFullHeart(const Hearts& hearts, int heartIndex);

    /**
     * @brief Prepares the heart sprite for use by setting the most important values like opacity or
     * position.
     * @param heartNumber The number of hearts to configure
     * @param heartSprite Sprite of hearts to configure
     */
    void setupHeartSprite(int heartNumber, sf::Sprite& heartSprite) const;

    /**
     * @brief Configures heart sprite settings, such as position and opacity.
     */
    void setupHeartSprites();

    /**
     * @brief Configures the position of the indicated heart.
     * @param heartNumber Heart number to set its position.
     * @param heartSprite Sprite of the heart, whose position is set.
     */
    void setupHeartPosition(int heartNumber, sf::Sprite& heartSprite) const;

    /**
     * @brief Configures the transparency of the indicated heart
     * @param heartSprite Sprite hearts to configure
     */
    void setupHeartOpacity(sf::Sprite& heartSprite) const;


    std::array<sf::Sprite, MAX_HEARTS> mHeartSprites;
    sf::Vector2f mRenderPosition;

    const sf::Texture& mEmptyHeart;
    const sf::Texture& mHalfHeart;
    const sf::Texture& mFullHeart;
};