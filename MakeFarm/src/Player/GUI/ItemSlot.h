#pragma once
#include "Resources/Resources.h"
#include "World/Item/Item.h"
#include "optional"

/**
 * @brief Representation of a slot with item. For example as part of inventory.
 */
class ItemSlot
{
public:
    static constexpr auto TEXTURE_OFFSET_X = 1.f;
    static constexpr auto TEXTURE_OFFSET_Y = 1.f;

    /**
     * @brief Determines the size of the entire item slot
     * @param textureManager Manager containing game textures
     * @param scale The scale at which the slot will be displayed
     * @return Dimensions of the entire item slot
     */
    static sf::Vector2f dimensions(const TextureManager& textureManager, float scale = 1.f);

    explicit ItemSlot(const GameResources& gameResources);
    ItemSlot(const ItemSlot& rhs) = default;

    /**
     * @brief Swaps item and amount with another slot
     * @param rhs Another slot with which to swap item and amount
     */
    void swap(ItemSlot& rhs);

    /**
     * @brief Returns the number of items in a given slot
     * @return Amount of items in a given slot
     */
    int amount() const;

    /**
     * @brief Changes the number of items in the slot.
     * @param amount The number into which the number of items is to be changed.
     * @return The amount of the item that did not fit into the slot
     */
    int amount(int amount);

    /**
     * @brief Returns the item that is in the slot.
     * @return Item that is in the slot. The value is empty if there is no object in the slot.
     */
    std::optional<Item> item();

    /**
     * @brief Assigns a new itemId to a slot. If an itemId currently assigned is overwritten.
     * @param itemId Identifier of the itemId that should be assigned.
     */
    void item(ItemId itemId);

    /**
     * @brief Removes the currently assigned item from the slot.
     */
    void removeItem();

    /**
     * @brief It checks whether the slot contains the indicated point.
     * @tparam T The type of which are two coordinates of the point
     * @param point A point on the 2D plane.
     * @return True if the slot contains the given point, false otherwise.
     */
    template<typename T>
    bool doesContainPoint(const sf::Vector2<T>& point) const;

    /**
     * @brief Sets the position of the item sprite inside slot without affecting the slot background
     * position.
     * @param position The new position on which the item is to be placed.
     */
    void itemPosition(const sf::Vector2f& position);

    /**
     * @brief Sets the sprite of the item back to the slot location.
     */
    void moveBackItemToSlot();

    /**
     * @brief Sets the new position of the entire slot.
     * @param position New slot position
     */
    void position(const sf::Vector2f& position);

    /**
     * @brief Returns the current position of the slot.
     * @return Current position of the slot. If the item is displayed in a different location than
     * the slot, it returns the background position of the slot.
     */
    sf::Vector2f position() const;

    /**
     * @brief Sets the size of the text displayed in the slot.
     * @param characterSize Letter size of the text
     */
    void textSize(int characterSize);

    /**
     * @brief Sets the scale at which slots are drawn
     * @param scale Scale at which slots are drawn
     */
    void scale(float scale);

    /**
     * @brief Returns the current dimensions of the slot
     * @return Current dimensions of the slot
     */
    sf::Vector2f dimensions();

    /**
     * @brief It checks whether the slot contains any item.
     * @return True if it contains any item, false otherwise.
     */
    bool doesContainItem() const;

    /**
     * \brief Draws the inventory and hotbar to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    /**
     * @brief Sets the sprite that is the background of the slot
     * @param textureManager Manager containing game textures
     */
    void setupBackgroundSprite(const TextureManager& textureManager);

    /**
     * @brief Sets the text displaying the number of items in the slot.
     * @param fontManager Manager containing game fonts
     */
    void setupItemAmountText(const FontManager& fontManager);

    /**
     * @brief Updates the item sprite to the currently set item in the slot.
     */
    void updateItemSprite();

    /**
     * @brief Sets the object and object quantity of this slot to the indicated slot.
     * @param rhs Slot to which the data is to be written.
     */
    void assignAmountAndItemToGivenItemSlot(ItemSlot& rhs);

private:
    const GameResources& mGameResources;
    const TexturePack& mTexturePack;
    int mAmount;
    std::optional<Item> mItem;

    sf::Sprite mBackground;
    sf::Sprite mItemSprite;
    sf::Text mItemAmountText;
    int mCharacterSize = 7;
    float mScale = 1.f;
    sf::Vector2f mPosition;
};