#pragma once
#include "Player/GUI/InventorySlot.h"
#include "World/Item/Item.h"
#include <Resources/Resources.h>
#include <optional>

class Hotbar;

/**
 * @brief The equipment in which the player stores belongings
 */
class Inventory
{
public:
    static constexpr auto NUMBER_OF_COLUMNS = 9;
    static constexpr auto NUMBER_OF_ROWS = 4;

    Inventory(sf::Vector2i windowSize, const GameResources& gameResources);
    ~Inventory();

    /**
     * \brief Updates the inventory logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief Draws the inventory and hotbar to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * @brief Attempts to add an item with the specified amount to the inventory, returns true if
     * successful and false if unsuccessful.
     * @param itemToInsert Item to be inserted.
     * @param amount Number of items that should be added
     * @return Returns true if the object was successfully inserted, false otherwise
     * @warning In the future, it may come in handy to have functionality that stuffs your inventory
     * to the brim and returns the number of items you couldn't stuff.
     */
    bool tryAddItem(const ItemId& itemToInsert, int amount = 1);

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief Returns hotbar
     * @return hotbar
     */
    [[nodiscard]] Hotbar& hotbar();

    /**
     * @brief Returns hotbar
     * @return hotbar
     */
    [[nodiscard]] const Hotbar& hotbar() const;

    /**
     * @brief Finds a slot with a given item that could hold the indicated number of items.
     * @param item The item to look for
     * @param amount The amount of stuff to try to put in
     * @return InventorySlot that enables this, or nullptr if there is none
     */
    InventorySlot* const findExistingItemSlotThatMayTakeGivenAmount(const ItemId& item, int amount);

    /**
     * @brief In the first empty slot, it tries to create an InventorySlot
     * @tparam Ts The type of arguments to be passed on
     * @param args Arguments to be passed on
     * @return True if successful, false otherwise
     */
    template<typename... Ts>
    bool tryEmplaceFirstEmptySlot(Ts&&... args);

private:
    std::array<std::optional<InventorySlot>, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS> mItems;
    std::unique_ptr<Hotbar> mHotbar;
    const TexturePack& mTexturePack;
};
