#pragma once
#include <future>
#include <shared_mutex>

#include "Chunk.h"
#include "Physics/AABB.h"
#include "World/Camera.h"


#define DRAW_DEBUG_COLLISIONS false

#if DRAW_DEBUG_COLLISIONS
    #include <Physics/AABB.h>
#endif

/**
 * \brief A container of chunks to manage them easily.
 *
 * A chunk container that stores chunks and can also create new
 * chunks inside itself - including on camera. It removes chunks
 * too far from the camera.
 */
class ChunkContainer
{
public:
    struct Coordinate final : public CoordinateBase
    {

        using CoordinateBase::CoordinateBase;

        /**
         * Transforms chunk coordinates to game world coordinates.
         * @return Coordinates in the position of the game world
         */
        [[nodiscard]] sf::Vector3i nonChunkMetric() const;

        /**
         * Converts the coordinates of the coordinateInGivenDirection to the coordinates of the
         * chunk in which it is located. These coordinates are chunk-specific and independent of the
         * game world coordinates.
         *
         * @param worldBlockCoordinate World coordinates of the coordinateInGivenDirection
         * @return Chunk coordinates
         */
        static ChunkContainer::Coordinate blockToChunkMetric(
            const Block::Coordinate& worldBlockCoordinate);
    };

    using Chunks = std::unordered_map<ChunkContainer::Coordinate, std::shared_ptr<Chunk>,
                                      std::hash<CoordinateBase>>;

    ChunkContainer() = default;

    /**
     * Draws terrain of chunks in the container to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawTerrain(const Renderer3D& renderer3D, const sf::Shader& shader) const;

    /**
     * Draws liquids of chunks in the container to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawLiquids(const Renderer3D& renderer3D, const sf::Shader& shader) const;

    /**
     * Draws florals of chunks in the container to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawFlorals(const Renderer3D& renderer3D, const sf::Shader& shader) const;

#if DRAW_DEBUG_COLLISIONS
    /**
     * @brief Draws the collisions that have occurred
     * @param renderer3D Renderer3D responsible for drawing collisions to the screen
     * @param wireframeShader Shader displaying collisions
     */
    void drawOccuredCollisions(const Renderer3D& renderer3D,
                               const sf::Shader& wireframeShader) const;
#endif

    /**
     * \brief Updates the chunkcontainer logic dependent, or independent of time, every rendered
     * frame. \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief Finds a block inside a container based on the global position of the
     * coordinateInGivenDirection \param worldBlockCoordinates Global position of the
     * coordinateInGivenDirection inside the game world \return Pointer to
     * coordinateInGivenDirection found, or nullptr if not found
     */
    [[nodiscard]] const Block* worldBlock(const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * \brief Finds a block inside a container based on the global position of the
     * coordinateInGivenDirection \param worldBlockCoordinates Global position of the
     * coordinateInGivenDirection inside the game world \return Pointer to
     * coordinateInGivenDirection found, or nullptr if not found
     */
    [[nodiscard]] Block* worldBlock(const Block::Coordinate& worldBlockCoordinates);

    /**
     * \brief Returns information about whether a coordinateInGivenDirection on a given position has
     * been already created \param worldBlockCoordinates World coordinates of the
     * coordinateInGivenDirection to check \return True if such a coordinateInGivenDirection exists,
     * false otherwise
     */
    [[nodiscard]] bool doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * \brief Converts a coordinateInGivenDirection in the indicated position into an air block
     * \param worldBlockCoordinates World coordinates of the coordinateInGivenDirection to change
     */
    void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates);

    /**
     * \brief Based on the position of the coordinateInGivenDirection in the game world, it returns
     * the chunk that contains it. \param worldBlockCoordinates Block coordinates in the game world
     * \return Chunk, which contains this block. Nullptr if the coordinateInGivenDirection is not
     * present.
     */
    [[nodiscard]] std::shared_ptr<Chunk> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates);

    /**
     * Returns the chunk located in the listed direction from this chunk
     * @param direction Direction next to which the chunk you are looking for is located
     * @return Pointer to chunk found
     */
    [[nodiscard]] std::shared_ptr<Chunk> chunkNearby(const Chunk& baseChunk,
                                                     const Direction& direction);

    /**
     * @brief Rebuilds chunks around a given chunk.
     * @param chunkCoordinates Coordinates chunk around which other chunks should be rebuilt.
     */
    void rebuildChunksAround(ChunkContainer::Coordinate chunkCoordinates);

    /**
     * @brief Checks if the chunk is inside the container.
     */
    bool isChunkPresentInTheContainer(const Chunk& chunk) const;

    /**
     * @brief Returns the map in which the chunks are located.
     */
    Chunks& data();

    /**
     * @brief Returns the map in which the chunks are located.
     */
    const Chunks& data() const;

    /**
     * @brief Finds the given chunk in the data and returns its shared_ptr. Useful in situations
     * where, with multithreading, the given chunk could be deleted and the reference to it would
     * become unsafe.
     * @param chunk Reference for the chunk to look for.
     * @return Shared_ptr to the chunk in the container, or nullptr if the chunk is not in the
     * container.
     */
    std::shared_ptr<Chunk> findChunk(const Chunk& chunk);

    /**
     * @brief Returns a chunk on a given coordinateInGivenDirection.
     * @param chunkCoordinate Coordinate the chunk to get.
     * @return Pointer to chunk with given coordinates
     */
    std::shared_ptr<Chunk> at(const ChunkContainer::Coordinate& chunkCoordinate);

    /**
     * @brief Erases the chunk with the indicated coordinates
     * @param chunkCoordinate Coordinate the chunk to erase.
     * @return
     */
    Chunks::size_type erase(const ChunkContainer::Coordinate& chunkCoordinate);

    /**
     * @brief Constructs a chunk based on the given parameters
     * @tparam Ts Types of parameters of the chunk constructor
     * @param args Parameters of the chunk constructor.
     * @return Returns a pair consisting of an iterator to the inserted element, or the
     * already-existing element if no insertion happened, and a bool denoting whether the insertion
     * took place (true if insertion happened, false if it did not).
     */
    template<class... Ts>
    auto emplace(Ts&&... args)
    {
        std::scoped_lock guard(mChunksAccessMutex);
        return data().emplace(std::forward<Ts>(args)...);
    }

    /**
     * @brief Returns whether a given chunk exists in the container
     * @param chunkPosition Position of the chunk sought
     * @return True if the object is in a container, false otherwise.
     */
    bool isPresent(const ChunkContainer::Coordinate& chunkPosition) const;

    /**
     * @brief It insert a block in a chunk with rebuilding the chunk. If there was another
     * coordinateInGivenDirection then it is overwritten.
     * @param id Block identifier
     * @param worldCoordinate World coordinates of the coordinateInGivenDirection to place
     * @param postPlaceRebuild Optional rebuilding operation that should happen after inserting the
     * block.
     */
    void tryToPlaceBlock(const BlockId& id, Block::Coordinate worldCoordinate,
                         std::vector<BlockId> blocksThatMightBeOverplaced,
                         Chunk::RebuildOperation postPlaceRebuild);

    /**
     * @brief Checks if a given collision box collides with a coordinateInGivenDirection in any
     * chunk contained in the container.
     * @param aabb Collision box to check if it collides with any coordinateInGivenDirection
     * @return True if it collides, false otherwise
     */
    bool doesItCollide(const AABB& aabb) const;

    /**
     * @brief Blocks that the collision box touches and that are not airborne
     * @param aabb Collision box to check if it collides with blocks
     * @return A vector of non-air blocks touched by a collision box.
     */
    std::vector<Block> nonAirBlocksItTouches(const AABB& aabb) const;

private:
    /**
     * \brief Based on the position of the coordinateInGivenDirection in the game world, it returns
     * the chunk that contains it. \param worldBlockCoordinates Block coordinates in the game world
     * \return Chunk, which contains this coordinateInGivenDirection. Nullptr if the block is not
     * present.
     */
    [[nodiscard]] std::shared_ptr<const Chunk> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * @brief It checks if a new chunk appeared that has queued blocks to be inserted instead of air
     * blocks.
     */
    void tryToPlaceScheduledBlocksForPresentChunks();

    /**
     * @brief It checks if a new chunk appeared that has queued blocks to be inserted instead of air
     * blocks.
     */
    void tryToPlaceScheduledBlocksForNewAppearingChunks();

    /**
     * @brief Checks if there is a collision between the collision coordinateInGivenDirection and
     * the specified point
     * @param aabb A collision box that is checked to see if the point lies in it
     * @param nonBlockMetricPoint Point given in non-metric coordinates
     * @return True if there was a collision, false otherwise.
     */
    bool isThereCollisionBetweenBlockAtGivenPoint(const AABB& aabb,
                                                  sf::Vector3f nonBlockMetricPoint) const;

private:
    /**
     * @brief Mutex guarding access to chunk resources so that
     * multiple threads don't create access problems.
     */
    mutable std::shared_mutex mChunksAccessMutex;

    /**
     * @brief Stores information on a coordinateInGivenDirection to appear in one of the newly
     * created chunks in the future
     */
    struct BlockToBePlaced
    {
        ChunkContainer::Coordinate chunkCoordinates;
        BlockId blockid;
        Block::Coordinate worldBlockCoordinates;
        Chunk::RebuildOperation rebuild;
        std::vector<BlockId> blocksThatMightBeOverplaced;
    };

    /**
     * @brief Mutex making sure too many processes don't use the coordinateInGivenDirection queue to
     * insert new blocks into a future chunk instead of air blocks
     */
    mutable std::recursive_mutex mBlockMightBePlacedAccessMutex;

    /**
     * @brief A queue of blocks that might replace air block with new coordinateInGivenDirection in
     * a chunk that has not yet been created.
     */
    std::list<BlockToBePlaced> mBlockMightBePlacedInFutureChunks;

    /**
     * @brief Unordered map storing chunks inside this container.
     */
    Chunks mData;

    /** == Debug == */
#if DRAW_DEBUG_COLLISIONS
    void addCollisionToDebugDraw(const AABB& aabb) const;
    void limitDrawnCollisions() const;
    mutable std::deque<AABB> mOccuredCollisions;
#endif
};
