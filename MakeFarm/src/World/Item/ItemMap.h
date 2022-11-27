#pragma once
#include "ItemType.h"
#include "Utils/Settings.h"
#include "string"
#include "unordered_map"

/**
 * A map containing access to information about the items in the game.
 */
class ItemMap
{
public:
    /**
     * @brief Returns an instance of the item map
     * @return Instance of the item map
     */
    const static ItemMap& itemMap();

    /**
     * Returns the type of item, which is information about the item
     * @param itemId The id of the item
     * @return Item information set
     */
    [[nodiscard]] const ItemType& itemType(const ItemId& itemId) const;

private:
    ItemMap();

    /**
     * It searches the folder for item settings.
     * @param directoryName Path to folder in search of items
     */
    void parseDirectory(const std::string& directoryName);

    /**
     * @brief Checks if the file has a .cfg extension
     * @param filePath Path to the file
     * @return True if file has extension .cfg, false otherwise
     */
    bool isFileCfgExtension(std::string& filePath) const;

    /**
     * @brief Reads the item name from the settings
     * @param settingsFile File containing block settings
     * @return Item name
     */
    [[nodiscard]] std::string readItemName(const Settings& settingsFile) const;

    /**
     * @brief Reads the way an object should be rendered
     * @param settingsFile File containing block settings
     * @return The way an object should be rendered
     */
    [[nodiscard]] Item::Render readItemRenderType(const Settings& settingsFile) const;

    /**
     * @brief Reads item identifier
     * @param settingsFile File containing block settings
     * @return Item identifier
     */
    [[nodiscard]] ItemId readItemId(const Settings& settingsFile) const;

    /**
     * @brief Removes the cfg extension from the character string
     * @param fileName The character string from which we want to remove the cfg extension
     */
    void removeCfgExtension(std::string& fileName) const;

    /**
     * @brief Reads the values of the block and assigns it to the map
     * @param settingsFile File containing block settings
     * @param fileName The name of the file containing the configuration of a given block
     */
    void readItemValues(const Settings& settingsFile, const std::string& fileName);

    /**
     * @brief Reads the maximum number of items one slot can hold
     * @param settingsFile File containing block settings
     * @return Maximum number of items one slot can hold
     */
    [[nodiscard]] int readItemMaxAmountInSlot(const Settings& settingsFile) const;

    std::unordered_map<ItemId, ItemType> mItemMap;
};
