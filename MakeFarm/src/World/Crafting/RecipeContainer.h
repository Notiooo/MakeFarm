#pragma once
#include "Utils/Settings.h"
#include "World/Crafting/Recipe.h"
#include <vector>

/**
 * A map containing access to information about the items in the game.
 */
class RecipeContainer
{
public:
    /**
     * @brief Returns an instance of the item map
     * @return Instance of the item map
     */
    const static RecipeContainer& recipeContainer();

    /**
     * Returns the type of item, which is information about the item
     * @param itemId The id of the item
     * @return Item information set
     */
    [[nodiscard]] const std::vector<Recipe>& recipes() const;

private:
    RecipeContainer();

    /**
     * It searches the folder for item settings.
     * @param directoryName Path to folder in search of items
     */
    void parseDirectory(const std::string& directoryName);

    /**
     * @brief Checks if the file has a .cfg extension
     * @param filePath Path to the file
     * @return True if file has extension .cfg, false otherwise
     */
    bool isFileCfgExtension(std::string& filePath) const;

    /**
     * @brief Removes the cfg extension from the character string
     * @param fileName The character string from which we want to remove the cfg extension
     */
    void removeCfgExtension(std::string& fileName) const;

private:
    std::vector<Recipe> mRecipes;
};
