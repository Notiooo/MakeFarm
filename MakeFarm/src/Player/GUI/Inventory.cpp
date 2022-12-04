#include "Inventory.h"
#include "Player/GUI/Hotbar.h"
#include "pch.h"
#include <filesystem>
#include <serializer.h>

Inventory::Inventory(sf::Vector2i windowSize, const GameResources& gameResources,
                     const std::string& saveWorldPath)
    : mItems()
    , mTexturePack(gameResources.texturePack)
    , mSaveWorldFilePath(saveWorldPath)
{
    mHotbar = std::make_unique<Hotbar>(
        reinterpret_cast<std::array<std::optional<InventorySlot>, NUMBER_OF_COLUMNS>&>(mItems),
        gameResources, windowSize);

    std::ifstream file(inventorySaveFilePath(), std::ios::binary);
    if (file.is_open())
    {
        readSerializedInventory(file);
        hotbar().updateHotbarState();
    }
}

bool Inventory::tryAddItem(const ItemId& itemToInsert, int amount)
{
    auto inventorySlot = findExistingItemSlotThatMayTakeGivenAmount(itemToInsert, amount);
    if (inventorySlot)
    {
        inventorySlot->amount += amount;
        hotbar().updateHotbarState();
        return true;
    }

    return tryEmplaceFirstEmptySlot(amount, itemToInsert);
}

template<typename... Ts>
bool Inventory::tryEmplaceFirstEmptySlot(Ts&&... args)
{
    auto found = std::find_if(this->mItems.begin(), this->mItems.end(),
                              [](std::optional<InventorySlot>& slot)
                              {
                                  return !slot.has_value();
                              });

    if (found != mItems.end())
    {
        found->emplace(std::forward<Ts>(args)...);
        hotbar().updateHotbarState();
        return true;
    }

    return false;
}

InventorySlot* const Inventory::findExistingItemSlotThatMayTakeGivenAmount(const ItemId& item,
                                                                           int amount)
{
    auto found =
        std::find_if(mItems.begin(), mItems.end(),
                     [item, amount](std::optional<InventorySlot>& slot)
                     {
                         if (slot.has_value())
                         {
                             return slot->item.id() == item &&
                                    slot->amount + amount <= slot->item.maximumAmountInSlot();
                         }
                         return false;
                     });

    if (found != mItems.end())
    {
        return &found->value();
    }

    return nullptr;
}

Hotbar& Inventory::hotbar()
{
    return *mHotbar;
}

const Hotbar& Inventory::hotbar() const
{
    return *mHotbar;
}

void Inventory::update(const float& deltaTime)
{
    mHotbar->update(deltaTime);
}

void Inventory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mHotbar->draw(target, states);
}

void Inventory::handleEvent(const sf::Event& event)
{
    mHotbar->handleEvent(event);
}

std::string Inventory::inventorySaveFilePath()
{
    return mSaveWorldFilePath + "/inventory.bin";
}

void Inventory::saveInventoryToFile()
{
    std::vector<unsigned char> serializedData = serializedInventory();

    std::filesystem::create_directories(mSaveWorldFilePath);
    std::ofstream outfile(inventorySaveFilePath(), std::ios::out | std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(serializedData.data()),
                  serializedData.size() * sizeof(char));
    outfile.close();
}

std::vector<unsigned char> Inventory::serializedInventory()
{
    std::vector<unsigned char> serializedData;
    zpp::serializer::memory_output_archive out(serializedData);
    out(serializableInventory());
    return serializedData;
}

Inventory::SerializableInventory Inventory::serializableInventory()
{
    SerializableInventory inventoryToSave;
    for (auto slotId = 0; slotId < mItems.size(); ++slotId)
    {
        auto& currentItem = mItems.at(slotId);
        if (currentItem.has_value())
        {
            auto& slotValue = currentItem.value();
            inventoryToSave[slotId] =
                SerializableInventorySlot{slotValue.amount, slotValue.item.id()};
        }
    }
    return inventoryToSave;
}

void Inventory::readSerializedInventory(std::ifstream& file)
{
    // TODO: This thing is actually repeated in three places. Would be got to extract it
    std::istreambuf_iterator<char> iter(file);
    std::vector<char> vec(iter, std::istreambuf_iterator<char>{});
    std::vector<unsigned char> data(vec.begin(), vec.end());
    zpp::serializer::memory_input_archive in(data);
    // ==========
    SerializableInventory inventoryToLoad;
    in(inventoryToLoad);
    overwriteInventory(inventoryToLoad);
}

void Inventory::overwriteInventory(const Inventory::SerializableInventory& inventory)
{
    for (auto slotId = 0; slotId < mItems.size(); ++slotId)
    {
        auto& currentSlot = inventory.at(slotId);
        if (currentSlot.amount > 0)
        {
            mItems.at(slotId).emplace(currentSlot.amount, currentSlot.item);
        }
    }
}


Inventory::~Inventory()
{
    saveInventoryToFile();
}
