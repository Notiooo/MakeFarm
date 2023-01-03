#pragma once
#include "Player/GUI/ItemSlots.h"
#include <World/Crafting/Recipe.h>

/**
 * @brief Crafting system used to create one new object from a 9x9 grid of objects.
 */
class Crafting
{
public:
    static constexpr auto NUMBER_OF_ROWS = 3;
    static constexpr auto NUMBER_OF_COLUMNS = 3;
    static constexpr auto NUMBER_OF_RESULT_SLOTS = 1;
    static constexpr auto NUMBER_OF_SLOTS =
        NUMBER_OF_ROWS * NUMBER_OF_RESULT_SLOTS + NUMBER_OF_RESULT_SLOTS;

    static sf::Vector2f dimensions(const TextureManager& textureManager, float scale);

    Crafting(sf::RenderWindow& gameWindow, const GameResources& gameResources,
             sf::Vector2f position);

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief Returns the item slots that the crafting system contains (9x9 grid and output score).
     * @return Item slots that the crafting system contains (9x9 grid and output score).
     */
    ItemSlots itemSlots();

    /**
     * @brief Hides an item that is the result of crafting items located in a 9x9 grid.
     */
    void hideResultItem();

    /**
     * @brief Returns an item resualting from the crafting of items from the 9x9 grid.
     * @return An item resualting from the crafting of items from the 9x9 grid.
     */
    std::shared_ptr<ItemSlot> resultItem();

    /**
     * @brief Removes the current recipe from the 9x9 grid, which contains items for crafting.
     */
    void removeRecipeFromCraftingZone();

    /**
     * @brief Finalizes the crafting, removes the items creating the recipe in the 9x9 grid and
     * waits for the player to take away the effect of their crafting.
     */
    void finalizeCrafting();

    /**
     * \brief Draws the crafting to the screen.
     * \param target where drawable object should be drawn to.
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    /**
     * @brief Sets up item slots, assigning positions and textures.
     * @param gameResources Game resources including fontmanager and texturemanager.
     */
    void setupSlots(const GameResources& gameResources);

    /**
     * @brief Returns the position of the slot on the individual x, y part of the grid. The slots
     * form a 2D grid, where the smallest element of the grid is the slot.
     * @param positionOfSlot Individual x, y part of the grid.
     * @return Position of the slot on the individual x, y part of the grid.
     */
    sf::Vector2f positionOfSlot(sf::Vector2<int> positionOfSlot);

    /**
     * @brief Creates an item slot at the indicated column and row position. Assigns textures to it,
     * gives it a position. The slots form a 2D grid, where the smallest element of the grid is the
     * slot.
     * @param gameResources Game resources including fontmanager and texturemanager.
     * @param column The column on which the slot is to be placed
     * @param row The row on which the slot is to be placed
     */
    void createItemSlot(const GameResources& gameResources, int column, int row);

    /**
     * @brief Verifies that the crafting layout is consistent with the indicated recipe.
     * @param recipe The recipe with which the crafting zone is to be compared
     * @return True if the recipe follows the layout in the crafting zone, false otherwise.
     */
    bool isCraftingLayoutEqualToRecipe(const Recipe& recipe);

    /**
     * @brief Updates whether the transaction can again be considered non-finalized - i.e. whether
     * the player has taken the item resulting from the crafting and the process can be repeated
     * again.
     */
    void updateIfResultItemOfFinilizedCraftingIsTaken();

    /**
     * @brief It updates whether the layout in the crafting zone is correct and also sets the
     * resulting object on this basis.
     */
    void updateLayoutCorrectness();

private:
    bool mIsCraftingFinalized = false;
    std::shared_ptr<ItemSlot> mResultItemSlot;
    ItemSlots mItemSlots;
    sf::Vector2f mPosition;
    const sf::Vector2f mSlotDimension;
};