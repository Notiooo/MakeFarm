#pragma once
#include <memory>


#include "ChunkMeshBuilder.h"
#include "World/Block/Block.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/MultiDimensionalArray.h"

enum class Direction
{
	Above,
	Below,
	ToTheLeft,
	ToTheRight,
	InFront,
	Behind
};

class Chunk
{
	friend class ChunkMeshBuilder;
public:
	Chunk(const TexturePack& texturePack);
	static const int BLOCKS_PER_DIMENSION = 16;
	static const int BLOCKS_PER_WALL = BLOCKS_PER_DIMENSION * BLOCKS_PER_DIMENSION;
	static const int BLOCKS_IN_CHUNK = BLOCKS_PER_WALL * BLOCKS_PER_DIMENSION;
	
	static const int CHUNK_WALL_SIZE = BLOCKS_PER_WALL * Block::BLOCK_SIZE;
	static const int CHUNK_AREA_SIZE = 6 * CHUNK_WALL_SIZE;
	static const int CHUNK_VOLUME_SIZE = CHUNK_WALL_SIZE * CHUNK_WALL_SIZE * CHUNK_WALL_SIZE;

	void createCube(const sf::Vector3i& position);
	void createChunk();
	void update(const float& deltaTime);
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

	Block& getBlock(const sf::Vector3i& position) const;
	bool isPositionInsideChunk(const sf::Vector3i& position) const;
	Block& getBlock(const sf::Vector3i& position, const Direction& direction) const;
	sf::Vector3i getBlockPosition(const sf::Vector3i& position, const Direction& direction) const;


private:
	bool faceHasNeighbor(Block::Face face, const sf::Vector3i& blockPos);
	
private:
	sf::Vector3i position = {0, 0, 0};
	const TexturePack& texturePack;
	ChunkMeshBuilder meshBuilder;
	MultiDimensionalArray<std::unique_ptr<Block>,
		BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION> chunkOfBlocks;
	
	Model3D model;
};
