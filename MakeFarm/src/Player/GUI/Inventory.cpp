#include "Inventory.h"
#include "Player/GUI/Hotbar.h"
#include "pch.h"

Inventory::Inventory(sf::Vector2i windowSize, const GameResources& gameResources)
    : mItems()
    , mTexturePack(gameResources.texturePack)
{
    mHotbar = std::make_unique<Hotbar>(
        reinterpret_cast<std::array<std::optional<InventorySlot>, NUMBER_OF_COLUMNS>&>(mItems),
        gameResources, windowSize);
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

Inventory::~Inventory() = default;