#pragma once
#include "World/Camera.h"
#include "World/Chunks/AsyncProcessedObjects.h"
#include "World/Chunks/Chunk.h"
#include "World/Chunks/ChunkContainer.h"

class ChunkManager
{
public:
    /**
     * \brief Number of chunks drawn in one direction from the player
     */
    static constexpr int WORLD_GENERATION_CHUNK_DISTANCE = 10;

    /**
     * \brief Specifies the maximum number of new created chunks at once
     *
     * This is related to the fact that I don't want to create everything at once.
     * It takes too long to do this kind of work when creating a lot.
     * It is much easier to start with the closest ones, and then gradually
     * in subsequent calls add the missing ones that are further away
     */
    static constexpr int MAX_NEW_GENERATED_CHUNK_AT_ONE_THREAD = WORLD_GENERATION_CHUNK_DISTANCE;

    /**
     * \brief This is the maximum site height that can be generated.
     *
     * For example:
     * Chunk::BLOCKS_PER_Y_DIMENSION - 10 means that the highest height
     * that the highest mountain will reach is maximum chunk height - 10
     */
    static constexpr int MAX_HEIGHT_MAP = Chunk::BLOCKS_PER_Y_DIMENSION - 10;

    /**
     * @brief Number of threads responsible for generating new chunks.
     */
    static constexpr int NUMBER_OF_THREADS_GENERATING_NEW_CHUNKS = 1;

    using Chunks = std::list<std::shared_ptr<Chunk>>;

    ChunkManager(const TexturePack& texturePack);

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
     * \brief Generates new chunks around the camera
     * \param camera The camera around which the chunks are to be generated
     */
    void generateChunksAround(const Camera& camera);

    /**
     * \briefRemoves chunks too far from the camera
     * \param camera Camera based on which chunks will be removed
     */
    void clearFarAwayChunks(const Camera& camera);

    /**
     * \brief Indicates chunk as willing to rebuild mesh in near future
     */
    void rebuildSlow(std::shared_ptr<Chunk> chunk);

    /**
     * \brief Indicates chunk as willing to rebuild mesh in this, or next frame
     */
    void rebuildFast(std::shared_ptr<Chunk> chunk);

    /**
     * @brief Returns the chunks that the manager manages.
     * @return Container of chunks.
     */
    const ChunkContainer& chunks() const;

    /**
     * @brief Returns the chunks that the manager manages.
     * @return Container of chunks.
     */
    ChunkContainer& chunks();

private:
    /**
     * @brief Determines the priority at which chunks may be rebuilt.
     */
    enum class RebuildPriority
    {
        High,
        Low
    };

    /**
     * @brief Rebuilds chunks intended to be slowly rebuilt in the background.
     */
    Chunks rebuildSlowChunks(Chunks&& chunks);

    /**
     * @brief Rebuilds chunks intended to be fast rebuilt.
     */
    Chunks rebuildFastChunks(Chunks&& chunks);

    /**
     * @brief Rebuilds at least once the chunks passed
     */
    Chunks rebuildNotProcessedChunks(Chunks&& chunks, const RebuildPriority& rebuildSpeed);

    /**
     * @brief Deletes chunks intended for removal.
     * @param coordinateOfChunksToDelete Chunks to be removed.
     */
    void clearChunks(std::vector<ChunkContainer::Coordinate>&& coordinateOfChunksToDelete);

    /**
     * \brief Generates a new chunk at the indicated position in the chunk grid.
     * \param chunkPosition Position in the chunk grid where the new chunk will be created
     */
    void generateChunk(ChunkContainer::Coordinate chunkPosition);

    /**
     * \brief Generates new chunks in and around the specified origin
     * \param origin A reference point in and around which new chunks will be generated
     */
    std::list<ChunkContainer::Coordinate> generatesNewChunksAtAndAroundOrigin(
        const ChunkContainer::Coordinate& origin);

    /**
     * \brief Generates a new chunk if one does not exist at this position
     * \param chunkCoordinates Position of the chunk in the chunk grid
     * \return True if a new chunk has been generated, false otherwise
     */
    bool generateChunkIfNotExist(const ChunkContainer::Coordinate& chunkCoordinates);

    /**
     * \brief Rebuilds important and insignificant chunks and updates their meshes
     */
    void rebuildChunks();

    /**
     * \brief Rebuilds chunk meshes as fast as it is possible without updating meshes.
     */
    void rebuildImportantChunks();

    /**
     * \brief Rebuilds chunks asynchronously and slowly without updating meshes
     */
    void rebuildInsignificantChunks();

    /**
     * @brief Updates meshes for chunks in the queue whose rebuild has ended.
     * @tparam T The type of object that is stored by the asynchronous queue of processed objects.
     * @param processedObjects Asynchronous queue of processed objects.
     */
    template<typename T>
    void updateChunksMeshes(AsyncProcessedObjects<T>& processedObjects);

    /**
     * @brief Checks if a given chunk is currently being processed.
     * @param chunkPosition Chunk position to be checked.
     * @return True, if the chunk is processed. False otherwise
     */
    bool isChunkDuringProcessing(const ChunkContainer::Coordinate& chunkPosition);

    /**
     * @brief Checks whether the specified chunk is in the container.
     * @param chunkPosition Chunk position to be checked.
     * @return True, if the chunk is inside container. False otherwise
     */
    bool isChunkPresentInContainer(const ChunkContainer::Coordinate& chunkPosition);

    /**
     * @brief Updates the mesh of each chunk from the specified container.
     * @param chunks Container of chunks that should be updated.
     */
    void updateMeshForEveryChunkWhichIsNotDuringGeneration(Chunks& chunks);

    /**
     * @brief Updates the mesh of each chunk from the specified container.
     * @param chunkCoordinates Container of chunks that should be updated.
     */
    void updateMeshForEveryChunkWhichIsNotDuringProcessing(
        std::list<ChunkContainer::Coordinate>& chunkCoordinates);

    /**
     * @brief Into the asynchronous object queue it drops a new object for reprocessing.
     * If the object is already there, it is moved to the end of the queue.
     * @param asyncProcessedObjects Asychronous queue of processed objects.
     * @param chunk Chunk that should be moved or pushed.
     */
    void pushChunkToTheBackOfObjectsToBeProcessedOrMoveToTheBackIfExists(
        AsyncProcessedObjects<std::shared_ptr<Chunk>>& asyncProcessedObjects,
        const std::shared_ptr<Chunk>& chunk) const;

    /**
     * @brief Creates new threads that generate new chunks.
     * @param currentChunkOfCamera The location of the camera around which new chunks should be
     * generated.
     */
    void createThreadsGeneratingNewChunks(const ChunkContainer::Coordinate& currentChunkOfCamera);

    /**
     * @brief Update the mesh for any chunk that is not being processed and whose rebuilt thread has
     * ended.
     */
    void processFinishedThreadsGeneratingNewChunks();

    /**
     * @brief Updates first the meshes of chunks that need to be rebuilt shbk. If there are no such,
     * it updates those for slow updates.
     */
    void updateChunkMeshes();

    /**
     * @brief Moves the chunk to the back of the list
     * @param chunks List of chunks
     * @param chunk The chunk iterator that should be moved
     */
    void moveChunkToTheBackOfList(std::list<std::shared_ptr<Chunk>>& chunks,
                                  std::list<std::shared_ptr<Chunk>>::iterator& chunk) const;

    /**
     * @brief Rebuilds the chunk at least once. If the Chunk is occupied then waits for it.
     * @param chunk Chunk to be rebuilt
     * @param rebuildSpeed Priority of how important this rebuild is. Higher priority, faster to
     * rebuild
     */
    void rebuildChunkAtLeastOnce(const std::shared_ptr<Chunk>& chunk,
                                 const RebuildPriority& rebuildSpeed);

    /**
     * @brief Attempts to rebuild chunk, but does not do so if Chunk is occupied
     * @param chunk Chunk to be rebuilt
     * @param rebuildSpeed Priority of how important this rebuild is. Higher priority, faster to
     * rebuild
     * @param chunkCoordinates Chunk coordinates to be rebuilt
     * @return True if the chunk managed to rebuild, false otherwise.
     */
    bool tryToRebuildChunkIfNotProcessed(const std::shared_ptr<Chunk>& chunk,
                                         const RebuildPriority& rebuildSpeed,
                                         const ChunkContainer::Coordinate& chunkCoordinates);

    /** == Members == */
    const TexturePack& mTexturePack;

    /** Chunks */
    ChunkContainer mChunkContainer;
    mutable std::recursive_mutex mChunksAccessMutex;

    /** Asynchronous processed objects */
    AsyncProcessedObjects<std::shared_ptr<Chunk>> mChunkToRebuildFast;
    AsyncProcessedObjects<std::shared_ptr<Chunk>> mChunkToRebuildSlow;
    AsyncProcessedObjects<ChunkContainer::Coordinate> mCurrentlyProcessedChunks;
};

template<typename T>
void ChunkManager::updateChunksMeshes(AsyncProcessedObjects<T>& processedObjects)
{
    /*
     * applyMesh uses the various calls to OpenGL directly.
     * As we know OpenGL is a state machine and it's not very
     * nice to execute it asynchronously, so the buffer swap
     * of chunks is executed synchronously as it's fast anyway
     */

    std::scoped_lock guard(processedObjects.processedObjectsThreadsMutex);
    auto chunkVectorIterator = processedObjects.processedObjectsThreads.begin();
    auto endChunkIterator = processedObjects.processedObjectsThreads.end();
    while (chunkVectorIterator != endChunkIterator)
    {
        if (hasTheThreadFinishedTheJob(*chunkVectorIterator))
        {
            auto chunkVector = chunkVectorIterator->get();
            updateMeshForEveryChunkWhichIsNotDuringGeneration(chunkVector);
            processedObjects.processedObjectsThreads.erase(chunkVectorIterator++);
        }
        else
        {
            ++chunkVectorIterator;
        }
    }
}