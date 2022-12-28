#include "Inventory.h"
#include "Player/GUI/Bars/Hotbar.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Mouse.h"
#include "Utils/utils.h"
#include "pch.h"
#include <filesystem>

Inventory::Inventory(sf::RenderWindow& gameWindow, const GameResources& gameResources,
                     const std::string& saveWorldPath)
    : mItems()
    , mTexturePack(gameResources.texturePack)
    , mSaveWorldFilePath(saveWorldPath)
    , mGameWindow(gameWindow)
{
    auto windowSize = sf::Vector2i(mGameWindow.getSize());

    mHotbar = std::make_unique<Hotbar>(
        reinterpret_cast<std::array<std::optional<InventorySlot>, NUMBER_OF_COLUMNS>&>(mItems),
        gameResources, windowSize);

    setupInventoryBackground(gameResources, windowSize);
    setupSpritesOfSlots(gameResources);
    loadSavedInventoryData();
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

void Inventory::setupSpritesOfSlots(const GameResources& gameResources)
{
    auto slotIndex = 0;
    for (auto& sprite: mItemsSprites)
    {
        setupBackgroundSprite(gameResources.textureManager, slotIndex, sprite);
        setupItemAmountText(gameResources.fontManager, sprite);
        ++slotIndex;
    }
}

void Inventory::setupItemAmountText(const FontManager& fontManager,
                                    Inventory::InventorySlotSprite& slotSprite) const
{
    slotSprite.itemAmountText.setFont(fontManager.getResourceReference(FontId::ArialNarrow));
    slotSprite.itemAmountText.setCharacterSize(7 * INVENTORY_SCALE);
    slotSprite.itemAmountText.setFillColor(sf::Color::White);
    slotSprite.itemAmountText.setOutlineColor(sf::Color::Black);
    slotSprite.itemAmountText.setOutlineThickness(1.f);
}

void Inventory::setupBackgroundSprite(const TextureManager& textureManager, int slotIndex,
                                      Inventory::InventorySlotSprite& slotSprite)
{
    slotSprite.background.setTexture(
        textureManager.getResourceReference(TextureManagerId::GUI_Inventory_Slot));
    slotSprite.background.setScale(INVENTORY_SCALE, INVENTORY_SCALE);
    slotSprite.background.setPosition(positionOfGivenSlot(slotIndex));
}

bool Inventory::tryAddItem(const ItemId& itemToInsert, int amount)
{
    auto inventorySlot = findExistingItemSlotThatMayTakeGivenAmount(itemToInsert, amount);
    if (inventorySlot)
    {
        inventorySlot->amount += amount;
        updateInventoryState();
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
        updateInventoryState();
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
    if (doesMouseHoldAnItem())
    {
        auto& holdingItem = mItemsSprites.at(mIndexOfSlotHoldingByMouse.value());
        holdingItem.item.setPosition(sf::Vector2f(Mouse::getPosition(mGameWindow)));
        holdingItem.itemAmountText.setPosition(sf::Vector2f(Mouse::getPosition(mGameWindow)));
    }
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
    drawAllSlots(target, states);
    tryDrawItemHoldingByMouse(target, states);
}

void Inventory::drawAllSlots(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; ++i)
    {
        auto& sprite = mItemsSprites.at(i);
        SfmlDraw(sprite.background, target, states);

        if (mItems.at(i).has_value())
        {
            SfmlDraw(sprite.item, target, states);
            SfmlDraw(sprite.itemAmountText, target, states);
        }
    }
}

void Inventory::tryDrawItemHoldingByMouse(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (doesMouseHoldAnItem())
    {
        auto& holdingItem = mItemsSprites.at(mIndexOfSlotHoldingByMouse.value());
        SfmlDraw(holdingItem.item, target, states);
        SfmlDraw(holdingItem.itemAmountText, target, states);
    }
}

void Inventory::handleEvent(const sf::Event& event)
{
    if (mIsInventoryOpened)
    {
        handleLeftMouseClick(event);
    }

    mHotbar->handleEvent(event);
}

void Inventory::handleLeftMouseClick(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == Mouse::Left)
        {
            auto optionalSlotIndex =
                indexOfSlotAtPosition({event.mouseButton.x, event.mouseButton.y});

            auto doesMouseClickedAtSlot = optionalSlotIndex.has_value();
            if (!doesMouseHoldAnItem())
            {
                catchItemAtGivenSlotByMouse(optionalSlotIndex);
            }
            else if (doesMouseClickedAtSlot)
            {
                clickOnSlotWhileMouseHoldingItem(optionalSlotIndex.value());
            }
        }
    }
}

void Inventory::clickOnSlotWhileMouseHoldingItem(int clickedSlotIndex)
{
    auto& slotIndex = clickedSlotIndex;
    auto doesItClickedOnTheSameSlotFromWhichItemIsHolding =
        slotIndex == mIndexOfSlotHoldingByMouse.value();

    if (doesItClickedOnTheSameSlotFromWhichItemIsHolding)
    {
        clearItemHoldingByMouse();
    }
    else
    {
        dropItemHoldingByMouseOntoTheGivenSlot(slotIndex);
    }
    updateInventoryState();
}

void Inventory::clearItemHoldingByMouse()
{
    mIndexOfSlotHoldingByMouse = std::nullopt;
}

void Inventory::dropItemHoldingByMouseOntoTheGivenSlot(const int& slotIndex)
{
    auto& selectedSlot = mItems.at(slotIndex);
    if (!selectedSlot.has_value())
    {
        mItems.at(slotIndex) = mItems.at(mIndexOfSlotHoldingByMouse.value());
        mItems.at(mIndexOfSlotHoldingByMouse.value()) = std::nullopt;
        clearItemHoldingByMouse();
    }
}

void Inventory::catchItemAtGivenSlotByMouse(const std::optional<int>& slotIndex)
{
    mIndexOfSlotHoldingByMouse = slotIndex;
}

bool Inventory::doesMouseHoldAnItem() const
{
    return mIndexOfSlotHoldingByMouse.has_value();
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

        if (doesMouseHoldAnItem())
        {
            clearItemHoldingByMouse();
            updateInventoryState();
        }
    }
    else
    {
        Mouse::lockMouseAtCenter(mGameWindow);
    }
}

sf::Vector2f Inventory::positionOfGivenSlot(int index)
{
    auto inventoryDimensions = mInventoryBackground.getGlobalBounds();
    auto slotDimensions = mItemsSprites.at(0).background.getGlobalBounds();

    auto row = static_cast<int>(index / NUMBER_OF_COLUMNS);
    auto column = index % NUMBER_OF_COLUMNS;

    auto widthOfRowOfSlots = slotDimensions.width * NUMBER_OF_COLUMNS;
    auto leftPadding = (inventoryDimensions.width - widthOfRowOfSlots) / 2.f;

    auto leftTopCornerOfInventory = sf::Vector2f(inventoryDimensions.left, inventoryDimensions.top);

    auto startingPosition =
        leftTopCornerOfInventory +
        sf::Vector2f(leftPadding, inventoryDimensions.height - slotDimensions.height);


    if (row == 0)
    {
        startingPosition -= sf::Vector2f(0, 3 * INVENTORY_SCALE);
        return startingPosition + sf::Vector2f(slotDimensions.width * column, 0);
    }

    startingPosition -= sf::Vector2f(0, 6 * INVENTORY_SCALE);
    return startingPosition +
           sf::Vector2f(slotDimensions.width * column, -slotDimensions.height * row);
}

void Inventory::updateInventoryState()
{
    auto index = 0;
    for (auto& slot: mItems)
    {
        if (slot.has_value())
        {
            auto& itemInSlot = slot.value().item;
            auto& amount = slot.value().amount;
            auto positionOfCurrentSlot = positionOfGivenSlot(index);
            createSpriteAtGivenSlot(index, itemInSlot, positionOfCurrentSlot);
            createItemAmountAtGivenSlot(index, amount, positionOfCurrentSlot);
        }

        ++index;
    }
    hotbar().updateHotbarState();
}

void Inventory::createSpriteAtGivenSlot(int slotNumber, Item& itemInSlot, sf::Vector2f slotPosition)
{
    auto& itemSprite = mItemsSprites.at(slotNumber).item;
    itemSprite.setTexture(mTexturePack.texture(itemInSlot.renderType()));
    itemSprite.setTextureRect(mTexturePack.textureRect(itemInSlot));
    itemSprite.setScale(INVENTORY_SCALE, INVENTORY_SCALE);
    itemSprite.setPosition(slotPosition.x + ITEM_OFFSET, slotPosition.y + ITEM_OFFSET);
}

void Inventory::createItemAmountAtGivenSlot(int slotNumber, int amount, sf::Vector2f slotPosition)
{
    auto& itemAmount = mItemsSprites.at(slotNumber).itemAmountText;
    auto fontOffset = sf::Vector2f(ITEM_AMOUNT_OFFSET_X, ITEM_AMOUNT_OFFSET_Y) * INVENTORY_SCALE;
    itemAmount.setPosition(slotPosition.x + fontOffset.x, slotPosition.y + fontOffset.y);

    itemAmount.setString(std::to_string(amount));
}

std::optional<int> Inventory::indexOfSlotAtPosition(sf::Vector2i position)
{
    auto slotDimensions = mItemsSprites.at(0).background.getGlobalBounds();

    for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; ++i)
    {
        auto slotPosition = positionOfGivenSlot(i);
        if (slotPosition.x <= position.x && slotPosition.x + slotDimensions.width >= position.x &&
            slotPosition.y <= position.y && slotPosition.y + slotDimensions.height >= position.y)
        {
            return i;
        }
    }
    return std::nullopt;
}


Inventory::~Inventory()
{
    saveInventoryDataToFile();
}