#pragma once
#include "Utils/Settings.h"
#include "World/Block/Block.h"
#include "World/Item/Item.h"


/**
 * A class that holds the game's textures.
 * Allows to easily read them, or change the file it read from.
 */
class TexturePack : public sf::NonCopyable
{
public:
    TexturePack() = default;
    TexturePack(const std::string& texturePackFolder);

    static constexpr auto TEXTURE_SIZE = 16;

    enum class Spritesheet
    {
        Blocks,
        Items
    };

    /**
     * Allows to load any texture pack from the resources folder
     * @param texturePackFolder Name of the folder with the texture pack
     */
    void loadTexturePack(const std::string& texturePackFolder);

    /**
     * Bind a texture pack for rendering
     */
    void bind(const Spritesheet& textures) const;

    /**
     * Returns normalized texture coordinates that can be passed directly along with the vertices to
     * draw the texture.
     * @param textureId
     * @return
     */
    std::vector<GLfloat> normalizedCoordinates(Block::TextureId textureId) const;

    /**
     * @brief Returns texture coordinates given in pixels
     * @param textureId Texture identifier
     * @return 2D texture coordinates in pixels
     */
    sf::Vector2i pixelCoordinates(unsigned int textureId) const;

    /**
     * @brief Returns the spritesheet for a given texture type
     * @param textures Different type of spritesheet, such as blocks or items.
     * @return Texture, which is a spritesheet.
     */
    const sf::Texture& texture(const Spritesheet& textures) const;

    /**
     * @brief Returns the spritesheet for a given item type
     * @param textures Different type of spritesheet, such as blocks or items.
     * @return Texture, which is a spritesheet.
     */
    const sf::Texture& texture(const Item::Render& renderType) const;

    /**
     * @brief Returns the rect in spritesheet for the given object.
     * @param item The object whose rect in the spritesheet is trying to get.
     * @return Rect for a given item in a spritesheet.
     */
    sf::Rect<int> textureRect(const Item& item) const;

private:
    sf::Texture mBlocks;
    sf::Texture mItems;
    Settings mTexturePackSettings;

    int mTextureSize;
    std::string mTexturePackName;
};
