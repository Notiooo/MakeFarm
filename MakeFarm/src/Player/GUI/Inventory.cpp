#include "Inventory.h"
#include "Player/GUI/Bars/Hotbar.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Mouse.h"
#include "Utils/utils.h"
#include "pch.h"
#include <filesystem>

Inventory::Inventory(sf::RenderWindow& gameWindow, const GameResources& gameResources,
                     const std::string& saveWorldPath)
    : mItems(gameResources, gameWindow)
    , mTexturePack(gameResources.texturePack)
    , mSaveWorldFilePath(saveWorldPath)
    , mGameWindow(gameWindow)
    , mTextureManager(gameResources.textureManager)
{
    auto windowSize = sf::Vector2i(mGameWindow.getSize());
    setupInventoryBackground(gameResources, windowSize);
    setupInventorySlots(gameResources);

    mHotbar = std::make_unique<Hotbar>(mItems, gameResources, windowSize);

    loadSavedInventoryData();
}

void Inventory::setupInventorySlots(const GameResources& gameResources)
{
    mItems.reserve(NUMBER_OF_SLOTS);
    for (auto itemIndex = 0; itemIndex < NUMBER_OF_SLOTS; ++itemIndex)
    {
        auto itemSlot = ItemSlot(gameResources);
        itemSlot.scale(INVENTORY_SCALE);
        itemSlot.position(positionOfGivenSlot(itemIndex));
        mItems.insert(std::move(itemSlot));
    }
}

void Inventory::setupInventoryBackground(const GameResources& gameResources,
                                         const sf::Vector2<int>& windowSize)
{
    mInventoryBackground.setTexture(gameResources.textureManager.getResourceReference(
        TextureManagerId::GUI_Inventory_Background));
    mInventoryBackground.setScale(INVENTORY_SCALE, INVENTORY_SCALE);
    centerOrigin(mInventoryBackground);
    mInventoryBackground.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
}

bool Inventory::tryAddItem(const ItemId& itemToInsert, int amount)
{
    auto inventorySlot = findExistingItemSlotThatMayTakeGivenAmount(itemToInsert, amount);
    if (inventorySlot)
    {
        inventorySlot->amount(inventorySlot->amount() + amount);
        updateInventoryState();
        return true;
    }

    return tryEmplaceFirstEmptySlot(amount, itemToInsert);
}

bool Inventory::tryEmplaceFirstEmptySlot(int amount, const ItemId& itemId)
{
    auto found = std::find_if(this->mItems.begin(), this->mItems.end(),
                              [](const std::shared_ptr<ItemSlot>& slot)
                              {
                                  return !slot->doesContainItem();
                              });

    if (found != mItems.end())
    {
        auto& itemSlot = *found;
        itemSlot->item(itemId);
        itemSlot->amount(amount);
        updateInventoryState();
        return true;
    }

    return false;
}

std::shared_ptr<ItemSlot> Inventory::findExistingItemSlotThatMayTakeGivenAmount(
    const ItemId& itemId, int amount)
{
    auto found = std::find_if(mItems.begin(), mItems.end(),
                              [itemId, amount](const std::shared_ptr<ItemSlot>& slot)
                              {
                                  if (slot->doesContainItem())
                                  {
                                      auto item = slot->item().value();
                                      return item.id() == itemId &&
                                             slot->amount() + amount <= item.maximumAmountInSlot();
                                  }
                                  return false;
                              });

    if (found != mItems.end())
    {
        return *found;
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
    mItems.update(deltaTime);
    mHotbar->update(deltaTime);
}

void Inventory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mHotbar->draw(target, states);

    if (mIsInventoryOpened)
    {
        drawInventory(target, states);
    }
}

void Inventory::drawInventory(sf::RenderTarget& target, sf::RenderStates states) const
{
    SfmlDraw(mInventoryBackground, target, states);
    mItems.draw(target, states);
}

void Inventory::handleEvent(const sf::Event& event)
{
    if (mIsInventoryOpened)
    {
        mItems.handleEvent(event);
        updateInventoryState();
    }

    mHotbar->handleEvent(event);
}

std::string Inventory::inventorySaveFilePath()
{
    return mSaveWorldFilePath + "/inventory.bin";
}

Inventory::SerializableInventory Inventory::serializableInventory()
{
    SerializableInventory inventoryToSave;
    for (auto slotId = 0; slotId < mItems.size(); ++slotId)
    {
        auto currentSlot = mItems.at(slotId);
        if (currentSlot->doesContainItem())
        {
            inventoryToSave[slotId] =
                SerializableInventorySlot{currentSlot->amount(), currentSlot->item()->id()};
        }
    }
    return inventoryToSave;
}

void Inventory::overwriteInventory(const Inventory::SerializableInventory& inventory)
{
    for (auto slotId = 0; slotId < mItems.size(); ++slotId)
    {
        auto& currentSlot = inventory.at(slotId);
        if (currentSlot.amount > 0)
        {
            mItems.at(slotId)->item(currentSlot.item);
            mItems.at(slotId)->amount(currentSlot.amount);
        }
    }
}

void Inventory::saveInventoryDataToFile()
{
    mSerializer.serialize(serializableInventory());
    mSerializer.saveToFile(inventorySaveFilePath());
}

void Inventory::loadSavedInventoryData()
{
    std::ifstream file(inventorySaveFilePath(), std::ios::binary);
    if (file.is_open())
    {
        SerializableInventory inventoryToLoad;
        mSerializer.readSerialized(file, inventoryToLoad);
        overwriteInventory(inventoryToLoad);
        updateInventoryState();
    }
}

void Inventory::clear()
{
    for (auto& mItem: mItems)
    {
        mItem.reset();
    }
    updateInventoryState();
}

void Inventory::toggleInventory()
{
    mIsInventoryOpened = !mIsInventoryOpened;
    if (mIsInventoryOpened)
    {
        Mouse::unlockMouse(mGameWindow);
        mItems.clearItemHoldingByMouse();
    }
    else
    {
        Mouse::lockMouseAtCenter(mGameWindow);
        updateInventoryState();
    }
}

sf::Vector2f Inventory::positionOfGivenSlot(int index)
{
    auto inventoryDimensions = mInventoryBackground.getGlobalBounds();
    auto slotDimensions = ItemSlot::dimensions(mTextureManager, INVENTORY_SCALE);

    auto row = static_cast<int>(index / NUMBER_OF_COLUMNS);
    auto column = index % NUMBER_OF_COLUMNS;

    auto widthOfRowOfSlots = slotDimensions.x * NUMBER_OF_COLUMNS;
    auto leftPadding = (inventoryDimensions.width - widthOfRowOfSlots) / 2.f;

    auto leftTopCornerOfInventory = sf::Vector2f(inventoryDimensions.left, inventoryDimensions.top);

    auto startingPosition =
        leftTopCornerOfInventory +
        sf::Vector2f(leftPadding, inventoryDimensions.height - slotDimensions.y);


    if (row == 0)
    {
        startingPosition -= sf::Vector2f(0, 3 * INVENTORY_SCALE);
        return startingPosition + sf::Vector2f(slotDimensions.x * column, 0);
    }

    startingPosition -= sf::Vector2f(0, 6 * INVENTORY_SCALE);
    return startingPosition + sf::Vector2f(slotDimensions.x * column, -slotDimensions.y * row);
}

void Inventory::updateInventoryState()
{
    hotbar().updateHotbarState();
}


Inventory::~Inventory()
{
    saveInventoryDataToFile();
}