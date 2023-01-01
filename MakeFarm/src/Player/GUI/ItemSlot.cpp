#include "ItemSlot.h"
#include "pch.h"
#include <Renderer3D/Renderer3D.h>

ItemSlot::ItemSlot(const GameResources& gameResources)
    : mAmount(0)
    , mItem(std::nullopt)
    , mGameResources(gameResources)
    , mTexturePack(gameResources.texturePack)
{
    setupBackgroundSprite(gameResources.textureManager);
    setupItemAmountText(gameResources.fontManager);
}

int ItemSlot::amount() const
{
    return mAmount;
}

int ItemSlot::amount(int amount)
{
    if (amount <= 0)
    {
        removeItem();
        return amount;
    }

    mAmount = amount;
    assert(doesContainItem());
    auto maxAmount = item().value().maximumAmountInSlot();
    auto rest = (amount > maxAmount) ? amount - maxAmount : 0;
    if (rest != 0)
    {
        mAmount = maxAmount;
    }
    else
    {
        mAmount = amount;
    }
    auto amountText = (mAmount != 0) ? std::to_string(mAmount) : "";
    mItemAmountText.setString(amountText);
    return rest;
}

std::optional<Item> ItemSlot::item()
{
    return mItem;
}

void ItemSlot::item(ItemId itemId)
{
    mItem = itemId;
    updateItemSprite();
    moveBackItemToSlot();
}

void ItemSlot::position(const sf::Vector2f& position)
{
    mPosition = position;
    mBackground.setPosition(position);
    itemPosition(position);
}

sf::Vector2f ItemSlot::position() const
{
    return mPosition;
}

void ItemSlot::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    SfmlDraw(mBackground, target, states);
    if (doesContainItem())
    {
        SfmlDraw(mItemSprite, target, states);
        SfmlDraw(mItemAmountText, target, states);
    }
}

void ItemSlot::setupBackgroundSprite(const TextureManager& textureManager)
{
    mBackground.setTexture(
        textureManager.getResourceReference(TextureManagerId::GUI_Inventory_Slot));
}

void ItemSlot::setupItemAmountText(const FontManager& fontManager)
{
    mItemAmountText.setFont(fontManager.getResourceReference(FontId::ArialNarrow));
    mItemAmountText.setCharacterSize(mCharacterSize);
    mItemAmountText.setFillColor(sf::Color::White);
    mItemAmountText.setOutlineColor(sf::Color::Black);
    mItemAmountText.setOutlineThickness(1.f);
}

void ItemSlot::textSize(int characterSize)
{
    mCharacterSize = characterSize;
    mItemAmountText.setCharacterSize(characterSize * mScale);
}

void ItemSlot::scale(float scale)
{
    mScale = scale;
    mBackground.setScale(scale, scale);
    mItemSprite.setScale(scale, scale);
    mItemAmountText.setCharacterSize(mCharacterSize * scale);
    mItemSprite.setPosition(
        {mPosition.x + TEXTURE_OFFSET_X * scale, mPosition.y + TEXTURE_OFFSET_Y * scale});
}

void ItemSlot::updateItemSprite()
{
    assert(mItem.has_value());
    mItemSprite.setTexture(mTexturePack.texture(mItem.value().renderType()));
    mItemSprite.setTextureRect(mTexturePack.textureRect(mItem.value()));
}

sf::Vector2f ItemSlot::dimensions()
{
    auto globalBounds = mBackground.getGlobalBounds();
    return {globalBounds.width, globalBounds.height};
}

void ItemSlot::removeItem()
{
    mItem = std::nullopt;
    mAmount = 0;
}

void ItemSlot::itemPosition(const sf::Vector2f& position)
{
    mItemSprite.setPosition(
        {position.x + TEXTURE_OFFSET_X * mScale, position.y + TEXTURE_OFFSET_Y * mScale});
    mItemAmountText.setPosition(
        {position.x + TEXTURE_OFFSET_X * mScale, position.y + TEXTURE_OFFSET_Y * mScale});
}

void ItemSlot::moveBackItemToSlot()
{
    auto& slotPosition = mBackground.getPosition();
    itemPosition(slotPosition);
}

bool ItemSlot::doesContainItem() const
{
    return mItem.has_value();
}

sf::Vector2f ItemSlot::dimensions(const TextureManager& textureManager, float scale)
{
    auto& slotTexture = textureManager.getResourceReference(TextureManagerId::GUI_Inventory_Slot);
    return sf::Vector2f(slotTexture.getSize()) * scale;
}

void ItemSlot::swap(ItemSlot& rhs)
{
    if (rhs.doesContainItem())
    {
        auto rhsAmount = rhs.amount();
        auto rhsItemId = rhs.item()->id();
        assignAmountAndItemToGivenItemSlot(rhs);
        item(rhsItemId);
        amount(rhsAmount);
    }
    else
    {
        assignAmountAndItemToGivenItemSlot(rhs);
        removeItem();
    }
}

void ItemSlot::assignAmountAndItemToGivenItemSlot(ItemSlot& rhs)
{
    if (doesContainItem())
    {
        rhs.item(item()->id());
        rhs.amount(amount());
    }
    else
    {
        rhs.removeItem();
    }
}