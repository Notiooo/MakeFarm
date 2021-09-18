#pragma once
#include "World/Camera.h"


class ChunkContainer;


/**
 * \brief Creates a block pointed to by the current camera inside the specified ChunkContainer
 */
class FacedBlock
{
public:
	FacedBlock(const TexturePack& texturePack);

	[[nodiscard]] bool isAnyBlockSelected() const;
	void markFacedBlock(const Camera& camera, const ChunkContainer& chunkContainer);
	Block::Coordinate& getBlockPosition();

	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

private:
	std::unique_ptr<Model3D> mSelectedBlock;
	Block::Coordinate mSelectedBlockPosition{ 0,0,0 };

	// TODO: Later on this class should not use any textures
	const TexturePack& mTexturePack;
};
