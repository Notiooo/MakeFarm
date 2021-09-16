#pragma once
#include <fstream>
#include <sstream>


/**
 * \brief A class to read game settings
 *
 * At this point it has been implemented as a singleton,
 * which should change at a later stage of production.
 */
class Settings : sf::NonCopyable
{
public:
	Settings() = default;
	Settings(const std::string& fileName);
	
	void setFile(const std::string& fileName);

	template <typename T>
	T get(const std::string& settingName);

	template <typename T>
	void set(const std::string& settingName, const T& value);

private:
	std::string settingsFilename;

	template <typename T>
	T parseResult(const std::string& settingResult);
};


/*
 * Not very efficient piece of code
 * Code that must be changed at a later stage of production.
 * It separates the relevant parts of the configuration
 *
 * The biggest drawback so far is opening the file over and over again
 */
template <typename T>
T Settings::get(const std::string& settingName)
{
	if (settingsFilename.empty())
		throw std::logic_error("Trying to get content of file of unknown name");
	
	std::ifstream settingsFile(settingsFilename);
	std::string fileLine;
	while(std::getline(settingsFile, fileLine))
	{
		std::stringstream ss(fileLine);
		std::string foundSetting;

		// Now I extract the settingName part
		// settingName = settingValue
		// ^^^^^^^^^^^
		// foundSetting
		std::string word;
		while (ss >> word && word != "=")
			foundSetting += word;

		if (foundSetting == settingName)
		{
			// Now I'm removing white spaces at the beginning
			// and return the settingValue part
			// settingName = settingValue
			//               ^^^^^^^^^^^^
			//               foundSetting
			ss >> std::ws;
			std::getline(ss, foundSetting);
			return parseResult<T>(foundSetting);
		}
	}
	throw std::runtime_error("No such setting found: " + settingName);
}

template <typename T>
void Settings::set(const std::string& settingName, const T& value)
{
	// Check if the setting is present there
	std::ofstream settingsFileWrite("temp-" + settingsFilename);
	std::ifstream settingsFileRead(settingsFilename);
	bool found = false;

	std::string fileLine;
	while (std::getline(settingsFileRead, fileLine))
	{
		if(fileLine.rfind(settingName, 0) == 0)
		{
			settingsFileWrite << settingName << " = " << std::to_string(value) << "\n";
			found = true;
		}
		else
		{
			settingsFileWrite << fileLine << "\n";
		}
	}
	
	if(!found)
	{
		settingsFileWrite << settingName << " = " << std::to_string(value) << "\n";
	}
	
	settingsFileWrite.close();
	settingsFileRead.close();

	std::remove(settingsFilename.c_str());
	std::rename(("temp-" + settingsFilename).c_str(), settingsFilename.c_str());
}

template <typename T>
T Settings::parseResult(const std::string& settingResult)
{
	throw std::logic_error("Unknown setting type for: " + settingResult + "!");
}

template <>
inline int Settings::parseResult(const std::string & settingResult)
{
	return std::stoi(settingResult);
}

template <>
inline float Settings::parseResult(const std::string& settingResult)
{
	return std::stof(settingResult);
}

template <>
inline double Settings::parseResult(const std::string& settingResult)
{
	return std::stod(settingResult);
}

template <>
inline std::string Settings::parseResult(const std::string& settingResult)
{
	return settingResult;
}
