#include "pch.h"
#include "TexturePack.h"

TexturePack::TexturePack(const std::string& texturePackFolder)
{
	loadTexturePack(texturePackFolder);
}

void TexturePack::loadTexturePack(const std::string& texturePackFolder)
{
	texturePackSettings.setFile("resources/Textures/" + texturePackFolder + "/config.cfg");
	texturePackName = texturePackSettings.get<std::string>("Name");
	blockSize = texturePackSettings.get<int>("TextureSize");
	textures.loadFromFile("resources/Textures/" + texturePackFolder + "/textures.png");

	if ((textures.getSize().x % blockSize) && (textures.getSize().y % blockSize))
		throw std::logic_error("The texture pack: " + texturePackName + "- have improper dimensions!");
}

void TexturePack::bind() const
{
	sf::Texture::bind(&textures);
}

std::vector<GLfloat> TexturePack::getNormalizedCoordinates(const BlockList& block) const
{
	const auto blocksPerRow = textures.getSize().x / blockSize;
	const auto sizeOfPixel = 1.0f / static_cast<float>(textures.getSize().x);

	auto left = (static_cast<int>(block) % blocksPerRow) * blockSize * sizeOfPixel;
	auto right = (((static_cast<int>(block) % blocksPerRow) * blockSize) + blockSize) * sizeOfPixel;
	auto bottom = (static_cast<int>(block) / blocksPerRow) * blockSize * sizeOfPixel;
	auto top = (((static_cast<int>(block) / blocksPerRow) * blockSize) + blockSize) * sizeOfPixel;

	return
	{
		right, top,
		left, top,
		left, bottom,
		right, bottom
	};
}

sf::IntRect TexturePack::getTextureIntRect(const BlockList& block) const
{
	sf::IntRect textureRect;
	textureRect.width = blockSize;
	textureRect.height = blockSize;
	
	const auto blocksPerRow = textures.getSize().x / blockSize;
	textureRect.left = static_cast<int>(block) % blocksPerRow;
	textureRect.top = static_cast<int>(block) / blocksPerRow;

	return textureRect;
}
