#include "RecipeSettings.h"
#include "Utils/message_assert.h"
#include "pch.h"

std::vector<Recipe> RecipeSettings::getRecipes() const
{
    if (mSettingsFilename.empty())
    {
        throw std::logic_error("Trying to get content of file of unknown/closed file");
    }

    if (doesFileContainsDataNeedForRecipe())
    {
        auto idOfResultItem = resultItemId();

        std::vector<ItemId> allRecipesItemsIdMerged;
        std::vector<int> allRecipesAmountMerged;

        parseLines(allRecipesItemsIdMerged, allRecipesAmountMerged);
        assert((allRecipesItemsIdMerged.size() / 9) == allRecipesAmountMerged.size());
        return parseRecipes(idOfResultItem, allRecipesItemsIdMerged, allRecipesAmountMerged);
    }

    return std::vector<Recipe>();
}

int RecipeSettings::resultItemId() const
{
    prepareToReadFromFstreamAgain();
    std::string fileLine;
    while (std::getline(mSettingsFile, fileLine))
    {
        std::stringstream ss(fileLine);
        std::string foundSetting;

        std::string word;
        while (ss >> word && word != "=")
        {
            foundSetting += word;
        }

        if (foundSetting == "Id")
        {
            int idOfBlock;
            ss >> idOfBlock;
            return idOfBlock;
        }
    }
    message_assert(false, "Item Id in item file is not specified");
    return 0;
}

bool RecipeSettings::doesFileContainsDataNeedForRecipe() const
{
    prepareToReadFromFstreamAgain();
    bool isCraftingFound = false;
    bool isIdOfBlockFound = false;
    std::string fileLine;
    while (std::getline(mSettingsFile, fileLine))
    {
        std::stringstream ss(fileLine);
        std::string foundSetting;

        // Now I extract the settingName part
        // settingName = settingValue
        // ^^^^^^^^^^^
        // foundSetting
        std::string word;
        while (ss >> word && word != "=")
        {
            foundSetting += word;
        }

        if (foundSetting == "Id")
        {
            isIdOfBlockFound = true;
        }

        if (foundSetting == "Crafting")
        {
            isCraftingFound = true;
        }
    }
    return isIdOfBlockFound && isCraftingFound;
}

std::vector<Recipe> RecipeSettings::parseRecipes(
    int idOfFinalBlock, const std::vector<ItemId>& allRecipesItemsIdMerged,
    const std::vector<int>& allRecipesAmountMerged) const
{
    auto numberOfRecipes = allRecipesAmountMerged.size();
    std::vector<Recipe> recipes;
    for (int recipeIndex = 0; recipeIndex < numberOfRecipes; ++recipeIndex)
    {
        Recipe recipe;
        for (int row = 0; row < 3; ++row)
        {
            for (int column = 0; column < 3; ++column)
            {
                recipe.layout[column][row] =
                    allRecipesItemsIdMerged[recipeIndex * 9 + row * 3 + column];
            }
        }
        recipe.finalAmount = allRecipesAmountMerged[recipeIndex];
        recipe.finalItem = static_cast<ItemId>(idOfFinalBlock);
        recipes.emplace_back(recipe);
    }
    return recipes;
}

void RecipeSettings::parseLines(std::vector<ItemId>& allRecipesItemsIdMerged,
                                std::vector<int>& allRecipesAmountMerged) const
{
    prepareToReadFromFstreamAgain();
    std::string fileLine;
    while (std::getline(mSettingsFile, fileLine))
    {
        if (fileLine == "Crafting =")
        {
            break;
        }
    }

    auto lineCounter = 1;
    while (std::getline(mSettingsFile, fileLine))
    {
        if (fileLine.empty())
        {
            continue;
        }
        if (fileLine == "EndCrafting =")
        {
            break;
        }
        parseRecipeLine(fileLine, lineCounter, allRecipesItemsIdMerged, allRecipesAmountMerged);
        ++lineCounter;
    }
}

void RecipeSettings::parseRecipeLine(const std::string& line, int lineIndex,
                                     std::vector<ItemId>& allItemIdsInRecipesCombined,
                                     std::vector<int>& allFinalAmountsInRecipesCombined) const
{
    bool isItLineWithAmount = (lineIndex % 4) == 0;
    std::stringstream ss(line);
    if (isItLineWithAmount)
    {
        allFinalAmountsInRecipesCombined.emplace_back(parseFinalAmountValueFromLine(ss));
    }
    else
    {
        parseItemIdValuesFromLineIntoVector(ss, allItemIdsInRecipesCombined);
    }
}

void RecipeSettings::parseItemIdValuesFromLineIntoVector(
    std::stringstream& streamWithValueToParse,
    std::vector<ItemId>& vectorToInsertParsedItemIds) const
{
    int value;
    while (streamWithValueToParse >> value)
    {
        vectorToInsertParsedItemIds.emplace_back(static_cast<ItemId>(value));
    }
}

int RecipeSettings::parseFinalAmountValueFromLine(std::stringstream& streamWithLineToParse) const
{
    std::string setting;
    std::string equalSign;
    auto value = 0;
    if (streamWithLineToParse >> setting >> equalSign >> value)
    {
        assert(setting == "Amount");
        assert(equalSign == "=");
        return value;
    }
    message_assert(false, "Amount for crafting recipe is not specified");
    return 1;
}
