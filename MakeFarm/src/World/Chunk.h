#pragma once
#include <memory>

#include "World/Block.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/MultiDimensionalArray.h"

class Chunk
{
public:
	Chunk();

	void createCube(unsigned x, unsigned y, unsigned z);
	void createChunk();
	void update(const float& deltaTime);
	Block& getBlock(unsigned x, unsigned y, unsigned z) const;
	
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;
	static const int CHUNK_SIZE = 16;
private:

	MultiDimensionalArray<std::unique_ptr<Block>, 16, 16, 16> chunkOfBlocks;
	
	VertexArray va;
	std::unique_ptr<VertexBuffer> vb;
	std::unique_ptr<IndexBuffer> ib;
	
	TexturePack texture;

	int vertexCounter = 0;
};
