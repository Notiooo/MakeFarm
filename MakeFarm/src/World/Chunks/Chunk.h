#pragma once
#include <future>
#include <memory>
#include <mutex>
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

	/**
	 * \brief Prepares/generates the mesh chunk, but does not replace it yet.
	 */
	void prepareMesh();

	/**
	 * \brief Swaps the current chunk mesh with the latest, most recently generated one
	 */
	void updateMesh();

	void update(const float& deltaTime);
	void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

	/**
	 * \brief Returns the block according to the coordinates given relative to the chunk position.
	 * \param localCoordinates Position in relation to the chunk
	 * \return Block reference inside chunk
	 */
	Block& getLocalBlock(const Block::Coordinate& localCoordinates);

	/**
	 * \brief Removes a block on coordinates given relatively to the position of the chunk
	 * \param localCoordinates Coordinates relative to the position of the chunk
	 */
	void removeLocalBlock(const Block::Coordinate& localCoordinates);

	/**
	 * \brief Returns the block according to the coordinates given relative to the chunk position.
	 * \param localCoordinates Position in relation to the chunk
	 * \return Block reference inside chunk
	 */
	[[nodiscard]] const Block& getLocalBlock(const Block::Coordinate& localCoordinates) const;

	/**
	 * \brief Changes global world coordinates to local ones relative to chunk
	 * \param worldCoordinates World coordinates of the block 
	 * \return Local coordinates of block relative to chunk
	 */
	[[nodiscard]] Block::Coordinate globalToLocalCoordinates(const Block::Coordinate& worldCoordinates) const;

	/**
	 * \brief Changes local chunk coordinates to global ones inside the world
	 * \param localCoordinates Local coordinates of the block
	 * \return World coordinates of block inside the world
	 */
	[[nodiscard]] Block::Coordinate localToGlobalCoordinates(const Block::Coordinate& localCoordinates) const;

	/**
	 * \brief Checks whether the local coordinates relative to the chunk are inside it
	 * \param localCoordinates Local coordinates relative to chunk
	 * \return True if local coordinates are inside the chunk, false otherwise
	 */
	[[nodiscard]] static bool areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates);


	/**
	 * \brief Checks that the given local coordinates relative to the chunk are at its extremity.
	 * \param localCoordinates Local coordinates relative to chunk
	 * \return True if the coordinates are on the edge of the chunk, false otherwise
	 */
	[[nodiscard]] bool isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates);

	/**
	 * \brief Checks whether the chunk is inside any chunk container
	 * \return True if the chunk is inside any chunk container, false otherwise
	 */
	[[nodiscard]] bool belongsToAnyChunkContainer() const;

	/**
	 * \brief Indicates chunk as willing to rebuild mesh in near future
	 */
	void markToBeRebuildSlow() const;

	/**
	 * \brief Indicates chunk as willing to rebuild mesh in this, or next frame
	 */
	void markToBeRebuildFast() const;

protected:
	void generateChunkTerrain();
	void rebuildChunksAround();
	void rebuildMesh();

	std::vector<Direction> getDirectionOfBlockFacesInContactWithOtherChunk(const Block::Coordinate& localCoordinates);
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
	std::unique_ptr<Model3D> mModel;

	std::shared_ptr<ChunkBlocks> mChunkOfBlocks;
	std::recursive_mutex rebuildMeshMutex;
	std::future<void> mChunkTerrainGenerationProcess;
};
