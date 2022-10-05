#pragma once
#include <future>
#include <shared_mutex>

#include "Chunk.h"
#include "World/Camera.h"

/*
 * TODO: Returning pointers to chunks and blocks should be
 * considered to change as in some cases a weak_ptr could be used.
 * Dangling pointer is a bad pointer! :<
 * In some cases, also need to pay attention to performance.
 * Therefore, this will be done at a later stage.
 */


/**
 * \brief A container of chunks to manage them easily.
 *
 * A chunk container that stores chunks and can also create new
 * chunks inside itself - including on camera. It removes chunks
 * too far from the camera.
 */
class ChunkContainer
{
	friend class Chunk;
public:
	ChunkContainer(const TexturePack& texturePack);

    /**
     * Draws this chunk container to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
	void draw(const Renderer3D& renderer3D, const sf::Shader& shader) const;

    /**
     * \brief Updates the chunkcontainer logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
	void update(const float& deltaTime);

	/**
	 * \brief Number of chunks drawn in one direction from the player
	 */
	static constexpr int WORLD_GENERATION_CHUNK_DISTANCE = 6;

	/**
	 * \brief This is the maximum site height that can be generated.
	 *
	 * For example:
	 * Chunk::BLOCKS_PER_DIMENSION * 10 means that the highest height
	 * that the highest mountain will reach is 10 chunks high
	 */
	static constexpr int MAX_HEIGHT_MAP = Chunk::BLOCKS_PER_DIMENSION * 10;

	/**
	 * \brief Specifies the maximum number of new created chunks at specific height at once.
	 *
	 * This is related to the fact that I don't want to create everything at once.
	 * It takes too long to do this kind of work when creating a lot.
	 * It is much easier to start with the closest ones, and then gradually
	 * in subsequent calls add the missing ones that are further away
	 *
	 * Additionally, as one creation call has a global limit of created chunks at once,
	 * I also want to avoid the situation where only one plane is drawn at a time.
	 */
	static constexpr int MAX_NEW_CHUNKS_PER_HEIGHT_AT_ONCE = 10;


	/**
	 * \brief Specifies the maximum number of new created chunks at once
	 *
	 * This is related to the fact that I don't want to create everything at once.
	 * It takes too long to do this kind of work when creating a lot.
	 * It is much easier to start with the closest ones, and then gradually
	 * in subsequent calls add the missing ones that are further away
	 */
	static constexpr int MAX_NEW_CHUNK_AT_ONCE = MAX_NEW_CHUNKS_PER_HEIGHT_AT_ONCE * WORLD_GENERATION_CHUNK_DISTANCE;

	struct Coordinate final : public CoordinateBase
	{

		using CoordinateBase::CoordinateBase;

        /**
         * Transforms chunk coordinates to game world coordinates.
         * @return Coordinates in the position of the game world
         */
		[[nodiscard]] sf::Vector3i nonChunkMetric() const;

        /**
         * Converts the coordinates of the block to the coordinates of the chunk in
         * which it is located. These coordinates are chunk-specific and
         * independent of the game world coordinates.
         *
         * @param worldBlockCoordinate World coordinates of the block
         * @return Chunk coordinates
         */
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
	[[nodiscard]] const Block* worldBlock(const Block::Coordinate& worldBlockCoordinates) const;

	/**
	 * \brief Returns information about whether a block on a given position has been already created
	 * \param worldBlockCoordinates World coordinates of the block to check
	 * \return True if such a block exists, false otherwise
	 */
	[[nodiscard]] bool doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const;

	
	/**
	 * \brief Converts a block in the indicated position into an air block
	 * \param worldBlockCoordinates World coordinates of the block to change
	 */
	void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates);


	/**
	 * \brief Generates new chunks around the camera
	 * \param camera The camera around which the chunks are to be generated
	 */
	void generateChunksAround(const Camera& camera);


	/**
	 * \briefRemoves chunks too far from the camera
	 * \param camera Camera based on which chunks will be removed
	 */
	void clearFarAwayChunks(const Camera& camera);

private:

	/**
	 * \brief Generates a new chunk at the indicated position in the chunk grid.
	 * \param chunkPosition Position in the chunk grid where the new chunk will be created
	 */
	void generateChunk(ChunkContainer::Coordinate chunkPosition);


	/**
	 * \brief Generates new chunks in and around the specified origin
	 * \param origin A reference point in and around which new chunks will be generated
	 */
	void generatesNewChunksAtAndAroundOrigin(const ChunkContainer::Coordinate& origin);


	/**
	 * \brief Generates a new chunk if one does not exist at this position
	 * \param chunkPosition Position of the chunk in the chunk grid
	 * \return True if a new chunk has been generated, false otherwise
	 */
	bool generateChunkIfNotExist(ChunkContainer::Coordinate chunkPosition);

	/**
	 * \brief Rebuilds important and insignificant chunks and updates their meshes
	 */
	void rebuildChunks();

	/**
	 * \brief Rebuilds and updates chunk meshes instantly
	 */
	void rebuildImportantChunks();

	/**
	 * \brief Rebuilds chunks asynchronously and slowly without updating meshes
	 */
	void rebuildInsignificantChunks();

	/**
	 * \brief Update meshes of slow rebuild chunks
	 */
	void updateInsignificantChunksMeshes();

	/**
	 * \brief Based on the position of the block in the game world, it returns the chunk that contains it.
	 * \param worldBlockCoordinates Block coordinates in the game world
	 * \return Chunk, which contains this block. Nullptr if the block is not present.
	 */
	[[nodiscard]] std::shared_ptr<const Chunk> blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * \brief Based on the position of the block in the game world, it returns the chunk that contains it.
     * \param worldBlockCoordinates Block coordinates in the game world
     * \return Chunk, which contains this block. Nullptr if the block is not present.
     */
	[[nodiscard]] std::shared_ptr<Chunk> blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates);

private:
	const TexturePack& mTexturePack;
	std::unordered_map<ChunkContainer::Coordinate, std::shared_ptr<Chunk>, std::hash<CoordinateBase>> mChunks;
	mutable std::shared_mutex mChunksAccessMutex;
	std::vector<std::shared_ptr<Chunk>> mChunksToRebuildFast;
	std::vector<std::shared_ptr<Chunk>> mChunksToRebuildSlow;
	std::mutex mChunksToRebuildFastAccessMutex;
	std::mutex mChunksToRebuildSlowAccessMutex;
	std::list<std::future<std::vector<std::shared_ptr<Chunk>>>> mChunkCreateMeshProcessesSlow;
};
