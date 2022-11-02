#include "ChunkContainer.h"
#include "pch.h"
#include <Utils/IteratorRanges.h>

#include "CoordinatesAroundOriginGetter.h"
#include "World/Block/BlockMap.h"


void ChunkContainer::drawTerrain(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
    std::unique_lock guard(mChunksAccessMutex);
    for (auto& [coordinate, chunk]: data())
    {
        chunk->drawTerrain(renderer3D, shader);
    }
}

void ChunkContainer::drawLiquids(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
    std::unique_lock guard(mChunksAccessMutex);
    for (auto& [coordinate, chunk]: data())
    {
        chunk->drawLiquids(renderer3D, shader);
    }
}

void ChunkContainer::drawFlorals(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
    std::unique_lock guard(mChunksAccessMutex);
    for (auto& [coordinate, chunk]: data())
    {
        chunk->drawFlorals(renderer3D, shader);
    }
}

void ChunkContainer::update(const float& deltaTime)
{
    placeScheduledBlocksForNewAppearingChunks();
    tryToPlaceScheduledBlocksForNewAppearingChunks();
}

void ChunkContainer::placeScheduledBlocksForNewAppearingChunks()
{
    std::scoped_lock guard(mBlockToBePlacedAccessMutex);
    if (!mBlockToBePlacedInFutureChunks.empty())
    {
        placeScheduledBlocksForPresentChunks();
    }
}

void ChunkContainer::tryToPlaceScheduledBlocksForNewAppearingChunks()
{
    std::scoped_lock guard(mBlockMightBePlacedAccessMutex);
    if (!mBlockMightBePlacedInFutureChunks.empty())
    {
        tryToPlaceScheduledBlocksForPresentChunks();
    }
}

void ChunkContainer::tryToPlaceScheduledBlocksForPresentChunks()
{
    for (auto [begIter, endIter] = iterator_range(mBlockMightBePlacedInFutureChunks);
         begIter != endIter;)
    {
        if (isPresent(begIter->chunkCoordinates))
        {
            auto block = worldBlock(begIter->worldBlockCoordinates);
            if (block && block->blockId() == BlockId::Air)
            {
                block->setBlockType(begIter->blockid);
            }
            begIter = mBlockMightBePlacedInFutureChunks.erase(begIter);
        }
        else
        {
            ++begIter;
        }
    }
}

void ChunkContainer::placeScheduledBlocksForPresentChunks()
{
    for (auto [begIter, endIter] = iterator_range(mBlockToBePlacedInFutureChunks);
         begIter != endIter;)
    {
        if (isPresent(begIter->chunkCoordinates))
        {
            worldBlock(begIter->worldBlockCoordinates)->setBlockType(begIter->blockid);
            begIter = mBlockToBePlacedInFutureChunks.erase(begIter);
        }
        else
        {
            ++begIter;
        }
    }
}

sf::Vector3i ChunkContainer::Coordinate::nonChunkMetric() const
{
    return sf::Vector3i(x * Chunk::BLOCKS_PER_X_DIMENSION * Block::BLOCK_SIZE,
                        y * Chunk::BLOCKS_PER_Y_DIMENSION * Block::BLOCK_SIZE,
                        z * Chunk::BLOCKS_PER_Z_DIMENSION * Block::BLOCK_SIZE);
}

ChunkContainer::Coordinate ChunkContainer::Coordinate::blockToChunkMetric(
    const Block::Coordinate& worldBlockCoordinate)
{

    static auto fastFloor = [](const float& numberToFloor)
    {
        const auto truncated = static_cast<int>(numberToFloor);
        return truncated - (truncated > numberToFloor);
    };

    // clang-format off
    auto returnVar = ChunkContainer::Coordinate(
        fastFloor(worldBlockCoordinate.x / static_cast<float>(Chunk::BLOCKS_PER_X_DIMENSION)),
        fastFloor(worldBlockCoordinate.y / static_cast<float>(Chunk::BLOCKS_PER_Y_DIMENSION)),
        fastFloor(worldBlockCoordinate.z / static_cast<float>(Chunk::BLOCKS_PER_Z_DIMENSION))
    );
    // clang-format on

    return returnVar;
}

const Block* ChunkContainer::worldBlock(const Block::Coordinate& worldBlockCoordinates) const
{
    if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
    {
        return &chunk->localBlock(chunk->globalToLocalCoordinates(worldBlockCoordinates));
    }
    return nullptr;
}

Block* ChunkContainer::worldBlock(const Block::Coordinate& worldBlockCoordinates)
{
    return const_cast<Block*>(
        static_cast<const ChunkContainer&>(*this).worldBlock(worldBlockCoordinates));
}

bool ChunkContainer::doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const
{
    std::unique_lock guard(mChunksAccessMutex);
    const auto foundChunk =
        data().find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
    return foundChunk != data().cend();
}

std::shared_ptr<const Chunk> ChunkContainer::blockPositionToChunk(
    const Block::Coordinate& worldBlockCoordinates) const
{
    std::unique_lock guard(mChunksAccessMutex);
    const auto foundChunk =
        data().find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
    if (foundChunk != data().cend())
    {
        return foundChunk->second;
    }
    return nullptr;
}

std::shared_ptr<Chunk> ChunkContainer::blockPositionToChunk(
    const Block::Coordinate& worldBlockCoordinates)
{
    return std::const_pointer_cast<Chunk>(
        static_cast<const ChunkContainer&>(*this).blockPositionToChunk(worldBlockCoordinates));
}


void ChunkContainer::removeWorldBlock(const Block::Coordinate& worldBlockCoordinates)
{
    if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
    {
        auto localCoordinates = chunk->globalToLocalCoordinates(worldBlockCoordinates);

        // TODO: Something with mutex would probably be useful there. Observe this part.
        chunk->removeLocalBlock(localCoordinates);

        /*
         * When removing a block, you may find that it is in contact with an adjacent chunk.
         * Rebuilding one chunk doesn't help, because the neighboring chunk remains in the form
         * where it assumes the block is there. This leads to a hole in the chunk. For this reason,
         * you must rebuild all chunks that are in contact with the block being removed.
         */
        const auto directions =
            chunk->directionOfBlockFacesInContactWithOtherChunk(localCoordinates);
        for (auto& blockDirection: directions)
        {
            auto neighboringBlockInOtherChunk = chunk->localToGlobalCoordinates(
                chunk->localNearbyBlockPosition(localCoordinates, blockDirection));

            if (const auto neighboringChunk = blockPositionToChunk(neighboringBlockInOtherChunk))
            {
                neighboringChunk->rebuildFast();
            }
        }
    }
}

std::shared_ptr<Chunk> ChunkContainer::chunkNearby(const Chunk& baseChunk,
                                                   const Direction& direction)
{
    switch (direction)
    {
        case Direction::ToTheLeft:
        {
            return blockPositionToChunk(baseChunk.localToGlobalCoordinates({-1, 0, 0}));
        }
        case Direction::ToTheRight:
        {
            return blockPositionToChunk(
                baseChunk.localToGlobalCoordinates({Chunk::BLOCKS_PER_X_DIMENSION, 0, 0}));
        }
        case Direction::InFront:
        {
            return blockPositionToChunk(
                baseChunk.localToGlobalCoordinates({0, 0, Chunk::BLOCKS_PER_Z_DIMENSION}));
        }
        case Direction::Behind:
        {
            return blockPositionToChunk(baseChunk.localToGlobalCoordinates({0, 0, -1}));
        }
        default: throw std::runtime_error("Unsupported Direction value was provided");
    }
}

ChunkContainer::Chunks& ChunkContainer::data()
{
    return mData;
}

const ChunkContainer::Chunks& ChunkContainer::data() const
{
    return mData;
}

bool ChunkContainer::isChunkPresentInTheContainer(const Chunk& chunk) const
{
    std::unique_lock guard(mChunksAccessMutex);
    return data().find(ChunkContainer::Coordinate::blockToChunkMetric(chunk.positionInBlocks())) !=
           data().cend();
}

std::shared_ptr<Chunk> ChunkContainer::findChunk(const Chunk& chunk)
{
    std::unique_lock guard(mChunksAccessMutex);
    auto foundChunk =
        data().find(ChunkContainer::Coordinate::blockToChunkMetric(chunk.positionInBlocks()));

    return (foundChunk != data().end()) ? foundChunk->second : nullptr;
}

void ChunkContainer::rebuildChunksAround(ChunkContainer::Coordinate chunkCoordinates)
{
    auto& chunk = data().at(chunkCoordinates);
    for (auto direction:
         {Direction::Behind, Direction::InFront, Direction::ToTheLeft, Direction::ToTheRight})
    {
        if (const auto chunkClose = chunkNearby(*chunk, direction))
        {
            chunkClose->rebuildSlow();
        }
    }
}
std::shared_ptr<Chunk> ChunkContainer::at(const ChunkContainer::Coordinate& chunkCoordinate)
{
    std::unique_lock guard(mChunksAccessMutex);
    return data().at(chunkCoordinate);
}

ChunkContainer::Chunks::size_type ChunkContainer::erase(
    const ChunkContainer::Coordinate& chunkCoordinate)
{
    std::scoped_lock guard(mChunksAccessMutex);
    return data().erase(chunkCoordinate);
}

bool ChunkContainer::isPresent(const ChunkContainer::Coordinate& chunkPosition) const
{
    std::unique_lock guard(mChunksAccessMutex);
    return data().find(chunkPosition) != data().end();
}

void ChunkContainer::placeBlockWithoutRebuild(const BlockId& id, Block::Coordinate worldCoordinate)
{
    if (const auto chunk = blockPositionToChunk(worldCoordinate))
    {
        chunk->localBlock(chunk->globalToLocalCoordinates(worldCoordinate)).setBlockType(id);
    }
    else
    {
        const auto& chunkCoordinates =
            ChunkContainer::Coordinate::blockToChunkMetric(worldCoordinate);

        std::scoped_lock guard(mBlockToBePlacedAccessMutex);
        mBlockToBePlacedInFutureChunks.push_back(
            BlockToBePlaced{chunkCoordinates, id, worldCoordinate});
    }
}

void ChunkContainer::tryToPlaceBlockWithoutRebuild(const BlockId& id,
                                                   Block::Coordinate worldCoordinate)
{
    if (const auto chunk = blockPositionToChunk(worldCoordinate))
    {
        auto& localBlock = chunk->localBlock(chunk->globalToLocalCoordinates(worldCoordinate));
        if (localBlock.blockId() == BlockId::Air)
        {
            localBlock.setBlockType(id);
        }
    }
    else
    {
        const auto& chunkCoordinates =
            ChunkContainer::Coordinate::blockToChunkMetric(worldCoordinate);

        std::scoped_lock guard(mBlockMightBePlacedAccessMutex);
        mBlockMightBePlacedInFutureChunks.push_back(
            BlockToBePlaced{chunkCoordinates, id, worldCoordinate});
    }
}
