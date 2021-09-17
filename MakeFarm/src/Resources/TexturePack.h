#pragma once
#include "Resources/TextureSheet.h"
#include "Utils/Settings.h"

class TexturePack : public sf::NonCopyable
{
public:
	TexturePack() = default;
	TexturePack(const std::string& texturePackFolder);

	void loadTexturePack(const std::string& texturePackFolder);
	void bind() const;
	
	std::vector<GLfloat> getNormalizedCoordinates(const TextureSheet& block) const;
	sf::IntRect getTextureIntRect(const TextureSheet& block) const;

private:
	sf::Texture textures;
	Settings texturePackSettings;
	
	int blockSize;
	std::string texturePackName;
};
