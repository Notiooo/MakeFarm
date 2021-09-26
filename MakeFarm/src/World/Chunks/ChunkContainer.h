#pragma once
#include <future>

#include "Chunk.h"
#include "World/Camera.h"

/*
 * This class is so far a resource-intensive bottleneck
 * that does not scale well. For the time being it is
 * used in the form of enabling further development,
 * but it must be optimized!
 *
 * I was thinking of an unordered_map that could collapse
 * chunk positions to slightly smaller numbers based on the
 * CHUNK_SIZE. Then access to those chunks would be pretty easy.
 *
 * TODO: Returning pointers to chunks and blocks should be
 * considered to change as in some cases a weak_ptr could be used.
 * Dangling pointer is a bad pointer! :<
 * In some cases, also need to pay attention to performance.
 * Therefore, this will be done at a later stage.
 */
class ChunkContainer
{
	friend class Chunk;
public:
	ChunkContainer(const TexturePack& texturePack);

	void draw(const Renderer3D& renderer3D, const sf::Shader& shader) const;
	void rebuildChunks();
	void fixedUpdate(const float& deltaTime);

	static constexpr int WORLD_GENERATION_CHUNK_NUMBER = 4;
	static constexpr int WORLD_GENERATION_RADIUS = WORLD_GENERATION_CHUNK_NUMBER * Chunk::CHUNK_WALL_SIZE;

	struct Coordinate final : public CoordinateBase
	{

		using CoordinateBase::CoordinateBase;

		[[nodiscard]] sf::Vector3i getNonChunkMetric() const;

		static ChunkContainer::Coordinate blockToChunkMetric(const Block::Coordinate& worldBlockCoordinate)
		{

			static auto fastFloor = [](const float& numberToFloor)
			{
				const auto truncated = static_cast<int>(numberToFloor);
				return truncated - (truncated > numberToFloor);
			};

			auto returnVar = ChunkContainer::Coordinate(
				fastFloor(worldBlockCoordinate.x / static_cast<float>(Chunk::BLOCKS_PER_DIMENSION)),
				fastFloor(worldBlockCoordinate.y / static_cast<float>(Chunk::BLOCKS_PER_DIMENSION)),
				fastFloor(worldBlockCoordinate.z / static_cast<float>(Chunk::BLOCKS_PER_DIMENSION))
			);

			// Here there is a huge problem with a certain "edge case". Let's consider a couple of cases:
			// worldBlockCoordinate = {0, -16, 0} -> result: {0, -1, 0}
			// worldBlockCoordinate = {0, -1,  0} -> result: {0, -1, 0}
			// worldBlockCoordinate = {0,  0,  0} -> result: {0,  0, 0}
			// worldBlockCoordinate = {0,  15, 0} -> result: {0,  0, 0}
			// worldBlockCoordinate = {0,  16, 0} -> result: {0,  1, 0}

			return returnVar;
		}
	};

	/**
	 * \brief Finds a block inside a container based on the global position of the block
	 * \param worldBlockCoordinates Global position of the block inside the game world
	 * \return Pointer to block found, or nullptr if not found
	 */
	[[nodiscard]] const Block* getWorldBlock(const Block::Coordinate& worldBlockCoordinates) const;


	[[nodiscard]] bool doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const;
	void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates);
	void generateChunksAround(const Camera& camera);
	void clearFarAwayChunks(const Camera& camera);

private:
	void addChunk(ChunkContainer::Coordinate chunkPosition);
	void addChunkIfNotExist(ChunkContainer::Coordinate chunkPosition);
	[[nodiscard]] std::shared_ptr<const Chunk> blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates) const;
	std::shared_ptr<Chunk> blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates);

private:
	const TexturePack& texturePack;
	std::unordered_map<ChunkContainer::Coordinate, std::shared_ptr<Chunk>, std::hash<CoordinateBase>> chunks;

	mutable std::mutex chunksAccessMutex;
	std::deque<std::shared_ptr<Chunk>> chunksToRebuildQueue;
	std::list<std::future<std::shared_ptr<Chunk>>> chunkCreateMeshProcesses;
};
