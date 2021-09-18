#pragma once
#include <fstream>
#include <sstream>


/**
 * \brief A class to read game settings
 */
class Settings : sf::NonCopyable
{
public:
	Settings() = default;
	Settings(const std::string& fileName);
	
	void openFile(const std::string& fileName);
	void closeFile();

	template <typename T>
	T get(const std::string& settingName) const;

	bool isPresent(const std::string& settingName) const;

	template <typename T>
	void set(const std::string& settingName, const T& value);

private:
	std::string settingsFilename;
	mutable std::ifstream settingsFile;

	template <typename T>
	T parseResult(const std::string& settingResult) const;
};


template <typename T>
T Settings::get(const std::string& settingName) const
{
	if (settingsFilename.empty())
		throw std::logic_error("Trying to get content of file of unknown/closed file");

	// It set up fstream to be possible to read it again
	settingsFile.clear();
	settingsFile.seekg(0);

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
	if (settingsFilename.empty())
		throw std::logic_error("Trying to get content of file of unknown/closed file");

	// Check if the setting is present there
	std::ofstream settingsFileWrite("temp-" + settingsFilename);

	bool found = false;

	std::string fileLine;
	while (std::getline(settingsFile, fileLine))
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
	closeFile();

	std::remove(settingsFilename.c_str());
	std::rename(("temp-" + settingsFilename).c_str(), settingsFilename.c_str());

	openFile(settingsFilename);
}

template <typename T>
T Settings::parseResult(const std::string& settingResult) const
{
	throw std::logic_error("Unknown setting type for: " + settingResult + "!");
}

template <>
inline int Settings::parseResult(const std::string & settingResult) const
{
	return std::stoi(settingResult);
}

template <>
inline unsigned Settings::parseResult(const std::string& settingResult) const
{
	return std::stoul(settingResult);
}

template <>
inline float Settings::parseResult(const std::string& settingResult) const
{
	return std::stof(settingResult);
}

template <>
inline double Settings::parseResult(const std::string& settingResult) const
{
	return std::stod(settingResult);
}

template <>
inline bool Settings::parseResult(const std::string& settingResult) const
{
	if (std::isdigit(settingResult[0]))
	{
		return (settingResult == "0" ? false : true);
	}
	else
	{
		auto basicString = std::string();
		std::transform(settingResult.cbegin(), settingResult.cend(), 
			std::back_insert_iterator<std::string>(basicString), 
			[](const auto& character) { return std::tolower(character); });
		if (basicString == "true")
			return true;
		if (basicString == "false")
			return false;

		throw std::logic_error("Wrong boolean provided: " + settingResult);
	}
}

template <>
inline std::string Settings::parseResult(const std::string& settingResult) const
{
	return settingResult;
}
