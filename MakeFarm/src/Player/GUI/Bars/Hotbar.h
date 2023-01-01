#pragma once
#include "Player/GUI/Inventory.h"
#include "Player/GUI/ItemSlot.h"
#include <array>

/**
 * @brief Quick access to the most important equipment slots always displayed on the screen.
 */
class Hotbar
{
public:
    static constexpr auto HOTBAR_SLOTS = Inventory::NUMBER_OF_COLUMNS;
    static constexpr auto BOTTOM_SCREEN_OFFSET = 100.f;
    static constexpr auto HOTBAR_SCALE = 2.f;
    static constexpr auto TEXTURE_OFFSET = 2.f;
    static constexpr auto ITEM_AMOUNT_OFFSET_X = 2.5f;
    static constexpr auto ITEM_AMOUNT_OFFSET_Y = 0.5;


    Hotbar(ItemSlots& hotbarSlots, const GameResources& gameResources, sf::Vector2i windowSize);

    /**
     * \brief Updates the hotbar logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * @brief Updates the status of the Hotbar in terms of what is in it and in what quantity.
     */
    void updateHotbarState();

    /**
     * \brief Draws the hotbar to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief Attempts to remove a designated amount of an object from the hand. If the operation
     * was successful it returns the identifier of the removed object, otherwise it returns an empty
     * optional.
     * @param amount Number of items to be removed
     * @return Identifier of the removed item, otherwise it returns an empty optional.
     */
    std::optional<ItemId> tryRemoveItemInHand(int amount = 1);

    /**
     * @brief Returns the position of the upper left hotbar point.
     * @return Position of the upper left hotbar point.
     */
    sf::Vector2f position() const;

private:
    /**
     * @brief Updates text information about the block currently being held
     */
    void updateTheTextAboutCurrentlyHeldBlock();

    /**
     * @brief Checks and keeps an eye on the ranges of the currently selected slot in the hotbar
     */
    void keepRangesOfSelectedSlotNumber();

    /**
     * @brief Updates information about the currently selected hotbar slot based on mouse scroll
     * movement
     * @param mouseWheelScrollDelta Mouse scroll movement delta from Event
     */
    void updateCurrentlySelectedSlot(float mouseWheelScrollDelta);

    /**
     * @brief Draws the entire hotbar background, i.e. individual "empty" slots
     * @param target where it should be drawn to
     * @param states provides information about rendering process (transform, shader, blend mode)
     */
    void drawHotbarBackgroundSlots(sf::RenderTarget& target, const sf::RenderStates& states) const;

    /**
     * @brief Draws individual items on the hotbar
     * @param target where it should be drawn to
     * @param states provides information about rendering process (transform, shader, blend mode)
     */
    void drawHotbarItems(sf::RenderTarget& target, const sf::RenderStates& states) const;

    /**
     * @brief Sets the text displaying the number of items in the hotbar correctly
     * @param fontManager Manager that stores fonts in the game
     */
    void setupItemAmountsTexts(const FontManager& fontManager);

    /**
     * @brief Prepares correct values for hotbar's "background"
     */
    void setupHotbarBackgroundSprites();

    /**
     * @brief Prepares and sets the correct values of the text displaying the name of the item over
     * the hotbar
     * @param fontManager Manager that stores fonts in the game
     */
    void setupCurrentlyHoldingItemNameText(const FontManager& fontManager);

    /**
     * @brief Returns the position in the form of the upper left corner of the individual slot.
     * @param slotIndex Slot number
     * @return The position of the upper left corner of the slot at a given location.
     */
    sf::Vector2f positionOfGivenSlot(int slotIndex) const;

    /**
     * @brief Creates a sprite of the indicated item on a slot with the indicated number and
     * position
     * @param slotNumber The number of the slot on which you want to place the sprite.
     * @param itemInSlot The object whose sprite we want to place in the indicated slot.
     * @param positionOfSlot Position of the top left corner of the slot
     */
    void createSpriteAtGivenSlot(int slotNumber, Item& itemInSlot,
                                 const sf::Vector2f& positionOfSlot);

    /**
     * @brief Creates a sprite of an item in a particular hotbar slot
     * @param slotIndex Index of the slot in which the sprite should be created
     * @param amount The number of items that should be displayed
     * @param positionOfSlot Position of the top left corner of the slot
     */
    void createItemAmountAtGivenSlot(int slotIndex, int amount, const sf::Vector2f& positionOfSlot);

private:
    ItemSlots& mSlots;
    std::array<sf::Sprite, Inventory::NUMBER_OF_COLUMNS> mBackgroundSprites;
    std::array<sf::Sprite, Inventory::NUMBER_OF_COLUMNS> mItemSprites;
    std::array<sf::Text, Inventory::NUMBER_OF_COLUMNS> mItemAmount;
    sf::Text mCurrentlyHoldingItemName;

    const TexturePack& mTexturePack;

    int mCurrentlySelectedNumberOfSlot = 0;
    sf::Vector2f mSizeOfHotbar;
    sf::Vector2f mRenderPosition;

    const sf::Texture& mSelectedBlockTexture;
    const sf::Texture& mUnselectedBlockTexture;
};
