#pragma once
#include "Utils/Settings.h"
#include "World/Block/Block.h"


/**
 * A class that holds the game's textures.
 * Allows to easily read them, or change the file it read from.
 */
class TexturePack : public sf::NonCopyable
{
public:
    TexturePack() = default;
    TexturePack(const std::string& texturePackFolder);

    /**
     * Allows to load any texture pack from the resources folder
     * @param texturePackFolder Name of the folder with the texture pack
     */
    void loadTexturePack(const std::string& texturePackFolder);

    /**
     * Bind a texture pack for rendering
     */
    void bind() const;

    /**
     * Returns normalized texture coordinates that can be passed directly along with the vertices to
     * draw the texture.
     * @param textureId
     * @return
     */
    std::vector<GLfloat> normalizedCoordinates(Block::TextureId textureId) const;

private:
    sf::Texture mTextures;
    Settings mTexturePackSettings;

    int mBlockSize;
    std::string mTexturePackName;
};
