#include "ChunkManager.h"
#include "Resources/TexturePack.h"
#include "Utils/IteratorRanges.h"
#include "World/Chunks/CoordinatesAroundOriginGetter.h"
#include "pch.h"

ChunkManager::ChunkManager(const TexturePack& texturePack, const std::string& savedWorldPath,
                           const int& worldSeed)
    : mTexturePack(texturePack)
    , mSavedWorldPath(savedWorldPath)
    , mWorldSeed(worldSeed)
{
#if DRAW_DEBUG_COLLISIONS
    mWireframeShader.loadFromFile("resources/shaders/WireframeRenderer/VertexShader.shader",
                                  "resources/shaders/WireframeRenderer/GeometryShader.shader",
                                  "resources/shaders/WireframeRenderer/FragmentShader.shader");
#endif
}

void ChunkManager::draw(const Renderer3D& renderer3D, const sf::Shader& worldRendererShader) const
{
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    mChunkContainer.drawTerrain(renderer3D, worldRendererShader);
    mChunkContainer.drawLiquids(renderer3D, worldRendererShader);
    mChunkContainer.drawFlorals(renderer3D, worldRendererShader);
#if DRAW_DEBUG_COLLISIONS
    mChunkContainer.drawOccuredCollisions(renderer3D, mWireframeShader);
#endif
}

void ChunkManager::update(const float& deltaTime, const Camera& camera)
{
#if DRAW_DEBUG_COLLISIONS
    camera.updateViewProjection(mWireframeShader);
#endif
    mChunkContainer.update(deltaTime);
    rebuildChunks();
}

void ChunkManager::rebuildImportantChunks()
{
    mChunkToRebuildFast.processAsync(this, &ChunkManager::rebuildFastChunks);
}

void ChunkManager::rebuildInsignificantChunks()
{
    if (mChunkToRebuildFast.isNoProcessRunning())
    {
        mChunkToRebuildSlow.processAsync(this, &ChunkManager::rebuildSlowChunks);
    }
}

void ChunkManager::rebuildSlow(std::shared_ptr<ChunkInterface> chunk)
{
    pushChunkToTheBackOfObjectsToBeProcessedOrMoveToTheBackIfExists(mChunkToRebuildSlow, chunk);
}

void ChunkManager::pushChunkToTheBackOfObjectsToBeProcessedOrMoveToTheBackIfExists(
    AsyncProcessedObjects<std::shared_ptr<ChunkInterface>>& asyncProcessedObjects,
    const std::shared_ptr<ChunkInterface>& chunk) const
{
    std::scoped_lock guard(asyncProcessedObjects.objectsToBeProcessedMutex);
    auto& objectsToBeProcessed = asyncProcessedObjects.objectsToBeProcessed;
    auto processObjectIterator =
        std::find(objectsToBeProcessed.begin(), objectsToBeProcessed.end(), chunk);
    if (processObjectIterator != objectsToBeProcessed.end())
    {
        moveChunkToTheBackOfList(objectsToBeProcessed, processObjectIterator);
    }
    else
    {
        objectsToBeProcessed.push_back(chunk);
    }
}

void ChunkManager::moveChunkToTheBackOfList(
    std::list<std::shared_ptr<ChunkInterface>>& chunks,
    std::list<std::shared_ptr<ChunkInterface>>::iterator& chunk) const
{
    chunks.splice(chunks.end(), chunks, chunk);
}

void ChunkManager::rebuildFast(std::shared_ptr<ChunkInterface> chunk)
{
    std::scoped_lock guard(mChunkToRebuildFast.objectsToBeProcessedMutex);
    mChunkToRebuildFast.objectsToBeProcessed.push_back(chunk);
}

void ChunkManager::updateMeshForEveryChunkWhichIsNotDuringGeneration(Chunks& chunks)
{
    for (const auto& chunk: chunks)
    {
        auto chunkPosition =
            ChunkContainer::Coordinate::blockToChunkMetric(chunk->positionInBlocks());

        std::unique_lock guard(mCurrentlyProcessedChunks.objectsToBeProcessedMutex);
        if (!isChunkDuringProcessing(chunkPosition))
        {
            mCurrentlyProcessedChunks.objectsToBeProcessed.emplace_back(chunkPosition);
            guard.unlock();

            chunk->updateMesh();
            mCurrentlyProcessedChunks.removeObject(chunkPosition);
        }
    }
}

void ChunkManager::rebuildChunks()
{
    rebuildImportantChunks();
    rebuildInsignificantChunks();
    updateChunkMeshes();
}

void ChunkManager::updateChunkMeshes()
{
    updateChunksMeshes(mChunkToRebuildFast);
    if (mChunkToRebuildFast.isNoProcessRunning())
    {
        updateChunksMeshes(mChunkToRebuildSlow);
    }
}

void ChunkManager::generateChunksAround(const glm::vec3& position)
{
    const auto chunkInThatPosition = ChunkContainer::Coordinate::blockToChunkMetric(
        Block::Coordinate::nonBlockToBlockMetric(sf::Vector3i(position.x, position.y, position.z)));

    createThreadsGeneratingNewChunks(chunkInThatPosition);
    processFinishedThreadsGeneratingNewChunks();
}

void ChunkManager::processFinishedThreadsGeneratingNewChunks()
{
    std::unique_lock guard(mCurrentlyProcessedChunks.processedObjectsThreadsMutex);
    auto& threadsProcessingChunks = mCurrentlyProcessedChunks.processedObjectsThreads;
    auto [threadIterator, endThreadIterator] = iterator_range(threadsProcessingChunks);
    while (threadIterator != endThreadIterator)
    {
        if (hasTheThreadFinishedTheJob(*threadIterator))
        {
            auto chunkVector = threadIterator->get();
            updateMeshForEveryChunkWhichIsNotDuringProcessing(chunkVector);
            threadIterator = mCurrentlyProcessedChunks.removeObject(std::move(threadIterator));
        }
        else
        {
            ++threadIterator;
        }
    }
}

void ChunkManager::createThreadsGeneratingNewChunks(
    const ChunkContainer::Coordinate& currentChunkOfCamera)
{
    std::unique_lock guard(mCurrentlyProcessedChunks.processedObjectsThreadsMutex);
    while (mCurrentlyProcessedChunks.processedObjectsThreads.size() <=
           NUMBER_OF_THREADS_GENERATING_NEW_CHUNKS)
    {
        mCurrentlyProcessedChunks.processedObjectsThreads.emplace_back(
            std::async(std::launch::async, &ChunkManager::generatesNewChunksAtAndAroundOrigin, this,
                       currentChunkOfCamera));
    }
}

void ChunkManager::updateMeshForEveryChunkWhichIsNotDuringProcessing(
    std::list<ChunkContainer::Coordinate>& chunkCoordinates)
{
    for (const auto& chunkPosition: chunkCoordinates)
    {
        std::unique_lock guard(mCurrentlyProcessedChunks.objectsToBeProcessedMutex);
        if (!isChunkDuringProcessing(chunkPosition))
        {
            mCurrentlyProcessedChunks.objectsToBeProcessed.emplace_back(chunkPosition);
            guard.unlock();

            // TODO: There is tiny chance that it may already disappear before calling updateMesh
            // Baah, it may be even deleted during update
            // Maybe some states per chunk like BUSY or FREE would be a good idea
            // to eliminate those crashes from time to time
            if (isChunkPresentInContainer(chunkPosition))
            {
                mChunkContainer.at(chunkPosition)->updateMesh();
            }
            mCurrentlyProcessedChunks.removeObject(chunkPosition);
        }
    }
}

void ChunkManager::clearFarAwayChunks(const glm::vec3& position)
{
    std::vector<ChunkContainer::Chunks::key_type> coordinateOfChunksToDelete;

    const auto currentChunkOfCamera = ChunkContainer::Coordinate::blockToChunkMetric(
        Block::Coordinate::nonBlockToBlockMetric(sf::Vector3i(position.x, position.y, position.z)));

    /*
     * In the current drawing stage, chunks are not drawn according to the distance from the camera.
     * It is easier to imagine a cube around the camera with side length 2 *
     * WORLD_GENERATION_CHUNK_DISTANCE. It is in its area that the chunks are drawn. Here it should
     * be checked in the same way. I don't want a situation where a barely created chunk is
     * immediately deleted.
     */
    std::unique_lock guard_shared(mChunksAccessMutex);
    for (auto& [coordinate, chunk]: mChunkContainer.data())
    {
        sf::Vector3i distanceBetweenCameraAndChunk;

        distanceBetweenCameraAndChunk.x = std::abs(coordinate.x - currentChunkOfCamera.x);
        distanceBetweenCameraAndChunk.z = std::abs(coordinate.z - currentChunkOfCamera.z);

        if (distanceBetweenCameraAndChunk.x > WORLD_GENERATION_CHUNK_DISTANCE + 1 ||
            distanceBetweenCameraAndChunk.z > WORLD_GENERATION_CHUNK_DISTANCE + 1)
        {
            coordinateOfChunksToDelete.emplace_back(coordinate);
            break;// temporary limit to one
        }
    }
    guard_shared.unlock();
    clearChunks(std::move(coordinateOfChunksToDelete));
}

void ChunkManager::clearChunks(std::vector<ChunkContainer::Coordinate>&& coordinateOfChunksToDelete)
{
    for (const auto& coordinate: coordinateOfChunksToDelete)
    {
        mChunkContainer.erase(coordinate);
    }
}

void ChunkManager::generateChunk(ChunkContainer::Coordinate chunkPosition)
{
    auto newChunk =
        std::make_shared<Chunk>(sf::Vector3i(chunkPosition.nonChunkMetric()), mTexturePack,
                                mChunkContainer, *this, mSavedWorldPath, mWorldSeed);
    auto chunkCoordinates =
        ChunkContainer::Coordinate::blockToChunkMetric(newChunk->positionInBlocks());

    auto& chunk = *newChunk;
    chunk.rebuildMesh();
    mChunkContainer.emplace(chunkPosition, std::move(newChunk));
    mChunkContainer.rebuildChunksAround(chunkCoordinates);
}

std::list<ChunkContainer::Coordinate> ChunkManager::generatesNewChunksAtAndAroundOrigin(
    const ChunkContainer::Coordinate& origin)
{
    auto chunkPositionGetter = CoordinatesAroundOriginGetter(origin);
    auto newlyAddedChunks = std::list<ChunkContainer::Coordinate>();

    constexpr auto SQUARE_PLANE_WITH_SIDE_OF_2_TIMES_WORLD_GENERATION_CHUNK_DISTANCE =
        2 * WORLD_GENERATION_CHUNK_DISTANCE * 2 * WORLD_GENERATION_CHUNK_DISTANCE;

    for (auto i = 0; i < SQUARE_PLANE_WITH_SIDE_OF_2_TIMES_WORLD_GENERATION_CHUNK_DISTANCE; ++i)
    {
        auto nextChunkPosition = chunkPositionGetter.nextValue();
        const auto hasTheNewChunkBeenAdded =
            generateChunkIfNotExist({nextChunkPosition.x, 0, nextChunkPosition.z});

        if (hasTheNewChunkBeenAdded)
        {
            newlyAddedChunks.emplace_back(nextChunkPosition.x, 0, nextChunkPosition.z);
        }

        /*
         * If it draws too many chunks, it stops drawing at further
         * distances so that the processes do not take too long to
         * create it at once.
         */
        if (newlyAddedChunks.size() == MAX_NEW_GENERATED_CHUNK_AT_ONE_THREAD)
        {
            return newlyAddedChunks;
        }
    }
    return newlyAddedChunks;
}

bool ChunkManager::generateChunkIfNotExist(const ChunkContainer::Coordinate& chunkCoordinates)
{
    std::unique_lock guard(mCurrentlyProcessedChunks.objectsToBeProcessedMutex);
    if (!isChunkPresentInContainer(chunkCoordinates) && !isChunkDuringProcessing(chunkCoordinates))
    {
        mCurrentlyProcessedChunks.objectsToBeProcessed.emplace_back(chunkCoordinates);
        guard.unlock();

        generateChunk(chunkCoordinates);
        mCurrentlyProcessedChunks.removeObject(chunkCoordinates);

        return true;
    }
    return false;
}

bool ChunkManager::isChunkPresentInContainer(const ChunkContainer::Coordinate& chunkPosition)
{
    return mChunkContainer.isPresent(chunkPosition);
}

bool ChunkManager::isChunkDuringProcessing(const ChunkContainer::Coordinate& chunkPosition)
{
    std::unique_lock guard(mCurrentlyProcessedChunks.objectsToBeProcessedMutex);
    return std::find(std::begin(mCurrentlyProcessedChunks.objectsToBeProcessed),
                     std::end(mCurrentlyProcessedChunks.objectsToBeProcessed),
                     chunkPosition) != std::end(mCurrentlyProcessedChunks.objectsToBeProcessed);
}

const ChunkContainer& ChunkManager::chunks() const
{
    return mChunkContainer;
}

ChunkContainer& ChunkManager::chunks()
{
    return const_cast<ChunkContainer&>(static_cast<const ChunkManager&>(*this).chunks());
}

ChunkManager::Chunks ChunkManager::rebuildSlowChunks(Chunks&& chunks)
{
    return rebuildNotProcessedChunks(std::move(chunks), RebuildPriority::Low);
}

void ChunkManager::rebuildChunkAtLeastOnce(const std::shared_ptr<ChunkInterface>& chunk,
                                           const RebuildPriority& rebuildSpeed)
{
    const auto& chunkCoordinates =
        ChunkContainer::Coordinate::blockToChunkMetric(chunk->positionInBlocks());

    auto isChunkRebuilt = tryToRebuildChunkIfNotProcessed(chunk, rebuildSpeed, chunkCoordinates);
    while (!isChunkRebuilt)
    {
        isChunkRebuilt = tryToRebuildChunkIfNotProcessed(chunk, rebuildSpeed, chunkCoordinates);
    }
}

bool ChunkManager::tryToRebuildChunkIfNotProcessed(
    const std::shared_ptr<ChunkInterface>& chunk, const ChunkManager::RebuildPriority& rebuildSpeed,
    const ChunkContainer::Coordinate& chunkCoordinates)
{
    std::unique_lock guard(mCurrentlyProcessedChunks.objectsToBeProcessedMutex);
    if (!isChunkDuringProcessing(chunkCoordinates))
    {
        switch (rebuildSpeed)
        {
            case RebuildPriority::Low:
                mCurrentlyProcessedChunks.objectsToBeProcessed.emplace_back(chunkCoordinates);
                break;
            case RebuildPriority::High:
                mCurrentlyProcessedChunks.objectsToBeProcessed.emplace_front(chunkCoordinates);
                break;
            default: throw std::runtime_error("Wrong RebuildSpeed enum value provided");
        }
        guard.unlock();

        chunk->rebuildMesh();
        mCurrentlyProcessedChunks.removeObject(chunkCoordinates);
        return true;
    }
    return false;
}

ChunkManager::Chunks ChunkManager::rebuildFastChunks(Chunks&& chunks)
{
    return rebuildNotProcessedChunks(std::move(chunks), RebuildPriority::High);
}

ChunkManager::Chunks ChunkManager::rebuildNotProcessedChunks(
    ChunkManager::Chunks&& chunks, const ChunkManager::RebuildPriority& rebuildSpeed)
{
    for (const auto& chunk: chunks)
    {
        rebuildChunkAtLeastOnce(chunk, rebuildSpeed);
    }
    return chunks;
}

void ChunkManager::forceFinishingAllProcesses()
{
    while (mChunkToRebuildFast.isAnyProcessRunning() && mChunkToRebuildSlow.isAnyProcessRunning() &&
           mCurrentlyProcessedChunks.isAnyProcessRunning())
    {
        processFinishedThreadsGeneratingNewChunks();
    }
}

ChunkManager::~ChunkManager()
{
    forceFinishingAllProcesses();
}

sf::Vector3f ChunkManager::calculateSpawnPoint()
{
    const auto startupChunk = ChunkContainer::Coordinate(0, 0, 0);
    auto startupPosition = Block::Coordinate(0, 0, 0);
    if (!chunks().isPresent(startupChunk))
    {
        generateChunk(startupChunk);
    }
    startupPosition = chunks().at(startupChunk)->highestSetBlock(startupPosition);
    startupPosition.y += 3;

    return sf::Vector3f(startupPosition.nonBlockMetric()) +
           sf::Vector3f(Block::BLOCK_SIZE / 2.f, 0, Block::BLOCK_SIZE / 2.f);
}
