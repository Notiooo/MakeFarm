#pragma once
#include "BlockType.h"
#include "Utils/Settings.h"

/**
 * A map containing access to information about the blocks in the game.
 */
class BlockMap
{
public:
    /**
     * Returns an instance of the block map
     * @return Instance of the block map
     */
    const static BlockMap& blockMap();

    /**
     * Returns the type of block, which is information about the block
     * @param blockId The id of the block
     * @return Block information set
     */
    [[nodiscard]] const BlockType& blockType(const BlockId& blockId) const;

private:
    BlockMap();

    /**
     * It searches the folder for block settings.
     * @param directoryName Path to folder in search of blocks
     */
    void parseDirectory(const std::string& directoryName);

    /**
     * @brief Checks if the file has a .cfg extension
     * @param filePath Path to the file
     * @return True if file has extension .cfg, false otherwise
     */
    bool isFileCfgExtension(std::string& filePath) const;

    /**
     * @brief Removes the cfg extension from the character string
     * @param fileName The character string from which we want to remove the cfg extension
     */
    void removeCfgExtension(std::string& fileName) const;

    /**
     * @brief Reads the block name from the settings
     * @param settingsFile File containing block settings
     * @return Block name
     */
    [[nodiscard]] std::string readBlockName(const Settings& settingsFile) const;

    /**
     * @brief For all the walls of the block, it reads the correct textures for it.
     * @param settingsFile File containing block settings.
     * @param blockType Block for which textures are read
     */
    void readBlockTextures(const Settings& settingsFile, BlockType& blockType) const;

    /**
     * @brief Reads the basic texture of the block and sets it to all faces of the block
     * @param settingsFile File containing block settings
     * @param blockType Block for which textures are read
     */
    void readBlockTexture(const Settings& settingsFile, BlockType& blockType) const;

    /**
     * @brief Reads the side texture of a block and sets it to its side walls
     * @param settingsFile File containing block settings
     * @param blockType Block for which textures are read
     */
    void readBlockSideTexture(const Settings& settingsFile, BlockType& blockType) const;

    /**
     * @brief Reads specifically given block faces and sets them on the block
     * @param settingsFile File containing block settings
     * @param blockType Block for which textures are read
     */
    void readCustomBlockFacesTextures(const Settings& settingsFile, BlockType& blockType) const;

    /**
     * @brief Reads if the block is transparent
     * @param settingsFile File containing block settings
     * @return True if transparent, false otherwise
     */
    [[nodiscard]] bool readBlockTransparency(const Settings& settingsFile) const;

    /**
     * @brief Reads if the block is collidable
     * @param settingsFile File containing block settings
     * @return True if collidable, false otherwise
     */
    [[nodiscard]] bool readBlockCollidability(const Settings& settingsFile) const;

    /**
     * @brief Reads block identifier
     * @param settingsFile File containing block settings
     * @return Block identifier
     */
    [[nodiscard]] BlockId readBlockIdentifier(const Settings& settingsFile) const;

    /**
     * @brief Reads the values of the block and assigns it to the map
     * @param settingsFile File containing block settings
     * @param fileName The name of the file containing the configuration of a given block
     */
    void readBlockValues(const Settings& settingsFile, const std::string& fileName);

    /**
     * @brief Reads the ID of the item that the block is dropping when destroyed
     * @param settingsFile File containing block settings
     * @return This is an optional structure that contains the ID of the object only when the block
     * drops something after destruction
     */
    [[nodiscard]] std::optional<ItemId> readBlockDropItemId(const Settings& settingsFile) const;

    /**
     * Maps filename to specific BlockType
     */
    std::unordered_map<BlockId, BlockType> mBlockMap;
};
