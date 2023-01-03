#include "Settings.h"
#include "pch.h"

Settings::Settings(const std::string& fileName)
    : mSettingsFilename(fileName)
{
    openFile(fileName);
}

void Settings::openFile(const std::string& fileName)
{
    mSettingsFilename = fileName;
    mSettingsFile.open(mSettingsFilename);

    if (!mSettingsFile)
    {
        throw std::logic_error("Opening a file: " + fileName + " has failed");
    }
}

void Settings::closeFile()
{
    mSettingsFile.close();
    mSettingsFilename = "";
    mSettingsFile.clear();
}

bool Settings::isPresent(const std::string& settingName) const
{
    if (mSettingsFilename.empty())
    {
        throw std::logic_error("Trying to get content of file of unknown/closed name");
    }

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

        bool stopParsing = false;
        if (foundSetting == "Crafting")
        {
            stopParsing = true;
        }
        else if (foundSetting == "EndCrafting")
        {
            stopParsing = false;
        }


        if (!stopParsing && foundSetting == settingName)
        {
            return true;
        }
    }
    return false;
}

void Settings::prepareToReadFromFstreamAgain() const
{
    mSettingsFile.clear();
    mSettingsFile.seekg(0);
}
