#pragma once
#include "BlockList.h"
#include "Utils/Settings.h"

enum class Coordinates
{
	Pixels,
	Normalized
};

class TexturePack : public sf::NonCopyable
{
public:
	TexturePack() = default;
	TexturePack(const std::string& texturePackFolder);

	void loadTexturePack(const std::string& texturePackFolder);
	void bind() const;
	
	std::vector<GLfloat> getNormalizedCoordinates(const BlockList& block) const;
	sf::IntRect getTextureIntRect(const BlockList& block) const;

private:
	sf::Texture textures;
	Settings texturePackSettings;
	
	int blockSize;
	std::string texturePackName;
};
