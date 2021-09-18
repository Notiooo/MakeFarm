#pragma once
#include <memory>


#include "ChunkMeshBuilder.h"
#include "World/Block/Block.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/MultiDimensionalArray.h"

class ChunkContainer;

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
	Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, const ChunkContainer& parent);
	Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack);

	Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack, const ChunkContainer& parent);
	Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack);

	Chunk(Chunk&& rhs) noexcept;

	static constexpr unsigned BLOCKS_PER_DIMENSION = 16;
	static constexpr unsigned BLOCKS_PER_WALL	   = BLOCKS_PER_DIMENSION * BLOCKS_PER_DIMENSION;
	static constexpr unsigned BLOCKS_IN_CHUNK	   = BLOCKS_PER_WALL * BLOCKS_PER_DIMENSION;

	static constexpr unsigned CHUNK_WALL_SIZE	   = BLOCKS_PER_DIMENSION * Block::BLOCK_SIZE;
	static constexpr unsigned CHUNK_AREA_SIZE	   = 6 * CHUNK_WALL_SIZE;

	void createMesh();
	void update(const float& deltaTime);
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

	Block& getLocalBlock(const Block::Coordinate& position);

	[[nodiscard]] const Block& getLocalBlock(const Block::Coordinate& localCoordinates) const;

	[[nodiscard]] Block::Coordinate globalToLocalCoordinates(const Block::Coordinate& globalPosition) const;
	[[nodiscard]] Block::Coordinate localToGlobalCoordinates(const Block::Coordinate& position) const;

	[[nodiscard]] bool areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates) const;

protected:
	[[nodiscard]] bool belongsToContainer() const;
	[[nodiscard]] Block& getLocalBlock(const Block::Coordinate& position, const Direction& direction);
	[[nodiscard]] const Block& getLocalBlock(const Block::Coordinate& localCoordinates, const Direction& direction) const;
	[[nodiscard]] Block::Coordinate getBlockPosition(const Block::Coordinate& position, const Direction& direction) const;

private:
	[[nodiscard]] bool faceHasTransparentNeighbor(const Block::Face& face, const Block::Coordinate& blockPos);
	void createBlockMesh(const Block::Coordinate& pos);
	
private:
	Block::Coordinate mChunkPosition;
	const TexturePack& mTexturePack;
	const ChunkContainer* mParentContainer = nullptr;
	ChunkMeshBuilder mMeshBuilder;
	Model3D mModel;

	std::unique_ptr<MultiDimensionalArray<Block, BLOCKS_PER_DIMENSION,
		BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION>> mChunkOfBlocks;
};
