#include "pch.h"
#include "Settings.h"

Settings::Settings(const std::string& fileName)
	: settingsFilename(fileName)
{
	openFile(fileName);
}

void Settings::openFile(const std::string& fileName)
{
	settingsFilename = fileName;
	settingsFile.open(settingsFilename);

	if(!settingsFile)
		throw std::logic_error("Opening a file: " + fileName + " has failed");
}

void Settings::closeFile()
{
	settingsFile.close();
	settingsFilename = "";
	settingsFile.clear();
}

bool Settings::isPresent(const std::string& settingName) const
{
	if (settingsFilename.empty())
		throw std::logic_error("Trying to get content of file of unknown/closed name");

	// It set up fstream to be possible to read it again
	settingsFile.clear();
	settingsFile.seekg(0);

	std::string fileLine;
	while (std::getline(settingsFile, fileLine))
	{
		std::stringstream ss(fileLine);
		std::string foundSetting;

		std::string word;
		while (ss >> word && word != "=")
			foundSetting += word;

		if (foundSetting == settingName)
		{
			return true;
		}
	}
	return false;
}
