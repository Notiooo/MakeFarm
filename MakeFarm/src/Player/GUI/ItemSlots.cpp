#include "ItemSlots.h"
#include "Utils/Mouse.h"
#include "pch.h"

ItemSlots::ItemSlots(const GameResources& gameResources, const sf::RenderWindow& gameWindow)
    : mGameWindow(gameWindow)
{
}

void ItemSlots::insert(ItemSlot&& itemSlot)
{
    mItemSlots.emplace_back(std::make_shared<ItemSlot>(itemSlot));
}

void ItemSlots::insert(std::shared_ptr<ItemSlot> itemSlot)
{
    mItemSlots.emplace_back(itemSlot);
}


void ItemSlots::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        switch (event.mouseButton.button)
        {
            case sf::Mouse::Left: handleLeftMouseButton(event); break;
            case sf::Mouse::Right: handleRightMouseButton(event); break;
        }
    }
}

void ItemSlots::handleLeftMouseButton(const sf::Event& event)
{
    auto optionalSlot = slotAtPosition(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
    if (!optionalSlot.has_value())
    {
        return;
    }

    auto clickedSlot = optionalSlot.value();
    if (!doesMouseHoldAnItem())
    {
        tryCatchItemAtGivenSlotByMouse(clickedSlot);
    }
    else
    {
        leftClickOnSlotWhileMouseHoldingItem(clickedSlot);
    }
}

void ItemSlots::handleRightMouseButton(const sf::Event& event)
{
    auto optionalSlot = slotAtPosition(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
    if (!optionalSlot.has_value())
    {
        return;
    }

    auto clickedSlot = optionalSlot.value();
    if (doesMouseHoldAnItem())
    {
        auto slotHeldByMouse = mSlotHoldingByMouse.value();
        moveOneItemFromSlotToAnotherSlot(slotHeldByMouse, clickedSlot);
    }
}

void ItemSlots::moveOneItemFromSlotToAnotherSlot(std::shared_ptr<ItemSlot>& slotToMoveFrom,
                                                 std::shared_ptr<ItemSlot>& slotToMoveTo) const
{
    if (slotToMoveTo != slotToMoveFrom)
    {
        if (!slotToMoveTo->doesContainItem())
        {
            moveOneItemFromSlotToAnEmptySlot(slotToMoveFrom, slotToMoveTo);
        }
        else if (slotToMoveTo->item()->id() == slotToMoveFrom->item()->id())
        {
            moveOneItemFromSlotToSlotWithTheSameItem(slotToMoveFrom, slotToMoveTo);
        }
    }
}

void ItemSlots::moveOneItemFromSlotToSlotWithTheSameItem(
    std::shared_ptr<ItemSlot>& slotToMoveFrom, std::shared_ptr<ItemSlot>& slotToMoveTo) const
{
    auto restItemsThatDidNotFitSlot = slotToMoveTo->amount(slotToMoveTo->amount() + 1);
    if (restItemsThatDidNotFitSlot == 0)
    {
        slotToMoveFrom->amount(slotToMoveFrom->amount() - 1);
    }
}

void ItemSlots::moveOneItemFromSlotToAnEmptySlot(std::shared_ptr<ItemSlot>& slotToMoveFrom,
                                                 std::shared_ptr<ItemSlot>& emptySlot) const
{
    emptySlot->item(slotToMoveFrom->item()->id());
    emptySlot->amount(1);
    slotToMoveFrom->amount(slotToMoveFrom->amount() - 1);
}


bool ItemSlots::doesMouseHoldAnItem() const
{
    return mSlotHoldingByMouse.has_value() && mSlotHoldingByMouse.value()->doesContainItem();
}

std::optional<std::shared_ptr<ItemSlot>> ItemSlots::slotAtPosition(sf::Vector2f position)
{
    for (auto& item: mItemSlots)
    {
        if (item->doesContainPoint(position))
        {
            return item;
        }
    }
    return std::nullopt;
}

void ItemSlots::tryCatchItemAtGivenSlotByMouse(const std::shared_ptr<ItemSlot>& slot)
{
    if (slot->doesContainItem())
    {
        holdSlotByMouse(slot);
    }
}

void ItemSlots::leftClickOnSlotWhileMouseHoldingItem(const std::shared_ptr<ItemSlot>& slot)
{
    if (slot == mSlotHoldingByMouse.value())
    {
        clearItemHoldingByMouse();
    }
    else
    {
        dropItemHoldingByMouseOntoTheGivenSlot(slot);
    }
}

void ItemSlots::clearItemHoldingByMouse()
{
    if (doesMouseHoldAnItem())
    {
        auto holdingItem = mSlotHoldingByMouse.value();
        holdingItem->moveBackItemToSlot();
    }
    mSlotHoldingByMouse = std::nullopt;
}

void ItemSlots::dropItemHoldingByMouseOntoTheGivenSlot(const std::shared_ptr<ItemSlot>& slot)
{
    auto slotHeldByMouse = mSlotHoldingByMouse.value();
    if (!slot->doesContainItem() || slot->item()->id() != slotHeldByMouse->item()->id())
    {
        slot->swap(*mSlotHoldingByMouse.value());
        clearItemHoldingByMouse();
    }
    else if (slot->item()->id() == slotHeldByMouse->item()->id())
    {
        auto rest = slot->amount(slotHeldByMouse->amount() + slot->amount());
        slotHeldByMouse->amount(rest);
    }
}

void ItemSlots::update(const float& deltaTime)
{
    if (doesMouseHoldAnItem())
    {
        auto holdingItem = mSlotHoldingByMouse.value();
        holdingItem->itemPosition(sf::Vector2f(Mouse::getPosition(mGameWindow)));
    }
}

void ItemSlots::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto& item: mItemSlots)
    {
        item->draw(target, states);
    }
    if (doesMouseHoldAnItem())
    {
        mSlotHoldingByMouse.value()->draw(target, states);
    }
}

std::size_t ItemSlots::size()
{
    return mItemSlots.size();
}

std::shared_ptr<ItemSlot> ItemSlots::at(const size_t& index)
{
    return mItemSlots.at(index);
}

void ItemSlots::reserve(const size_t& size)
{
    mItemSlots.reserve(size);
}

ItemSlots::SlotsContainer::iterator ItemSlots::begin()
{
    return mItemSlots.begin();
}

ItemSlots::SlotsContainer::iterator ItemSlots::end()
{
    return mItemSlots.end();
}

ItemSlots::SlotsContainer::const_iterator ItemSlots::cbegin() const
{
    return mItemSlots.cbegin();
}

ItemSlots::SlotsContainer::const_iterator ItemSlots::cend() const
{
    return mItemSlots.cend();
}

void ItemSlots::holdSlotByMouse(const std::shared_ptr<ItemSlot>& slot)
{
    clearItemHoldingByMouse();
    mSlotHoldingByMouse = slot;
}

ItemSlots ItemSlots::operator+(const ItemSlots& rhs)
{
    for (auto itemSlot: rhs.mItemSlots)
    {
        mItemSlots.emplace_back(itemSlot);
    }
    return *this;
}

ItemSlots& ItemSlots::operator=(const ItemSlots& rhs)
{
    mItemSlots = rhs.mItemSlots;
    mSlotHoldingByMouse = rhs.mSlotHoldingByMouse;
    return *this;
}

std::shared_ptr<ItemSlot> ItemSlots::slotHoldByMouse()
{
    if (mSlotHoldingByMouse.has_value())
    {
        return mSlotHoldingByMouse.value();
    }
    return nullptr;
}
