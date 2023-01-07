#include "Crafting.h"
#include "Player/GUI/Inventory.h"
#include "World/Crafting/RecipeContainer.h"
#include "pch.h"

Crafting::Crafting(sf::RenderWindow& gameWindow, const GameResources& gameResources,
                   sf::Vector2f position)
    : mItemSlots(gameResources, gameWindow)
    , mSlotDimension(ItemSlot::dimensions(gameResources.textureManager, Inventory::INVENTORY_SCALE))
    , mPosition(position)
{
    setupSlots(gameResources);
}

void Crafting::setupSlots(const GameResources& gameResources)
{
    mItemSlots.reserve(NUMBER_OF_SLOTS);
    for (auto column = 0; column < NUMBER_OF_COLUMNS; ++column)
    {
        for (auto row = 0; row < NUMBER_OF_ROWS; ++row)
        {
            createItemSlot(gameResources, column, row);
        }
    }
    createItemSlot(gameResources, 4, 1);
    mResultItemSlot = mItemSlots.at(9);
}

void Crafting::createItemSlot(const GameResources& gameResources, int column, int row)
{
    auto itemSlot = std::make_shared<ItemSlot>(gameResources);
    itemSlot->scale(Inventory::INVENTORY_SCALE);
    itemSlot->position(positionOfSlot(sf::Vector2i(column, row)));
    mItemSlots.insert(itemSlot);
}

sf::Vector2f Crafting::positionOfSlot(sf::Vector2<int> positionOfSlot)
{
    return {mPosition.x + positionOfSlot.x * mSlotDimension.x,
            mPosition.y + positionOfSlot.y * mSlotDimension.y};
}

void Crafting::hideResultItem()
{
    mResultItemSlot->removeItem();
}

void Crafting::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (mIsCraftingFinalized)
        {
            updateIfResultItemOfFinilizedCraftingIsTaken();
        }

        if (!mIsCraftingFinalized)
        {
            updateLayoutCorrectness();
        }
    }
}

void Crafting::updateLayoutCorrectness()
{
    auto doesAnyRecipeMatch = false;
    for (auto& recipe: RecipeContainer::recipeContainer().recipes())
    {
        if (isCraftingLayoutEqualToRecipe(recipe))
        {
            mResultItemSlot->item(recipe.finalItem);
            mResultItemSlot->amount(recipe.finalAmount);
            doesAnyRecipeMatch = true;
            break;
        }
    }
    if (!doesAnyRecipeMatch)
    {
        mResultItemSlot->removeItem();
    }
}

void Crafting::updateIfResultItemOfFinilizedCraftingIsTaken()
{
    if (!mResultItemSlot->doesContainItem())
    {
        mIsCraftingFinalized = false;
    }
}

bool Crafting::isCraftingLayoutEqualToRecipe(const Recipe& recipe)
{
    for (auto column = 0; column < NUMBER_OF_COLUMNS; ++column)
    {
        for (auto row = 0; row < NUMBER_OF_ROWS; ++row)
        {
            auto itemSlotInCrafting = mItemSlots.at(column * NUMBER_OF_COLUMNS + row);
            auto itemInCrafting = ItemId::None;
            if (itemSlotInCrafting->doesContainItem())
            {
                itemInCrafting = itemSlotInCrafting->item()->id();
            }

            if (recipe.layout[column][row] != itemInCrafting)
            {
                return false;
            }
        }
    }
    return true;
}

void Crafting::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mItemSlots.draw(target, states);
}

sf::Vector2f Crafting::dimensions(const TextureManager& textureManager, float scale)
{
    auto& slotTexture = textureManager.getResourceReference(TextureManagerId::GUI_Inventory_Slot);
    auto slotDimension = sf::Vector2f(slotTexture.getSize()) * scale;

    return {slotDimension.x * 4, slotDimension.y * 3};
}

ItemSlots Crafting::itemSlots()
{
    return mItemSlots;
}

std::shared_ptr<ItemSlot> Crafting::resultItem()
{
    return mResultItemSlot;
}

void Crafting::removeRecipeFromCraftingZone()
{
    for (int i = 0; i < NUMBER_OF_COLUMNS * NUMBER_OF_ROWS; ++i)
    {
        mItemSlots.at(i)->amount(mItemSlots.at(i)->amount() - 1);
    }
}

void Crafting::finalizeCrafting()
{
    if (!mIsCraftingFinalized)
    {
        mIsCraftingFinalized = true;
        removeRecipeFromCraftingZone();
    }
}
