#pragma once
#include "Player/GUI/ItemSlots.h"
#include "Resources/Resources.h"
#include "Utils/Serializer.h"
#include "World/Item/Item.h"
#include <optional>

class Hotbar;
class Crafting;

/**
 * @brief The equipment in which the player stores belongings
 */
class Inventory
{
public:
    static constexpr auto INVENTORY_SCALE = 3.f;
    static constexpr auto NUMBER_OF_COLUMNS = 9;
    static constexpr auto NUMBER_OF_ROWS = 4;
    static constexpr auto NUMBER_OF_SLOTS = NUMBER_OF_ROWS * NUMBER_OF_COLUMNS;

    Inventory(sf::RenderWindow& gameWindow, const GameResources& gameResources,
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

    /**
     * @brief Displays/hides the inventory on the screen and activates/deactivates all related
     * logic.
     */
    void toggleInventory();

    /**
     * @brief Updates the status of the displayed inventory
     */
    void updateInventoryState();

private:
    /**
     * @brief Finds a slot with a given item that could hold the indicated number of items.
     * @param itemId The item to look for
     * @param amount The amount of stuff to try to put in
     * @return ItemSlot that enables this, or nullptr if there is none
     */
    std::shared_ptr<ItemSlot> findExistingItemSlotThatMayTakeGivenAmount(const ItemId& itemId,
                                                                         int amount);

    /**
     * @brief In the first empty slot, it tries to insert an item into ItemSlot
     * @param amount The amount of the item that is trying to be inserted
     * @param itemId Identifier of the item it is trying to insert
     * @return True if successful, false otherwise
     */
    bool tryEmplaceFirstEmptySlot(int amount, const ItemId& itemId);

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

    /**
     * @brief Returns the positions on the screen of the upper left corner of the indicated slot.
     * @param index Slot whose position is to be returned
     * @return Position on the screen of the upper left corner of the indicated slot.
     */
    sf::Vector2f positionOfGivenSlot(int index);

    /**
     * @brief Draws inventory to the screen
     * @param target where it should be drawn to
     * @param states provides information about rendering process (transform, shader, blend mode)
     */
    void drawInventory(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * @brief Configures textures, position and general setting of the background of the inventory
     * @param gameResources Game resources including fontmanager and texturemanager.
     * @param windowSize Dimensions of the window on which the background of the inventory is drawn
     */
    void setupInventoryBackground(const GameResources& gameResources,
                                  const sf::Vector2<int>& windowSize);

    /**
     * @brief Sets inventory slots such as their location or scale
     * @param gameResources Game resources that store data such as fonts and textures
     */
    void setupInventorySlots(const GameResources& gameResources);

    /**
     * @brief Prepares settings for crafting displayed in the inventory.
     * @param gameWindow The game window in which the game is displayed.
     * @param gameResources Game resources that store data such as fonts and textures
     */
    void setupCrafting(sf::RenderWindow& gameWindow, const GameResources& gameResources);

private:
    sf::Sprite mInventoryBackground;
    ItemSlots mItems;
    ItemSlots mCombinedSlots;
    std::unique_ptr<Hotbar> mHotbar;
    const std::string& mSaveWorldFilePath;
    const TextureManager& mTextureManager;
    sf::RenderWindow& mGameWindow;
    Serializer mSerializer;
    std::unique_ptr<Crafting> mCrafting;

    bool mIsInventoryOpened = false;
};
