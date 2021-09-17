#pragma once
#include <memory>

#include "Renderer3D/IndexBuffer.h"
#include "Renderer3D/Model3D.h"
#include "Renderer3D/Renderer3D.h"
#include "Renderer3D/VertexArray.h"
#include "Resources/TextureSheet.h"
#include "Resources/TexturePack.h"

class Block
{
public:
	static constexpr float BLOCK_SIZE = 100.f;
	enum class Face
	{
		Top = 0,
		Left,
		Right,
		Bottom,
		Forward,
		Back,

		Counter
	};
	
	Block(const TexturePack& texturePack);
	bool isActive();
	void setActive(bool active);
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;
	
	TextureSheet getBlockTexture() const;
	
	// For test purposes
	void generateCube(unsigned x, unsigned y, unsigned z);
	// =======

private:
	bool active;
	TextureSheet blockType;

	// For test purposes
	Model3D model;
	Mesh3D mesh;
	const TexturePack& texturePack;
	// =========
};
