#pragma once
#include "Utils/Settings.h"

class TexturePack : public sf::NonCopyable
{
public:
	TexturePack() = default;
	TexturePack(const std::string& texturePackFolder);

	void loadTexturePack(const std::string& texturePackFolder);
	void bind() const;

	std::vector<GLfloat> getNormalizedCoordinates(int textureId) const;

private:
	sf::Texture textures;
	Settings texturePackSettings;
	
	int blockSize;
	std::string texturePackName;
};
