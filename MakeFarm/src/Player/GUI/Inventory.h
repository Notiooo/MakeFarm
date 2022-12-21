#pragma once
#include "Player/GUI/InventorySlot.h"
#include "Resources/Resources.h"
#include "Utils/Serializer.h"
#include "World/Item/Item.h"
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

    Inventory(sf::Vector2i windowSize, const GameResources& gameResources,
              const std::string& saveWorldPath);
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
     * @brief Cleans the contents of the inventory.
     */
    void clear();

private:
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

    /**
     * @brief File path to which the inventory is saved
     * @return The path to the file in the form of a string
     */
    std::string inventorySaveFilePath();

    /**
     * @brief This is another form of recording the status of an inventory slot, which is easier to
     * serialize and save to a file.
     */
    struct SerializableInventorySlot
    {
        template<typename Archive, typename Self>
        static void serialize(Archive& archive, Self& self)
        {
            archive(self.amount, self.item);
        }

        int amount = 0;
        ItemId item;
    };

    using SerializableInventory =
        std::array<SerializableInventorySlot, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS>;

    /**
     * @brief It prepares a version of the inventory that can be serialized and saved to a file.
     * @return A form of inventory storage that can be serialized.
     */
    SerializableInventory serializableInventory();

    /**
     * @brief Overwrites the current inventory with the transferred serializable inventory.
     * @param inventory Serialized inventory, which should overwrite the current inventory.
     */
    void overwriteInventory(const SerializableInventory& inventory);

    /**
     * @brief Saves the state of the inventory data to a file
     */
    void saveInventoryDataToFile();

    /**
     * @brief Reads the saved state of the inventory from the file
     * @warning Nothing happens when the save file is not there
     */
    void loadSavedInventoryData();

private:
    std::array<std::optional<InventorySlot>, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS> mItems;
    std::unique_ptr<Hotbar> mHotbar;
    const std::string& mSaveWorldFilePath;
    const TexturePack& mTexturePack;
    Serializer mSerializer;
};
