#include "BlockMap.h"
#include "Utils/Settings.h"
#include "pch.h"
#include <filesystem>


const BlockMap& BlockMap::blockMap()
{
    static const BlockMap instance;

    return instance;
}

const BlockType& BlockMap::blockType(const BlockId& blockId) const
{
    return mBlockMap.at(blockId);
}

BlockMap::BlockMap()
{
    parseDirectory("resources/blocks");
}

void BlockMap::parseDirectory(const std::string& directoryName)
{
    Settings settingsFile;
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
                readBlockValues(settingsFile, fileName);
                settingsFile.closeFile();
            }
        }
    }
}

void BlockMap::readBlockValues(const Settings& settingsFile, const std::string& fileName)
{
    BlockType blockType;
    blockType.fileName = fileName;
    blockType.id = readBlockIdentifier(settingsFile);
    blockType.name = readBlockName(settingsFile);
    readBlockTextures(settingsFile, blockType);
    blockType.transparent = readBlockTransparency(settingsFile);
    blockType.collidable = readBlockCollidability(settingsFile);
    blockType.dropItemId = readBlockDropItemId(settingsFile);

    mBlockMap[blockType.id] = blockType;
}

std::optional<ItemId> BlockMap::readBlockDropItemId(const Settings& settingsFile) const
{
    if (settingsFile.isPresent("DropItemId"))
    {
        return static_cast<ItemId>(settingsFile.get<int>("DropItemId"));
    }
    return std::nullopt;
}

BlockId BlockMap::readBlockIdentifier(const Settings& settingsFile) const
{
    if (settingsFile.isPresent("Id"))
    {
        return static_cast<BlockId>(settingsFile.get<int>("Id"));
    }
    throw std::runtime_error("Block does not have an id");
}

bool BlockMap::readBlockCollidability(const Settings& settingsFile) const
{
    bool isBlockCollidable = true;
    if (settingsFile.isPresent("Collidable"))
    {
        isBlockCollidable = settingsFile.get<bool>("Collidable");
    }
    return isBlockCollidable;
}

bool BlockMap::readBlockTransparency(const Settings& settingsFile) const
{
    bool isBlockTransparent = false;
    if (settingsFile.isPresent("Transparent"))
    {
        isBlockTransparent = settingsFile.get<bool>("Transparent");
    }
    return isBlockTransparent;
}

void BlockMap::readBlockTextures(const Settings& settingsFile, BlockType& blockType) const
{
    readBlockTexture(settingsFile, blockType);
    readBlockSideTexture(settingsFile, blockType);
    readCustomBlockFacesTextures(settingsFile, blockType);
}

void BlockMap::readCustomBlockFacesTextures(const Settings& settingsFile,
                                            BlockType& blockType) const
{
    auto setIfPresent =
        [&settingsFile, &blockType](const std::string& setting, const Block::Face& blockFace)
    {
        if (settingsFile.isPresent(setting))
        {
            blockType.textureId[blockFace] = settingsFile.get<Block::TextureId>(setting);
        }
    };

    // Set other texture faces of blocks (optional)
    setIfPresent("TextureLeft", Block::Face::Left);
    setIfPresent("TextureRight", Block::Face::Right);
    setIfPresent("TextureTop", Block::Face::Top);
    setIfPresent("TextureBottom", Block::Face::Bottom);
    setIfPresent("TextureFront", Block::Face::Front);
    setIfPresent("TextureBack", Block::Face::Back);
}

void BlockMap::readBlockSideTexture(const Settings& settingsFile, BlockType& blockType) const
{
    if (settingsFile.isPresent("TextureSide"))
    {
        auto sideTexture = settingsFile.get<Block::TextureId>("TextureSide");
        blockType.textureId[Block::Face::Left] = sideTexture;
        blockType.textureId[Block::Face::Right] = sideTexture;
        blockType.textureId[Block::Face::Front] = sideTexture;
        blockType.textureId[Block::Face::Back] = sideTexture;
    }
}

void BlockMap::readBlockTexture(const Settings& settingsFile, BlockType& blockType) const
{
    if (settingsFile.isPresent("Texture"))
    {
        auto generalTexture = settingsFile.get<Block::TextureId>("Texture");
        for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
        {
            blockType.textureId[static_cast<Block::Face>(i)] = generalTexture;
        }
    }
}

std::string BlockMap::readBlockName(const Settings& settingsFile) const
{
    std::string blockName = "Unnamed Block";
    if (settingsFile.isPresent("Name"))
    {
        blockName = settingsFile.get<std::string>("Name");
    }
    return blockName;
}

void BlockMap::removeCfgExtension(std::string& fileName) const
{
    fileName.erase(fileName.cend() - 4, fileName.cend());
}

bool BlockMap::isFileCfgExtension(std::string& filePath) const
{
    return filePath.size() > 4 && std::string(filePath.end() - 4, filePath.end()) == ".cfg";
}
