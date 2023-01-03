#pragma once
#include "Utils/Settings.h"
#include "World/Crafting/Recipe.h"
#include <vector>

/**
 * \brief A class used to read game settings + items related things like crafting recipes
 *
 * TODO: This class is very problematic and does not do things properly.
 *       In the future the configs should be changed to JSONs.
 */
class RecipeSettings : public Settings
{
public:
    using Settings::Settings;

    /**
     * @brief Returns the recipes found in the given item configuration file.
     * @return Recipes found in the given item configuration file.
     */
    std::vector<Recipe> getRecipes() const;

private:
    /**
     * @brief Reads from the line contained in the stringstream in what quantity the item resulting
     * from crafting should be.
     * @param streamWithLineToParse Stringstream in which there is a line containing information
     * about the final quantity of an item.
     * @return Number in what quantity the item resulting from crafting should be.
     */
    int parseFinalAmountValueFromLine(std::stringstream& streamWithLineToParse) const;

    /**
     * @brief Reads from the stringstream all item id's that are in the stringstream.
     * @param streamWithValueToParse Stringstream from which item ids should be read.
     * @param vectorToInsertParsedItemIds The vector to which item ids should be written.
     */
    void parseItemIdValuesFromLineIntoVector(
        std::stringstream& streamWithValueToParse,
        std::vector<ItemId>& vectorToInsertParsedItemIds) const;

    /**
     * @brief Processes the line on the basis of which it will complete the information on itemids
     * and the final quantities of an item making up the crafting recipe. These are the lines
     * between Crafting and EndCrafting of the item's configuration file.
     * @param line Lines between Crafting and EndCrafting of the item's configuration file.
     * @param lineIndex The index representing which is which is the recipe line. Each recipe
     * consists of 4 lines. There are three lines of a 3x3 crafting grid, and one line indicating
     * the final quantity of an item resulting from the crafting.
     * @param allItemIdsInRecipesCombined A container containing all the id's that make up a
     * recipes.
     * @param allFinalAmountsInRecipesCombined A container containing all the final quantities of an
     * item making up a recipe.
     */
    void parseRecipeLine(const std::string& line, int lineIndex,
                         std::vector<ItemId>& allItemIdsInRecipesCombined,
                         std::vector<int>& allFinalAmountsInRecipesCombined) const;

    /**
     * @brief It parses the lines of the item configuration file and, based on this, fills in
     * information about all item id's that make up the recipes, and all final quantities of an item
     * resulting from crafting.
     * @param allRecipesItemsIdMerged A container that will be filled and will contain all the id's
     * that make up a recipe.
     * @param allRecipesAmountMerged A container that will be filled and will contain all the final
     * quantities of an item making up a recipe.
     */
    void parseLines(std::vector<ItemId>& allRecipesItemsIdMerged,
                    std::vector<int>& allRecipesAmountMerged) const;

    /**
     * @brief Based on the linearly stored recipes in the form of linearly stored itemid forming a
     * 9x9 grid, and a container containing linearly stored entries of the final quantity of an item
     * being crafted, a set of recipes is created for the block of the configuration file currently
     * being processed
     * @param idOfFinalBlock Identifier of the block that is the final result of the recipe. This is
     * the object configuration file currently being processed.
     * @param allRecipesItemsIdMerged A container containing all the id's that make up a recipes.
     * @param allRecipesAmountMerged A container containing all the final quantities of an item
     * making up a recipe.
     * @return Recipe container for the individual outcome item.
     */
    std::vector<Recipe> parseRecipes(int idOfFinalBlock,
                                     const std::vector<ItemId>& allRecipesItemsIdMerged,
                                     const std::vector<int>& allRecipesAmountMerged) const;

    /**
     * @brief Checks whether the item configuration file being processed contains any recipes at
     * all.
     * @return True if the processed configuration file contains recipes, false otherwise.
     */
    bool doesFileContainsDataNeedForRecipe() const;

    /**
     * @brief Returns the item identifier of the item configuration file currently being processed.
     * @return Item identifier of the item configuration file currently being processed.
     */
    int resultItemId() const;
};