#include "RecipeContainer.h"
#include "World/Crafting/RecipeSettings.h"
#include "pch.h"
#include <filesystem>

const RecipeContainer& RecipeContainer::recipeContainer()
{
    static const RecipeContainer instance;

    return instance;
}

const std::vector<Recipe>& RecipeContainer::recipes() const
{
    return mRecipes;
}

RecipeContainer::RecipeContainer()
{
    parseDirectory("resources/items");
}

void RecipeContainer::parseDirectory(const std::string& directoryName)
{
    RecipeSettings settingsFile;
    for (const auto& file: std::filesystem::recursive_directory_iterator(directoryName))
    {
        if (file.is_regular_file())
        {
            std::string filePath = file.path().string();
            if (isFileCfgExtension(filePath))
            {
                std::string fileName = file.path().filename().string();
                removeCfgExtension(fileName);

                settingsFile.openFile(filePath);
                auto recipes = settingsFile.getRecipes();
                mRecipes.insert(mRecipes.end(), recipes.begin(), recipes.end());
                settingsFile.closeFile();
            }
        }
    }
}

void RecipeContainer::removeCfgExtension(std::string& fileName) const
{
    fileName.erase(fileName.cend() - 4, fileName.cend());
}

bool RecipeContainer::isFileCfgExtension(std::string& filePath) const
{
    return filePath.size() > 4 && std::string(filePath.end() - 4, filePath.end()) == ".cfg";
}
