#include "pch.h"
#include "TexturePack.h"


TexturePack::TexturePack(const std::string& texturePackFolder)
{
	loadTexturePack(texturePackFolder);
}

void TexturePack::loadTexturePack(const std::string& texturePackFolder)
{
	mTexturePackSettings.openFile("resources/textures/" + texturePackFolder + "/config.cfg");
    mTexturePackName = mTexturePackSettings.get<std::string>("Name");
    mBlockSize = mTexturePackSettings.get<int>("TextureSize");
	mTextures.loadFromFile("resources/textures/" + texturePackFolder + "/textures.png");
	mTexturePackSettings.closeFile();

	if ((mTextures.getSize().x % mBlockSize) && (mTextures.getSize().y % mBlockSize))
    {
        throw std::logic_error("The texture pack: " + mTexturePackName + "- have improper dimensions!");
    }
}

void TexturePack::bind() const
{
	sf::Texture::bind(&mTextures);
}

std::vector<GLfloat> TexturePack::normalizedCoordinates(Block::TextureId textureId) const
{
	const auto blocksPerRow = mTextures.getSize().x / mBlockSize;
	const auto sizeOfPixel = 1.0f / static_cast<float>(mTextures.getSize().x);

	auto left = (textureId % blocksPerRow) * mBlockSize * sizeOfPixel;
	auto right = (((textureId % blocksPerRow) * mBlockSize) + mBlockSize) * sizeOfPixel;
	auto bottom = (textureId / blocksPerRow) * mBlockSize * sizeOfPixel;
	auto top = (((textureId / blocksPerRow) * mBlockSize) + mBlockSize) * sizeOfPixel;

	return
	{
		right, top,
		left, top,
		left, bottom,
		right, bottom
	};
}