#pragma once
#include "Utils/Settings.h"
#include "World/Block/Block.h"

class TexturePack : public sf::NonCopyable
{
public:
	TexturePack() = default;
	TexturePack(const std::string& texturePackFolder);

	void loadTexturePack(const std::string& texturePackFolder);
	void bind() const;

	std::vector<GLfloat> getNormalizedCoordinates(Block::TextureId textureId) const;

private:
	sf::Texture textures;
	Settings texturePackSettings;
	
	int blockSize;
	std::string texturePackName;
};
