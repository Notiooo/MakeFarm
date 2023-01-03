#pragma once

#include "ItemId.h"

class ItemType;
class TexturePack;

/**
 * @brief An item is a thing in the game that a player can own, use, drop, or place.
 */
class Item
{
public:
    Item(const ItemId& itemId);

    /**
     * \brief Texture ID that corresponds to the texture by the position it is in the file.
     *
     * If you expand the texture file linearly and size from left to
     * right starting from zero, the position of the texture will
     * correspond to its id.
     *
     * An example:
     * Each '#' is an example texture
     *
     * #####
     * #####
     * #####
     *    ^
     *	This texture has an id of 14
     *
     */
    using TextureId = unsigned int;

    enum class Render
    {
        Block = 0,
        Item = 1
    };

    /**
     * @brief Returns information about whether the item represents a block.
     * @return True if this item represents a block, otherwise false.
     */
    [[nodiscard]] bool isBlock() const;

    /**
     * @brief Sets the type of object, changing its meaning
     * @param itemId New item identifier
     */
    void setItemType(const ItemId& itemId);

    /**
     * Returns a item id
     * @return Identifier of the item
     */
    [[nodiscard]] ItemId id() const;

    /**
     * Returns a item name
     * @return Name of the item
     */
    [[nodiscard]] std::string name() const;

    /**
     * @brief Returns maximum amount of an item that can fit in one inventory slot
     * @return Maximum amount of an item that can fit in one inventory slot.
     */
    [[nodiscard]] int maximumAmountInSlot() const;

    /**
     * @brief Returns the type of rendering that should be considered when rendering this object.
     * @return The type of rendering that should be considered when rendering this object.
     */
    [[nodiscard]] Render renderType() const;

    /**
     * @brief Returns the texture identifier
     * @return Texture identifier
     */
    [[nodiscard]] unsigned int textureId() const;


private:
    const ItemType* mItemType;
};
