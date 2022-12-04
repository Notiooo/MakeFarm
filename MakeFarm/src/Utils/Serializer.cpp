#include "Serializer.h"
#include "pch.h"

void Serializer::saveToFile(const std::string& pathToFile)
{
    auto pathWithoutFile = pathToFile.substr(0, pathToFile.find_last_of("\\/"));
    std::filesystem::create_directories(pathWithoutFile);
    std::ofstream outfile(pathToFile, std::ios::out | std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(mSerializedData.data()),
                  mSerializedData.size() * sizeof(char));
    outfile.close();
}