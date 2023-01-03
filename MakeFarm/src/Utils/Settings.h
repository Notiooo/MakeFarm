#pragma once
#include <fstream>
#include <sstream>


/**
 * \brief A class used to read game settings
 */
class Settings : sf::NonCopyable
{
public:
    Settings() = default;
    Settings(const std::string& fileName);
    virtual ~Settings() = default;

    /**
     * Opens game settings file
     * @param fileName Name of the settings file
     */
    void openFile(const std::string& fileName);

    /**
     * Closes current game settings file
     */
    void closeFile();

    /**
     * Reads the value of the setting under the specified name from the currently opened settings
     * file. If there is no such setting, it throws an error.
     * @tparam T Type of value hiding under a given setting
     * @param settingName The name of the setting to read from the settings file
     * @return The value hiding under the given setting name in the currently open settings file
     */
    template<typename T>
    T get(const std::string& settingName) const;

    /**
     * Checks whether a given setting is available in the currently open file.
     * @param settingName The name of the setting to read from the settings file
     * @return The value hiding under the given setting name in the currently open settings file
     */
    bool isPresent(const std::string& settingName) const;

    /**
     * Writes a new setting with the specified name and specified value to the currently open
     * settings file.
     * @tparam T The type of value that will be written to the file
     * @param settingName The name of the setting to write to the settings file
     * @param value Value for a given setting name
     */
    template<typename T>
    void set(const std::string& settingName, const T& value);

protected:
    /**
     * @brief It converts the result into the type indicated to it.
     * @tparam T The type to which the setting entry is to be converted
     * @param settingResult Variable to be converted to another type
     * @return The result of the setting in the form of a variable type equal to T
     */
    template<typename T>
    T parseResult(const std::string& settingResult) const;

    /**
     * @brief If a file has been read before, to read it from the beginning you have to reset it,
     * which this function does.
     */
    void prepareToReadFromFstreamAgain() const;

    std::string mSettingsFilename;
    mutable std::ifstream mSettingsFile;
};


template<typename T>
T Settings::get(const std::string& settingName) const
{
    if (mSettingsFilename.empty())
    {
        throw std::logic_error("Trying to get content of file of unknown/closed file");
    }

    prepareToReadFromFstreamAgain();

    std::string fileLine;
    bool stopParsing = false;
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

template<typename T>
void Settings::set(const std::string& settingName, const T& value)
{
    if (mSettingsFilename.empty())
    {
        throw std::logic_error("Trying to get content of file of unknown/closed file");
    }

    // Check if the setting is present there
    std::ofstream settingsFileWrite("temp-" + mSettingsFilename);

    bool found = false;

    std::string fileLine;
    while (std::getline(mSettingsFile, fileLine))
    {
        if (fileLine.rfind(settingName, 0) == 0)
        {
            settingsFileWrite << settingName << " = " << std::to_string(value) << "\n";
            found = true;
        }
        else
        {
            settingsFileWrite << fileLine << "\n";
        }
    }

    if (!found)
    {
        settingsFileWrite << settingName << " = " << std::to_string(value) << "\n";
    }

    settingsFileWrite.close();
    closeFile();

    std::remove(mSettingsFilename.c_str());
    std::rename(("temp-" + mSettingsFilename).c_str(), mSettingsFilename.c_str());

    openFile(mSettingsFilename);
}

template<typename T>
T Settings::parseResult(const std::string& settingResult) const
{
    throw std::logic_error("Unknown setting type for: " + settingResult + "!");
}

template<>
inline int Settings::parseResult(const std::string& settingResult) const
{
    return std::stoi(settingResult);
}

template<>
inline unsigned Settings::parseResult(const std::string& settingResult) const
{
    return std::stoul(settingResult);
}

template<>
inline float Settings::parseResult(const std::string& settingResult) const
{
    return std::stof(settingResult);
}

template<>
inline double Settings::parseResult(const std::string& settingResult) const
{
    return std::stod(settingResult);
}

template<>
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
                       [](const auto& character)
                       {
                           return std::tolower(character);
                       });

        if (basicString == "true")
        {
            return true;
        }
        else if (basicString == "false")
        {
            return false;
        }

        throw std::logic_error("Wrong boolean provided: " + settingResult);
    }
}

template<>
inline std::string Settings::parseResult(const std::string& settingResult) const
{
    return settingResult;
}
