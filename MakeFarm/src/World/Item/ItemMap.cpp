#include "ItemMap.h"
#include "Utils/Settings.h"
#include "pch.h"
#include <filesystem>

const ItemMap& ItemMap::itemMap()
{
    static const ItemMap instance;

    return instance;
}

const ItemType& ItemMap::itemType(const ItemId& itemId) const
{
    return mItemMap.at(itemId);
}

ItemMap::ItemMap()
{
    parseDirectory("resources/items");
}

void ItemMap::parseDirectory(const std::string& directoryName)
{
    Settings settingsFile;
    for (const auto& file: std::filesystem::recursive_directory_iterator(directoryName))
    {
        if (file.is_regular_file())
        {
            std::string filePath = file.path().string();
            if (isFileCfgExtension(filePath))
            {
                std::string fileName = file.path().filename().string();
                removeCfgExtension(fileName);

                settingsFile.openFile(filePath);
                readItemValues(settingsFile, fileName);
                settingsFile.closeFile();
            }
        }
    }
}

void ItemMap::readItemValues(const Settings& settingsFile, const std::string& fileName)
{
    ItemType item;
    item.id = readItemId(settingsFile);
    item.fileName = fileName;
    item.name = readItemName(settingsFile);
    item.render = readItemRenderType(settingsFile);
    item.maxAmountInSlot = readItemMaxAmountInSlot(settingsFile);

    mItemMap[item.id] = item;
}

int ItemMap::readItemMaxAmountInSlot(const Settings& settingsFile) const
{
    int maxAmountInSlot = ItemType::DEFAULT_MAXIMUM_AMOUNT_IN_SLOT;
    if (settingsFile.isPresent("MaxAmountInSlot"))
    {
        maxAmountInSlot = settingsFile.get<int>("MaxAmountInSlot");
    }
    return maxAmountInSlot;
}

void ItemMap::removeCfgExtension(std::string& fileName) const
{
    fileName.erase(fileName.cend() - 4, fileName.cend());
}

ItemId ItemMap::readItemId(const Settings& settingsFile) const
{
    if (settingsFile.isPresent("Id"))
    {
        return static_cast<ItemId>(settingsFile.get<int>("Id"));
    }
    throw std::runtime_error("No id for item is specified");
}

Item::Render ItemMap::readItemRenderType(const Settings& settingsFile) const
{
    if (settingsFile.isPresent("Render"))
    {
        auto renderType = settingsFile.get<std::string>("Render");
        if (renderType == "Item")
        {
            return Item::Render::Item;
        }
        else if (renderType == "Block")
        {
            return Item::Render::Block;
        }
    }
    throw std::runtime_error("No render type for item is specified");
}

std::string ItemMap::readItemName(const Settings& settingsFile) const
{
    std::string itemName = "Unnamed Item";
    if (settingsFile.isPresent("Name"))
    {
        itemName = settingsFile.get<std::string>("Name");
    }
    return itemName;
}

bool ItemMap::isFileCfgExtension(std::string& filePath) const
{
    return filePath.size() > 4 && std::string(filePath.end() - 4, filePath.end()) == ".cfg";
}
