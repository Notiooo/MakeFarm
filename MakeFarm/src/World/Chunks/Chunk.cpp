#include "Chunk.h"
#include "pch.h"

#include <utility>

#include "ChunkContainer.h"
#include "MeshBuilder.h"
#include "Resources/TexturePack.h"
#include "Utils/Direction.h"
#include "World/Block/BlockType.h"
#include <FastNoiseLite.h>


Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, ChunkContainer& parent)
    : Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack, parent)
{
}

Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack)
    : Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack)
{
}

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack,
             ChunkContainer& parent)
    : mChunkPosition(std::move(blockPosition))
    , mTexturePack(texturePack)
    , mParentContainer(&parent)
    , mMeshBuilder(mChunkPosition)
    , mChunkOfBlocks(std::make_shared<ChunkBlocks>())
{
    generateChunkTerrain();

    rebuildChunksAround();
    markToBeRebuildSlow();
}

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack)
    : mChunkPosition(std::move(blockPosition))
    , mTexturePack(texturePack)
    , mParentContainer(nullptr)
    , mMeshBuilder(mChunkPosition)
    , mChunkOfBlocks(std::make_shared<ChunkBlocks>())
{
    generateChunkTerrain();

    prepareMesh();
    updateMesh();
}

Chunk::Chunk(Chunk&& rhs) noexcept
    : mChunkPosition(std::move(rhs.mChunkPosition))
    , mTexturePack(rhs.mTexturePack)
    , mParentContainer(rhs.mParentContainer)
    , mMeshBuilder(mChunkPosition)
    , mModel(std::move(rhs.mModel))
    , mChunkOfBlocks(std::move(rhs.mChunkOfBlocks))
{
}

void Chunk::generateChunkTerrain()
{
    static FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02);

    for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
    {
        for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
        {
            auto globalCoordinate = localToGlobalCoordinates({x, 0, z});

            auto heightOfBlocks = noise.GetNoise(static_cast<float>(globalCoordinate.x),
                                                 static_cast<float>(globalCoordinate.z));

            heightOfBlocks = (heightOfBlocks + 1) / 2.f;// range of 0 .. 1
            auto heightOfColumn = heightOfBlocks * ChunkContainer::MAX_HEIGHT_MAP;

            heightOfColumn -= globalCoordinate.y;

            for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
            {
                if (heightOfColumn > 0)
                {
                    (*mChunkOfBlocks)[x][y][z] = std::make_unique<Block>("Grass");
                }
                else
                {
                    (*mChunkOfBlocks)[x][y][z] = std::make_unique<Block>("Air");
                }

                heightOfColumn -= Block::BLOCK_SIZE;
            }
        }
    }
}

void Chunk::createBlockMesh(const Block::Coordinate& pos)
{
    const auto& block = localBlock(pos);

    for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        if (doesBlockFaceHasTransparentNeighbor(static_cast<Block::Face>(i), pos))
        {
            mMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                 mTexturePack.normalizedCoordinates(
                                     block.blockTextureId(static_cast<Block::Face>(i))),
                                 pos);
        }
    }
}

void Chunk::prepareMesh()
{
    for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
    {
        for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
        {
            for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
            {
                if (localBlock({x, y, z}).blockId() == "Air")
                {
                    continue;
                }

                createBlockMesh({x, y, z});
            }
        }
    }
}

void Chunk::updateMesh()
{
    if (!mModel)
    {
        mModel = std::make_unique<Model3D>();
    }

    mModel->setMesh(mMeshBuilder.mesh3D());
}

void Chunk::fixedUpdate(const float& deltaTime)
{
}

bool Chunk::areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates)
{
    if (localCoordinates.x < BLOCKS_PER_DIMENSION && localCoordinates.x >= 0 &&
        localCoordinates.y < BLOCKS_PER_DIMENSION && localCoordinates.y >= 0 &&
        localCoordinates.z < BLOCKS_PER_DIMENSION && localCoordinates.z >= 0)
    {
        return true;
    }

    return false;
}

bool Chunk::isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates)
{
    if (localCoordinates.x == BLOCKS_PER_DIMENSION - 1 || localCoordinates.x == 0 ||
        localCoordinates.y == BLOCKS_PER_DIMENSION - 1 || localCoordinates.y == 0 ||
        localCoordinates.z == BLOCKS_PER_DIMENSION - 1 || localCoordinates.z == 0)
    {
        return true;
    }

    return false;
}

void Chunk::markToBeRebuildSlow() const
{
    if (mParentContainer)
    {
        std::shared_lock guard(mParentContainer->mChunksAccessMutex);
        if (auto foundChunk = mParentContainer->mChunks.find(
                ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition));
            foundChunk != mParentContainer->mChunks.end())
        {
            std::scoped_lock guard(mParentContainer->mChunksToRebuildSlowAccessMutex);
            auto& vec = mParentContainer->mChunksToRebuildSlow;

            if (auto foundElement = std::find(vec.begin(), vec.end(), foundChunk->second);
                foundElement != vec.end())
            {
                std::rotate(foundElement, foundElement + 1, vec.end());
            }
            else
            {
                mParentContainer->mChunksToRebuildSlow.push_back(foundChunk->second);
            }
        }
    }
}

void Chunk::markToBeRebuildFast() const
{
    if (mParentContainer)
    {
        std::shared_lock guard(mParentContainer->mChunksAccessMutex);
        if (auto foundChunk = mParentContainer->mChunks.find(
                ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition));
            foundChunk != mParentContainer->mChunks.end())
        {
            std::scoped_lock guard(mParentContainer->mChunksToRebuildFastAccessMutex);
            mParentContainer->mChunksToRebuildFast.push_back(foundChunk->second);
        }
    }
}

void Chunk::rebuildMesh()
{
    std::lock_guard _(mRebuildMeshMutex);
    mMeshBuilder.blockMesh();
    mMeshBuilder.resetMesh();
    prepareMesh();
    mMeshBuilder.unblockMesh();
}

void Chunk::rebuildChunksAround()
{
    for (auto direction = static_cast<int>(Direction::None) + 1;
         direction < static_cast<int>(Direction::Counter); ++direction)
    {
        if (const auto chunk = chunkNearby(static_cast<Direction>(direction)))
        {
            chunk->markToBeRebuildSlow();
        }
    }
}

void Chunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
    (*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z]->setBlockType(
        "Air");

    markToBeRebuildFast();
}

Block::Coordinate Chunk::globalToLocalCoordinates(const Block::Coordinate& worldCoordinates) const
{
    return static_cast<Block::Coordinate>(worldCoordinates - mChunkPosition);
}

Block& Chunk::localBlock(const Block::Coordinate& localCoordinates)
{
    return const_cast<Block&>(static_cast<const Chunk&>(*this).localBlock(localCoordinates));
}

Block& Chunk::localNearbyBlock(const Block::Coordinate& position, const Direction& direction)
{
    return const_cast<Block&>(
        static_cast<const Chunk&>(*this).localNearbyBlock(position, direction));
}

const Block& Chunk::localBlock(const Block::Coordinate& localCoordinates) const
{
    return *(*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z];
}

const Block& Chunk::localNearbyBlock(const Block::Coordinate& localCoordinates,
                                     const Direction& direction) const
{
    return localBlock(localNearbyBlockPosition(localCoordinates, direction));
}

Block::Coordinate Chunk::localNearbyBlockPosition(const Block::Coordinate& position,
                                                  const Direction& direction) const
{
    switch (direction)
    {
        case Direction::Above: return {position.x, position.y + 1, position.z};
        case Direction::Below: return {position.x, position.y - 1, position.z};
        case Direction::ToTheLeft: return {position.x - 1, position.y, position.z};
        case Direction::ToTheRight: return {position.x + 1, position.y, position.z};
        case Direction::InFront: return {position.x, position.y, position.z + 1};
        case Direction::Behind: return {position.x, position.y, position.z - 1};
        default: throw std::runtime_error("Unsupported Direction value was provided");
    }
}

std::shared_ptr<Chunk> Chunk::chunkNearby(const Direction& direction)
{
    switch (direction)
    {
        case Direction::Above:
            return mParentContainer->blockPositionToChunk(
                localToGlobalCoordinates({0, BLOCKS_PER_DIMENSION, 0}));
        case Direction::Below:
            return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({0, -1, 0}));
        case Direction::ToTheLeft:
            return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({-1, 0, 0}));
        case Direction::ToTheRight:
            return mParentContainer->blockPositionToChunk(
                localToGlobalCoordinates({BLOCKS_PER_DIMENSION, 0, 0}));
        case Direction::InFront:
            return mParentContainer->blockPositionToChunk(
                localToGlobalCoordinates({0, 0, BLOCKS_PER_DIMENSION}));
        case Direction::Behind:
            return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({0, 0, -1}));
        default: throw std::runtime_error("Unsupported Direction value was provided");
    }
}

Chunk::Chunk(std::shared_ptr<ChunkBlocks> chunkBlocks, Block::Coordinate blockPosition,
             const TexturePack& texturePack, ChunkContainer& parent)
    : mChunkPosition(std::move(blockPosition))
    , mTexturePack(texturePack)
    , mParentContainer(&parent)
    , mMeshBuilder(mChunkPosition)
    , mChunkOfBlocks(std::move(chunkBlocks))
{
    prepareMesh();
}

Block::Coordinate Chunk::localToGlobalCoordinates(const Block::Coordinate& localCoordinates) const
{
    return mChunkPosition + localCoordinates;
}

std::vector<Direction> Chunk::directionOfBlockFacesInContactWithOtherChunk(
    const Block::Coordinate& localCoordinates)
{
    std::vector<Direction> directions;

    if (localCoordinates.x == BLOCKS_PER_DIMENSION - 1)
    {
        directions.emplace_back(Direction::ToTheRight);
    }
    if (localCoordinates.x == 0)
    {
        directions.emplace_back(Direction::ToTheLeft);
    }
    if (localCoordinates.y == BLOCKS_PER_DIMENSION - 1)
    {
        directions.emplace_back(Direction::Above);
    }
    if (localCoordinates.y == 0)
    {
        directions.emplace_back(Direction::Below);
    }
    if (localCoordinates.z == BLOCKS_PER_DIMENSION - 1)
    {
        directions.emplace_back(Direction::InFront);
    }
    if (localCoordinates.z == 0)
    {
        directions.emplace_back(Direction::Behind);
    }

    return directions;
}

bool Chunk::doesBlockFaceHasTransparentNeighbor(const Block::Face& blockFace,
                                                const Block::Coordinate& blockPos)
{
    auto isBlockTransparent = [&blockPos, this](const Direction& face)
    {
        const auto blockNeighborPosition = localNearbyBlockPosition(blockPos, face);
        if (areLocalCoordinatesInsideChunk(blockNeighborPosition))
        {
            return (localBlock(blockNeighborPosition).isTransparent());
        }

        if (belongsToAnyChunkContainer())
        {
            if (const auto& neighborBlock =
                    mParentContainer->worldBlock(localToGlobalCoordinates(blockNeighborPosition)))
            {
                return neighborBlock->isTransparent();
            }
        }

        return true;
    };

    switch (blockFace)
    {
        case Block::Face::Top: return (isBlockTransparent(Direction::Above));
        case Block::Face::Left: return (isBlockTransparent(Direction::ToTheLeft));
        case Block::Face::Right: return (isBlockTransparent(Direction::ToTheRight));
        case Block::Face::Bottom: return (isBlockTransparent(Direction::Below));
        case Block::Face::Front: return (isBlockTransparent(Direction::InFront));
        case Block::Face::Back: return (isBlockTransparent(Direction::Behind));
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

bool Chunk::belongsToAnyChunkContainer() const
{
    return (mParentContainer != nullptr);
}

void Chunk::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    if (mModel)
    {
        mModel->draw(renderer3d, shader);
    }
}
