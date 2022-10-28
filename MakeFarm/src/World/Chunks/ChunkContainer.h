#pragma once
#include <future>
#include <shared_mutex>

#include "Chunk.h"
#include "World/Camera.h"

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
         * Converts the coordinates of the block to the coordinates of the chunk in
         * which it is located. These coordinates are chunk-specific and
         * independent of the game world coordinates.
         *
         * @param worldBlockCoordinate World coordinates of the block
         * @return Chunk coordinates
         */
        static ChunkContainer::Coordinate blockToChunkMetric(
            const Block::Coordinate& worldBlockCoordinate);
    };

    using Chunks = std::unordered_map<ChunkContainer::Coordinate, std::shared_ptr<Chunk>,
                                      std::hash<CoordinateBase>>;

    ChunkContainer() = default;

    /**
     * Draws this chunk container to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer3D& renderer3D, const sf::Shader& shader) const;

    /**
     * \brief Updates the chunkcontainer logic dependent, or independent of time, every rendered
     * frame. \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

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
     * \brief Based on the position of the block in the game world, it returns the chunk that
     * contains it. \param worldBlockCoordinates Block coordinates in the game world \return Chunk,
     * which contains this block. Nullptr if the block is not present.
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
     * @brief Returns a chunk on a given coordinate.
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

private:
    /**
     * \brief Based on the position of the block in the game world, it returns the chunk that
     * contains it. \param worldBlockCoordinates Block coordinates in the game world \return Chunk,
     * which contains this block. Nullptr if the block is not present.
     */
    [[nodiscard]] std::shared_ptr<const Chunk> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates) const;

private:
    /**
     * @brief Mutex guarding access to chunk resources so that
     * multiple threads don't create access problems.
     */
    mutable std::shared_mutex mChunksAccessMutex;

    /**
     * @brief Unordered map storing chunks inside this container.
     */
    Chunks mData;
};
