#include "pch.h"
#include "Settings.h"

Settings::Settings(const std::string& fileName)
	: settingsFilename(fileName)
{
	setFile(fileName);
}

void Settings::setFile(const std::string& fileName)
{
	settingsFilename = fileName;
}
