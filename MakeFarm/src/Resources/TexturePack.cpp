#include "TexturePack.h"
#include "pch.h"


TexturePack::TexturePack(const std::string& texturePackFolder)
{
    loadTexturePack(texturePackFolder);
}

void TexturePack::loadTexturePack(const std::string& texturePackFolder)
{
    mTexturePackSettings.openFile("resources/textures/" + texturePackFolder + "/config.cfg");
    mTexturePackName = mTexturePackSettings.get<std::string>("Name");
    mTextureSize = mTexturePackSettings.get<int>("TextureSize");
    mBlocks.loadFromFile("resources/textures/" + texturePackFolder + "/blocks.png");
    mItems.loadFromFile("resources/textures/" + texturePackFolder + "/items.png");
    mTexturePackSettings.closeFile();

    if ((mBlocks.getSize().x % mTextureSize) && (mBlocks.getSize().y % mTextureSize))
    {
        throw std::logic_error("The texture pack: " + mTexturePackName +
                               "- have improper dimensions of blocks!");
    }

    if ((mItems.getSize().x % mTextureSize) && (mItems.getSize().y % mTextureSize))
    {
        throw std::logic_error("The texture pack: " + mTexturePackName +
                               "- have improper dimensions of items!");
    }
}

void TexturePack::bind(const Spritesheet& textures) const
{
    switch (textures)
    {
        case Spritesheet::Blocks: sf::Texture::bind(&mBlocks); break;
        case Spritesheet::Items: sf::Texture::bind(&mItems); break;
    }
}

std::vector<GLfloat> TexturePack::normalizedCoordinates(Block::TextureId textureId) const
{
    const auto blocksPerRow = mBlocks.getSize().x / mTextureSize;
    const auto sizeOfPixel = 1.0f / static_cast<float>(mBlocks.getSize().x);

    auto left = (textureId % blocksPerRow) * mTextureSize * sizeOfPixel;
    auto right = (((textureId % blocksPerRow) * mTextureSize) + mTextureSize) * sizeOfPixel;
    auto bottom = (textureId / blocksPerRow) * mTextureSize * sizeOfPixel;
    auto top = (((textureId / blocksPerRow) * mTextureSize) + mTextureSize) * sizeOfPixel;

    // clang-format off
    return
    {
        right, top,
        left, top,
        left, bottom,
        right, bottom
    };
    // clang-format on
}

const sf::Texture& TexturePack::texture(const TexturePack::Spritesheet& textures) const
{
    switch (textures)
    {
        case Spritesheet::Blocks: return mBlocks;
        case Spritesheet::Items: return mItems;
    }
}

sf::Vector2i TexturePack::pixelCoordinates(unsigned int textureId) const
{
    assert(mBlocks.getSize() == mItems.getSize());
    const auto texturesPerRow = mBlocks.getSize().x / mTextureSize;

    return {static_cast<int>((textureId % texturesPerRow) * mTextureSize),
            static_cast<int>((((textureId / texturesPerRow) * mTextureSize)))};
}

const sf::Texture& TexturePack::texture(const Item::Render& renderType) const
{
    switch (renderType)
    {
        case Item::Render::Block: return texture(TexturePack::Spritesheet::Blocks);
        case Item::Render::Item: return texture(TexturePack::Spritesheet::Items);
    }
}

sf::Rect<int> TexturePack::textureRect(const Item& item) const
{
    auto coordinatesOfItemTexture = pixelCoordinates(static_cast<unsigned int>(item.textureId()));

    auto textureRect = sf::IntRect(coordinatesOfItemTexture.x, coordinatesOfItemTexture.y,
                                   TEXTURE_SIZE, TEXTURE_SIZE);
    return textureRect;
}