#pragma once
#include "World/Camera.h"
#include "World/Chunks/AsyncProcessedObjects.h"
#include "World/Chunks/ChunkContainer.h"
#include "World/Chunks/ChunkInterface.h"

class ChunkManager
{
public:
    /**
     * \brief Number of chunks drawn in one direction from the player
     * @warning I recommend not changing this option to higher number, as higher values are
     * unverified and the game may behave unstably.
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
     * @brief Number of threads responsible for generating new chunks.
     */
    static constexpr int NUMBER_OF_THREADS_GENERATING_NEW_CHUNKS = 1;

    using Chunks = std::list<std::shared_ptr<ChunkInterface>>;

    ChunkManager(const TexturePack& texturePack, const std::string& savedWorldPath,
                 const int& worldSeed);

    ~ChunkManager();

    /**
     * Draws this chunk container to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param worldRendererShader Shader with the help of which the object should be drawn
     * @param collisionRendererShader Shader responsible for rendering collisions
     */
    void draw(const Renderer3D& renderer3D, const sf::Shader& worldRendererShader) const;

    /**
     * \brief Updates the chunkcontainer logic dependent, or independent of time, every rendered
     * frame. \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime, const Camera& camera);

    /**
     * \brief Generates new chunks around the given position (origin)
     * \param position The position around which the chunks are to be generated
     */
    void generateChunksAround(const glm::vec3& position);

    /**
     * \brief Removes chunks too far from the camera
     * \param position Position (origin) based on which chunks will be removed
     */
    void clearFarAwayChunks(const glm::vec3& position);

    /**
     * \brief Indicates chunk as willing to rebuild mesh in near future
     */
    void rebuildSlow(std::shared_ptr<ChunkInterface> chunk);

    /**
     * \brief Indicates chunk as willing to rebuild mesh in this, or next frame
     */
    void rebuildFast(std::shared_ptr<ChunkInterface> chunk);

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

    /**
     * @brief Forces the termination of all processes before the function returns
     */
    void forceFinishingAllProcesses();

    /**
     * @brief Calculates a player's spawn point
     * @return Spawn point of the player in non-block coordinates.
     */
    sf::Vector3f calculateSpawnPoint();

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
        AsyncProcessedObjects<std::shared_ptr<ChunkInterface>>& asyncProcessedObjects,
        const std::shared_ptr<ChunkInterface>& chunk) const;

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
    void moveChunkToTheBackOfList(
        std::list<std::shared_ptr<ChunkInterface>>& chunks,
        std::list<std::shared_ptr<ChunkInterface>>::iterator& chunk) const;

    /**
     * @brief Rebuilds the chunk at least once. If the Chunk is occupied then waits for it.
     * @param chunk Chunk to be rebuilt
     * @param rebuildSpeed Priority of how important this rebuild is. Higher priority, faster to
     * rebuild
     */
    void rebuildChunkAtLeastOnce(const std::shared_ptr<ChunkInterface>& chunk,
                                 const RebuildPriority& rebuildSpeed);

    /**
     * @brief Attempts to rebuild chunk, but does not do so if Chunk is occupied
     * @param chunk Chunk to be rebuilt
     * @param rebuildSpeed Priority of how important this rebuild is. Higher priority, faster to
     * rebuild
     * @param chunkCoordinates Chunk coordinates to be rebuilt
     * @return True if the chunk managed to rebuild, false otherwise.
     */
    bool tryToRebuildChunkIfNotProcessed(const std::shared_ptr<ChunkInterface>& chunk,
                                         const RebuildPriority& rebuildSpeed,
                                         const ChunkContainer::Coordinate& chunkCoordinates);

    /** == Members == */
    const TexturePack& mTexturePack;
    const std::string& mSavedWorldPath;
    const int& mWorldSeed;

    /** Chunks */
    ChunkContainer mChunkContainer;
    mutable std::recursive_mutex mChunksAccessMutex;

    /** Asynchronous processed objects */
    AsyncProcessedObjects<std::shared_ptr<ChunkInterface>> mChunkToRebuildFast;
    AsyncProcessedObjects<std::shared_ptr<ChunkInterface>> mChunkToRebuildSlow;
    AsyncProcessedObjects<ChunkContainer::Coordinate> mCurrentlyProcessedChunks;

#if DRAW_DEBUG_COLLISIONS
    sf::Shader mWireframeShader;
#endif
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