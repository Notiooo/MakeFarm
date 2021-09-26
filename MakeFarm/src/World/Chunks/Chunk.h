#pragma once
#include <memory>
#include <queue>


#include "MeshBuilder.h"
#include "Renderer3D/Model3D.h"
#include "World/Block/Block.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/MultiDimensionalArray.h"

class ChunkContainer;

enum class Direction
{
	None,

	Above,
	Below,
	ToTheLeft,
	ToTheRight,
	InFront,
	Behind,

	Counter
};

class Chunk
{
	friend class ChunkContainer;
public:
	Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, ChunkContainer& parent);
	Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack);

	Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack, ChunkContainer& parent);
	Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack);

	Chunk(Chunk&& rhs) noexcept;

	static constexpr int BLOCKS_PER_DIMENSION = 16;
	static constexpr int BLOCKS_PER_WALL	  = BLOCKS_PER_DIMENSION * BLOCKS_PER_DIMENSION;
	static constexpr int BLOCKS_IN_CHUNK	  = BLOCKS_PER_WALL * BLOCKS_PER_DIMENSION;

	static constexpr int CHUNK_WALL_SIZE	  = BLOCKS_PER_DIMENSION * Block::BLOCK_SIZE;
	static constexpr int CHUNK_AREA_SIZE	  = 6 * CHUNK_WALL_SIZE;

	void prepareMesh();
	void updateMesh();

	void update(const float& deltaTime);
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

	Block& getLocalBlock(const Block::Coordinate& position);
	void removeLocalBlock(const Block::Coordinate& localCoordinates);

	[[nodiscard]] const Block& getLocalBlock(const Block::Coordinate& localCoordinates) const;

	[[nodiscard]] Block::Coordinate globalToLocalCoordinates(const Block::Coordinate& globalPosition) const;
	[[nodiscard]] Block::Coordinate localToGlobalCoordinates(const Block::Coordinate& position) const;
	std::vector<Direction> getDirectionOfBlockFacesInContactWithOtherChunk(const Block::Coordinate& localCoordinates);

	[[nodiscard]] static bool areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates);
	[[nodiscard]] bool isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates);

	void markToBeRebuildSlow() const;
	void markToBeRebuildFast() const;

protected:
	void generateChunkTerrain();
	void rebuildChunksAround();
	void rebuildMesh();

	[[nodiscard]] bool belongsToContainer() const;
	[[nodiscard]] Block& getLocalBlock(const Block::Coordinate& position, const Direction& direction);
	[[nodiscard]] const Block& getLocalBlock(const Block::Coordinate& localCoordinates, const Direction& direction) const;
	[[nodiscard]] Block::Coordinate getLocalBlockPosition(const Block::Coordinate& position, const Direction& direction) const;

	[[nodiscard]] std::shared_ptr<Chunk> getChunk(const Direction& direction);

private:
	using ChunkBlocks = MultiDimensionalArray<std::unique_ptr<Block>,
		BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION, BLOCKS_PER_DIMENSION>;

	Chunk(std::shared_ptr<ChunkBlocks> chunkBlocks, Block::Coordinate blockPosition, const TexturePack& texturePack, ChunkContainer& parent);
	[[nodiscard]] bool faceHasTransparentNeighbor(const Block::Face& face, const Block::Coordinate& blockPos);
	void createBlockMesh(const Block::Coordinate& pos);
	
private:
	Block::Coordinate mChunkPosition;
	const TexturePack& mTexturePack;
	ChunkContainer* const mParentContainer;
	MeshBuilder mMeshBuilder;
	Model3D mModel;

	std::shared_ptr<ChunkBlocks> mChunkOfBlocks;
};
