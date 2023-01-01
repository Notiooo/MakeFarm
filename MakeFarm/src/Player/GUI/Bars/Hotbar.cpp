#include "Hotbar.h"
#include "Renderer3D/Renderer3D.h"
#include "Resources/TexturePack.h"
#include "Utils/utils.h"
#include "World/Block/BlockMap.h"
#include "pch.h"

Hotbar::Hotbar(ItemSlots& hotbarSlots, const GameResources& gameResources, sf::Vector2i windowSize)
    : mSlots(hotbarSlots)
    , mTexturePack(gameResources.texturePack)
    , mUnselectedBlockTexture(gameResources.textureManager.getResourceReference(
          TextureManagerId::GUI_Inventory_Unselected_Block))
    , mSelectedBlockTexture(gameResources.textureManager.getResourceReference(
          TextureManagerId::GUI_Inventory_Selected_Block))
{
    auto& fontManager = gameResources.fontManager;

    mSizeOfHotbar = {mUnselectedBlockTexture.getSize().x *
                         static_cast<float>(Inventory::NUMBER_OF_COLUMNS) * HOTBAR_SCALE,
                     mUnselectedBlockTexture.getSize().y * HOTBAR_SCALE};

    mRenderPosition = {(static_cast<float>(windowSize.x) / 2.f) - (mSizeOfHotbar.x / 2.f),
                       windowSize.y - BOTTOM_SCREEN_OFFSET};

    setupItemAmountsTexts(fontManager);
    setupHotbarBackgroundSprites();
    setupCurrentlyHoldingItemNameText(fontManager);

    mBackgroundSprites.begin()->setTexture(mSelectedBlockTexture);
}

void Hotbar::setupCurrentlyHoldingItemNameText(const FontManager& fontManager)
{
    mCurrentlyHoldingItemName.setFont(fontManager.getResourceReference(FontId::ArialNarrow));
    mCurrentlyHoldingItemName.setOutlineColor(sf::Color::Black);
    mCurrentlyHoldingItemName.setOutlineThickness(1.f);
    mCurrentlyHoldingItemName.setFillColor(sf::Color::White);
    mCurrentlyHoldingItemName.setCharacterSize(21);
}

void Hotbar::setupHotbarBackgroundSprites()
{
    int slotIndex = 0;
    for (auto& sprite: mBackgroundSprites)
    {
        sprite.setTexture(mUnselectedBlockTexture);
        sprite.setScale(HOTBAR_SCALE, HOTBAR_SCALE);

        auto currentSpriteColor = sprite.getColor();
        currentSpriteColor.a = 200;
        sprite.setColor(currentSpriteColor);

        sprite.setPosition(mRenderPosition.x +
                               slotIndex * mUnselectedBlockTexture.getSize().x * HOTBAR_SCALE,
                           mRenderPosition.y);
        ++slotIndex;
    }
}

void Hotbar::setupItemAmountsTexts(const FontManager& fontManager)
{
    for (auto& text: mItemAmount)
    {
        text.setFont(fontManager.getResourceReference(FontId::ArialNarrow));
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::White);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(1.f);
    }
}

void Hotbar::update(const float& deltaTime)
{
}

void Hotbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

    drawHotbarBackgroundSlots(target, states);
    drawHotbarItems(target, states);

    SfmlDraw(mCurrentlyHoldingItemName, target, states);
}

void Hotbar::drawHotbarItems(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    for (int slotIndex = 0; slotIndex < HOTBAR_SLOTS; ++slotIndex)
    {
        if (mSlots.at(slotIndex)->doesContainItem())
        {
            SfmlDraw(mItemSprites.at(slotIndex), target, states);
            SfmlDraw(mItemAmount.at(slotIndex), target, states);
        }
    }
}

void Hotbar::drawHotbarBackgroundSlots(sf::RenderTarget& target,
                                       const sf::RenderStates& states) const
{
    for (int slotIndex = 0; slotIndex < HOTBAR_SLOTS; ++slotIndex)
    {
        SfmlDraw(mBackgroundSprites.at(slotIndex), target, states);
    }
}

void Hotbar::handleEvent(const sf::Event& event)
{
    switch (event.type)
    {
        case sf::Event::MouseWheelScrolled:
            updateCurrentlySelectedSlot(event.mouseWheelScroll.delta);
            updateTheTextAboutCurrentlyHeldBlock();
            break;
    }
}

void Hotbar::updateCurrentlySelectedSlot(float mouseWheelScrollDelta)
{
    mBackgroundSprites.at(mCurrentlySelectedNumberOfSlot).setTexture(mUnselectedBlockTexture);
    if (mouseWheelScrollDelta < 0.f)
    {
        ++mCurrentlySelectedNumberOfSlot;
    }
    else
    {
        --mCurrentlySelectedNumberOfSlot;
    }
    keepRangesOfSelectedSlotNumber();
    mBackgroundSprites.at(mCurrentlySelectedNumberOfSlot).setTexture(mSelectedBlockTexture);
}

void Hotbar::keepRangesOfSelectedSlotNumber()
{
    if (mCurrentlySelectedNumberOfSlot == Inventory::NUMBER_OF_COLUMNS)
    {
        mCurrentlySelectedNumberOfSlot = 0;
    }
    else if (mCurrentlySelectedNumberOfSlot < 0)
    {
        mCurrentlySelectedNumberOfSlot = Inventory::NUMBER_OF_COLUMNS - 1;
    }
}

void Hotbar::updateTheTextAboutCurrentlyHeldBlock()
{
    auto selectedSlot = mSlots.at(mCurrentlySelectedNumberOfSlot);
    if (selectedSlot->doesContainItem())
    {
        auto itemName = selectedSlot->item()->name();
        mCurrentlyHoldingItemName.setString(itemName);
        centerOrigin(mCurrentlyHoldingItemName);
        mCurrentlyHoldingItemName.setPosition(
            {mRenderPosition.x + mSizeOfHotbar.x / 2.f,
             mRenderPosition.y - mCurrentlyHoldingItemName.getCharacterSize() - 10.f});
    }
    else
    {
        mCurrentlyHoldingItemName.setString("");
    }
}

std::optional<ItemId> Hotbar::tryRemoveItemInHand(int amount)
{
    auto currentlySelectedSlot = mSlots.at(mCurrentlySelectedNumberOfSlot);
    if (currentlySelectedSlot->doesContainItem() && currentlySelectedSlot->amount() >= amount)
    {
        currentlySelectedSlot->amount(currentlySelectedSlot->amount() - amount);

        auto removedItemId = currentlySelectedSlot->item()->id();
        if (currentlySelectedSlot->amount() == 0)
        {
            currentlySelectedSlot->removeItem();
        }
        updateHotbarState();
        return removedItemId;
    }
    return std::nullopt;
}

void Hotbar::updateHotbarState()
{
    for (auto i = 0; i < Inventory::NUMBER_OF_COLUMNS; ++i)
    {
        auto slot = mSlots.at(i);
        if (slot->doesContainItem())
        {
            auto itemInSlot = slot->item().value();
            auto amount = slot->amount();
            auto positionOfCurrentSlot = positionOfGivenSlot(i);
            createSpriteAtGivenSlot(i, itemInSlot, positionOfCurrentSlot);
            createItemAmountAtGivenSlot(i, amount, positionOfCurrentSlot);
        }
    }
    updateTheTextAboutCurrentlyHeldBlock();
}

void Hotbar::createItemAmountAtGivenSlot(int slotIndex, int amount,
                                         const sf::Vector2f& positionOfSlot)
{
    auto& itemAmount = mItemAmount.at(slotIndex);
    auto fontOffset = sf::Vector2f(ITEM_AMOUNT_OFFSET_X, ITEM_AMOUNT_OFFSET_Y) * HOTBAR_SCALE;
    itemAmount.setPosition(positionOfSlot.x + fontOffset.x, positionOfSlot.y + fontOffset.y);

    itemAmount.setString(std::to_string(amount));
}

void Hotbar::createSpriteAtGivenSlot(int slotNumber, Item& itemInSlot,
                                     const sf::Vector2f& positionOfSlot)
{
    auto& itemSprite = mItemSprites.at(slotNumber);
    itemSprite.setTexture(mTexturePack.texture(itemInSlot.renderType()));
    itemSprite.setTextureRect(mTexturePack.textureRect(itemInSlot));
    itemSprite.setScale(HOTBAR_SCALE, HOTBAR_SCALE);

    auto spriteOffset = sf::Vector2f(TEXTURE_OFFSET, TEXTURE_OFFSET) * HOTBAR_SCALE;
    itemSprite.setPosition(positionOfSlot.x + spriteOffset.x, positionOfSlot.y + spriteOffset.y);
}

sf::Vector2f Hotbar::positionOfGivenSlot(int slotIndex) const
{
    auto slotTextureSize = sf::Vector2f(mUnselectedBlockTexture.getSize().x * HOTBAR_SCALE,
                                        mUnselectedBlockTexture.getSize().y * HOTBAR_SCALE);

    auto positionOfCurrentSlot =
        sf::Vector2f(mRenderPosition.x + slotIndex * slotTextureSize.x, mRenderPosition.y);
    return positionOfCurrentSlot;
}

sf::Vector2f Hotbar::position() const
{
    return mRenderPosition;
}
