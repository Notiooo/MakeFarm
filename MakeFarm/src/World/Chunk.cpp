#include "pch.h"
#include "Chunk.h"

Chunk::Chunk()
{
	texture.loadTexturePack("defaultTextures");
	
	for (auto x = 0; x < CHUNK_SIZE; ++x)
	for (auto y = 0; y < CHUNK_SIZE; ++y)
	for (auto z = 0; z < CHUNK_SIZE; ++z)
	{
		chunkOfBlocks[x][y][z] = std::make_unique<Block>(texture);
	}
}

void Chunk::createCube(unsigned x, unsigned y, unsigned z)
{
    chunkOfBlocks[x][y][z]->generateCube(x, y, z);
}

void Chunk::createChunk()
{
	for (auto x = 0; x < CHUNK_SIZE; ++x)
	for (auto y = 0; y < CHUNK_SIZE; ++y)
	for (auto z = 0; z < CHUNK_SIZE; ++z)
	{
		if (!chunkOfBlocks[x][y][z]->isActive())
			continue;

		createCube(x, y, z);
	}
}

void Chunk::update(const float& deltaTime)
{
}

Block& Chunk::getBlock(unsigned x, unsigned y, unsigned z) const
{
	return *chunkOfBlocks[x][y][z];
}

void Chunk::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	texture.bind();
	for (auto x = 0; x < CHUNK_SIZE; ++x)
	for (auto y = 0; y < CHUNK_SIZE; ++y)
	for (auto z = 0; z < CHUNK_SIZE; ++z)
	{
		chunkOfBlocks[x][y][z]->draw(renderer3d, shader);
	}
    //renderer3d.draw(va, *ib, shader);
}
