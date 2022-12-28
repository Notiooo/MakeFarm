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
    static constexpr auto INVENTORY_SCALE = 3.f;
    static constexpr auto NUMBER_OF_COLUMNS = 9;
    static constexpr auto NUMBER_OF_ROWS = 4;
    static constexpr auto ITEM_OFFSET = 1.f * INVENTORY_SCALE;
    static constexpr auto ITEM_AMOUNT_OFFSET_X = 0 * INVENTORY_SCALE;
    static constexpr auto ITEM_AMOUNT_OFFSET_Y = 0 * INVENTORY_SCALE;

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

    /**
     * @brief Returns the positions on the screen of the upper left corner of the indicated slot.
     * @param index Slot whose position is to be returned
     * @return Position on the screen of the upper left corner of the indicated slot.
     */
    sf::Vector2f positionOfGivenSlot(int index);

    /**
     * @brief Creates an item sprite on the indicated itemInSlot.
     * @param slotNumber Index of the itemInSlot on which the sprite is to be created
     * @param itemInSlot Item that is in the indicated slot
     * @param slotPosition The position of the upper-left corner of the screen slot.
     */
    void createSpriteAtGivenSlot(int slotNumber, Item& itemInSlot, sf::Vector2f slotPosition);

    /**
     * @brief Creates an amount of item text on the indicated itemInSlot.
     * @param slotNumber Index of the itemInSlot on which the text is to be created
     * @param amount Quantity of the item on the indicated slot
     * @param slotPosition The position of the upper-left corner of the screen slot.
     */
    void createItemAmountAtGivenSlot(int slotNumber, int amount, sf::Vector2f slotPosition);

    /**
     * @brief It sets the slot sprites, that is, prepares the background for each slot and sets the
     * text font for the number of items.
     * @param gameResources Game resources including fontmanager and texturemanager.
     */
    void setupSpritesOfSlots(const GameResources& gameResources);

    /**
     * @brief Returns the index of the slot at the indicated screen position. If there is no slot at
     * the indicated position then returns an empty optional value.
     * @param position Position of the point on the screen
     * @return index of the slot at the indicated screen position. If there is no slot at the
     * indicated position then returns an empty optional value.
     */
    std::optional<int> indexOfSlotAtPosition(sf::Vector2i position);

    /**
     * @brief Returns information about whether the mouse is holding an inventory item.
     * @return True if the mouse holds the item in the inventory, false otherwise
     */
    bool doesMouseHoldAnItem() const;

    /**
     * @brief Item in the indicated slot is caught by the mouse
     * @param slotIndex Slot from which the item should be grabbed
     */
    void catchItemAtGivenSlotByMouse(const std::optional<int>& slotIndex);

    /**
     * @brief Draws an object held by a mouse as long as any object is held by a mouse.
     * @param target where it should be drawn to
     * @param states provides information about rendering process (transform, shader, blend mode)
     */
    void tryDrawItemHoldingByMouse(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * @brief Draws all slots in the inventory onto the screen
     * @param target where it should be drawn to
     * @param states provides information about rendering process (transform, shader, blend mode)
     */
    void drawAllSlots(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * @brief Draws inventory to the screen
     * @param target where it should be drawn to
     * @param states provides information about rendering process (transform, shader, blend mode)
     */
    void drawInventory(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * @brief The object currently held by the mouse tries to be dropped on the indicated slot
     * @param slotIndex Index of the slot on which the item is to be dropped
     */
    void dropItemHoldingByMouseOntoTheGivenSlot(const int& slotIndex);

    /**
     * @brief Mouse stops holding the currently held object
     */
    void clearItemHoldingByMouse();

    /**
     * @brief Handles left mouse button click event
     * @param event user input
     */
    void handleLeftMouseClick(const sf::Event& event);

    /**
     * @brief Determines the behavior that happens when holding the mouse of an object is clicked on
     * a slot with the indicated index
     * @param clickedSlotIndex Index of the slot in which the mouse clicked
     */
    void clickOnSlotWhileMouseHoldingItem(int clickedSlotIndex);

    /**
     * @brief A set of sprites that is a representation of one inventory slot
     */
    struct InventorySlotSprite
    {
        sf::Sprite background;
        sf::Sprite item;
        sf::Text itemAmountText;
    };

    /**
     * @brief Configures the slotSprite that is the background of the slot
     * @param textureManager Manager storing game textures
     * @param slotIndex Index of the slot in which the sprite should be created
     * @param slotSprite A set of sprites representing a slot in the inventory
     */
    void setupBackgroundSprite(const TextureManager& textureManager, int slotIndex,
                               InventorySlotSprite& slotSprite);

    /**
     * @brief Configures text that displays the number of items in a slot
     * @param fontManager Manager storing game fonts
     * @param slotSprite A set of sprites representing a slot in the inventory
     */
    void setupItemAmountText(const FontManager& fontManager, InventorySlotSprite& slotSprite) const;

    /**
     * @brief Configures textures, position and general setting of the background of the inventory
     * @param gameResources Game resources including fontmanager and texturemanager.
     * @param windowSize Dimensions of the window on which the background of the inventory is drawn
     */
    void setupInventoryBackground(const GameResources& gameResources,
                                  const sf::Vector2<int>& windowSize);

private:
    sf::Sprite mInventoryBackground;
    std::array<std::optional<InventorySlot>, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS> mItems;
    std::array<InventorySlotSprite, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS> mItemsSprites;
    std::unique_ptr<Hotbar> mHotbar;
    const std::string& mSaveWorldFilePath;
    const TexturePack& mTexturePack;
    sf::RenderWindow& mGameWindow;
    Serializer mSerializer;

    std::optional<int> mIndexOfSlotHoldingByMouse;
    bool mIsInventoryOpened = false;
};
