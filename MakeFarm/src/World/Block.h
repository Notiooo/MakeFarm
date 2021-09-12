#pragma once
#include <memory>

#include "Renderer3D/IndexBuffer.h"
#include "Renderer3D/Renderer3D.h"
#include "Renderer3D/VertexArray.h"

enum class BlockType
{
	Air = 0,
	Grass,
	Counter
};

class Block
{
public:
	bool isActive();
	void setActive(bool active);

	// For test purposes
	void generateCube(unsigned x, unsigned y, unsigned z);

	
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

	static const char BLOCK_SIZE = 100;

private:
	bool active;
	BlockType blockType;

	// for test purpoeses
	VertexArray va;
	std::unique_ptr<VertexBuffer> vb;
	std::unique_ptr<IndexBuffer> ib;
};