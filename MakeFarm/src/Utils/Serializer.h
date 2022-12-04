#pragma once
#include <filesystem>
#include <fstream>
#include <iterator>
#include <serializer.h>

/**
 * @brief The class deals with converting variables from specific classes to writing their state to
 * a file for later reading
 */
class Serializer
{
public:
    /**
     * @brief Saves the serialized data to a file.
     * @param pathToFile The file to which the binary data should be written.
     */
    void saveToFile(const std::string& pathToFile);

    /**
     * @brief Serializes passed data by converting it to byte form
     * @tparam Ts Types of data to be serialized
     * @param args Data to be serialized
     */
    template<typename... Ts>
    void serialize(Ts&&... args);

    /**
     * @brief Reads the serialized data from the file
     * @tparam Ts Types of variables to which the serialized data are to be read
     * @param file The file from which the binary data should be read.
     * @param args Variables to which the serialized data are to be read
     */
    template<typename... Ts>
    void readSerialized(std::ifstream& file, Ts&... args);

private:
    std::vector<unsigned char> mSerializedData;
};

template<typename... Ts>
void Serializer::serialize(Ts&&... args)
{
    zpp::serializer::memory_output_archive out(mSerializedData);
    out(std::forward<Ts>(args)...);
}

template<typename... Ts>
void Serializer::readSerialized(std::ifstream& file, Ts&... args)
{
    std::istreambuf_iterator<char> iter(file);
    std::vector<char> vec(iter, std::istreambuf_iterator<char>{});
    std::vector<unsigned char> data(vec.begin(), vec.end());
    zpp::serializer::memory_input_archive in(data);
    in(std::forward<Ts>(args)...);
}