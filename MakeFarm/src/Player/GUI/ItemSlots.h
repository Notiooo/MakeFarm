#pragma once
#include "Player/GUI/ItemSlot.h"
#include "Resources/Resources.h"
#include "World/Item/Item.h"
#include "optional"

/**
 * @brief Representation of a container of ItemSlots.
 */
class ItemSlots
{
public:
    ItemSlots(const GameResources& gameResources, const sf::RenderWindow& gameWindow);
    using SlotsContainer = std::vector<std::shared_ptr<ItemSlot>>;

    /**
     * @brief Inserts the new slot into the container.
     * @param itemSlot A new slot that should be added to the container.
     */
    void insert(ItemSlot&& itemSlot);

    /**
     * @brief Returns how many item slots are in the container.
     * @return Number of item slots in the container.
     */
    std::size_t size();

    /**
     * @brief Returns a item slot on the indicated index.
     * @param index Index of the slot that should be returned.
     * @return Item slot under the indicated index.
     */
    std::shared_ptr<ItemSlot> at(const std::size_t& index);

    /**
     * @brief Reserves the amount of space to be used in the container.
     * @param size Amount of space to be used in the container.
     */
    void reserve(const std::size_t& size);

    /**
     * @brief Returns an iterator of a container pointing to its first element.
     * @return Iterator of a container pointing to its first element.
     */
    [[nodiscard]] SlotsContainer::iterator begin();

    /**
     * @brief Returns a container iterator pointing to the element one
     * after the last container element.
     * @return A container iterator pointing to the element one after the last container element.
     */
    [[nodiscard]] SlotsContainer::iterator end();

    /**
     * @brief Returns an iterator of a container pointing to its first element.
     * @return Iterator of a container pointing to its first element.
     */
    [[nodiscard]] SlotsContainer::const_iterator cbegin() const;

    /**
     * @brief Returns a container iterator pointing to the element one
     * after the last container element.
     * @return A container iterator pointing to the element one after the last container element.
     */
    [[nodiscard]] SlotsContainer::const_iterator cend() const;

    /**
     * @brief It cleans the object held by the mouse - that is, it detaches it from the mouse and
     * returns it back to the slot.
     */
    void clearItemHoldingByMouse();

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * \brief Updates the item slot logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief Draws the inventory and hotbar to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    /**
     * @brief Checks if the mouse is currently holding an item in the container
     * @return True if the mouse holds an object from the container, false otherwise.
     */
    [[nodiscard]] bool doesMouseHoldAnItem() const;

    /**
     * @brief It tries to grab an item from the slot indicated by the mouse.
     * @param Slot from which the item is trying to be caught
     */
    void tryCatchItemAtGivenSlotByMouse(const std::shared_ptr<ItemSlot>& slot);

    /**
     * @brief Performs click logic, which executes when the mouse clicks on the slot holding the
     * item.
     * @param slot The slot the mouse clicks on
     */
    void leftClickOnSlotWhileMouseHoldingItem(const std::shared_ptr<ItemSlot>& slot);

    /**
     * @brief Performs click logic, which executes when the mouse clicks on the slot without holding
     * the item.
     * @param slot The slot the mouse clicks on
     */
    void dropItemHoldingByMouseOntoTheGivenSlot(const std::shared_ptr<ItemSlot>& slot);

    /**
     * @brief Returns the slot that is in the indicated position. The variable contains no value if
     * there is no slot in the indicated position.
     * @param position Position on which the presence of a slot should be checked
     * @return Shared Pointer to ItemSlot, or an empty value if there was nothing on the item.
     */
    std::optional<std::shared_ptr<ItemSlot>> slotAtPosition(sf::Vector2f position);

    /**
     * @brief Mouse catches the indicated slot item.
     * @param slot The slot the mouse catches
     */
    void holdSlotByMouse(const std::shared_ptr<ItemSlot>& slot);

    /**
     * @brief Performs logic for left mouse click
     * @param event user input
     */
    void handleLeftMouseButton(const sf::Event& event);

    /**
     * @brief Performs logic for right mouse click
     * @param event user input
     */
    void handleRightMouseButton(const sf::Event& event);

    /**
     * @brief Moves one item from one slot to another slot that has no item in it.
     * @param slotToMoveFrom The slot from which the item is transferred.
     * @param emptySlot The slot to which the item is transferred
     */
    void moveOneItemFromSlotToAnEmptySlot(std::shared_ptr<ItemSlot>& slotToMoveFrom,
                                          std::shared_ptr<ItemSlot>& emptySlot) const;

    /**
     * @brief Moves one item from one slot to another slot that has the same type of item.
     * @param slotToMoveFrom The slot from which the item is transferred.
     * @param slotToMoveTo The slot to which the item is transferred
     */
    void moveOneItemFromSlotToSlotWithTheSameItem(std::shared_ptr<ItemSlot>& slotToMoveFrom,
                                                  std::shared_ptr<ItemSlot>& slotToMoveTo) const;

    /**
     * @brief Moves one item from one slot to another slot. If both slots have items and they are
     * two different types of items then nothing happens.
     * @param slotToMoveFrom The slot from which the item is transferred.
     * @param slotToMoveTo The slot to which the item is transferred
     */
    void moveOneItemFromSlotToAnotherSlot(std::shared_ptr<ItemSlot>& slotToMoveFrom,
                                          std::shared_ptr<ItemSlot>& slotToMoveTo) const;

private:
    const sf::RenderWindow& mGameWindow;
    SlotsContainer mItemSlots;
    std::optional<std::shared_ptr<ItemSlot>> mSlotHoldingByMouse;
};

template<typename T>
bool ItemSlot::doesContainPoint(const sf::Vector2<T>& point) const
{
    return mBackground.getGlobalBounds().contains(point);
}
