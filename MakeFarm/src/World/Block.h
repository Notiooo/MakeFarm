#pragma once
#include <memory>

#include "Renderer3D/IndexBuffer.h"
#include "Renderer3D/Model3D.h"
#include "Renderer3D/Renderer3D.h"
#include "Renderer3D/VertexArray.h"
#include "Resources/BlockList.h"
#include "Resources/TexturePack.h"


class Block
{
public:
	Block(const TexturePack& texturePack);
	bool isActive();
	void setActive(bool active);

	// For test purposes
	void generateCube(unsigned x, unsigned y, unsigned z);

	
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

	static constexpr float BLOCK_SIZE = 100.f;

private:
	bool active;
	BlockList blockType;
	
	Model3D model;
	Mesh3D mesh;
	
	const TexturePack& texturePack;
};
