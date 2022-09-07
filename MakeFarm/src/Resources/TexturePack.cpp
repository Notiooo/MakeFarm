#include "pch.h"
#include "TexturePack.h"


TexturePack::TexturePack(const std::string& texturePackFolder)
{
	loadTexturePack(texturePackFolder);
}

void TexturePack::loadTexturePack(const std::string& texturePackFolder)
{
	texturePackSettings.openFile("resources/textures/" + texturePackFolder + "/config.cfg");
	texturePackName = texturePackSettings.get<std::string>("Name");
	blockSize = texturePackSettings.get<int>("TextureSize");
	textures.loadFromFile("resources/textures/" + texturePackFolder + "/textures.png");
	texturePackSettings.closeFile();

	if ((textures.getSize().x % blockSize) && (textures.getSize().y % blockSize))
		throw std::logic_error("The texture pack: " + texturePackName + "- have improper dimensions!");
}

void TexturePack::bind() const
{
	sf::Texture::bind(&textures);
}

std::vector<GLfloat> TexturePack::getNormalizedCoordinates(Block::TextureId textureId) const
{
	const auto blocksPerRow = textures.getSize().x / blockSize;
	const auto sizeOfPixel = 1.0f / static_cast<float>(textures.getSize().x);

	auto left = (textureId % blocksPerRow) * blockSize * sizeOfPixel;
	auto right = (((textureId % blocksPerRow) * blockSize) + blockSize) * sizeOfPixel;
	auto bottom = (textureId / blocksPerRow) * blockSize * sizeOfPixel;
	auto top = (((textureId / blocksPerRow) * blockSize) + blockSize) * sizeOfPixel;

	return
	{
		right, top,
		left, top,
		left, bottom,
		right, bottom
	};
}