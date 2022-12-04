#include "Chunk.h"
#include "pch.h"

#include <filesystem>
#include <optional>
#include <utility>

#include "Resources/TexturePack.h"
#include "Utils/Direction.h"
#include "World/Block/BlockType.h"
#include "World/Chunks/ChunkContainer.h"
#include "World/Chunks/ChunkManager.h"
#include "World/Chunks/MeshBuilder.h"
#include "World/Chunks/TerrainGenerator.h"
#include <serializer.h>

Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, ChunkContainer& parent,
             ChunkManager& manager, const std::string& savedWorldPath)
    : Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack, parent, manager,
            savedWorldPath)
{
}

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack,
             ChunkContainer& parent, ChunkManager& manager, const std::string& savedWorldPath)
    : mChunkPosition(std::move(blockPosition))
    , mTexturePack(texturePack)
    , mParentContainer(parent)
    , mTerrainMeshBuilder(mChunkPosition)
    , mFluidMeshBuilder(mChunkPosition)
    , mFloralMeshBuilder(mChunkPosition)
    , mChunkOfBlocks(std::make_shared<ChunkBlocks>())
    , mChunkManager(manager)
    , mTerrainGenerator(std::make_unique<TerrainGenerator>())
    , mSavedWorldPath(savedWorldPath)
{
    auto chunkCoordinate = ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition);

    std::ifstream file(chunkSaveFilePath(), std::ios::binary);
    if (file.is_open())
    {
        auto serializedChunk = readSerializedChunk(file);
        overwriteChunk(serializedChunk);
    }
    else
    {
        generateChunkTerrain();
    }
}

Chunk::Chunk(Chunk&& rhs) noexcept
    : mChunkPosition(std::move(rhs.mChunkPosition))
    , mTexturePack(rhs.mTexturePack)
    , mParentContainer(rhs.mParentContainer)
    , mTerrainMeshBuilder(mChunkPosition)
    , mFluidMeshBuilder(mChunkPosition)
    , mFloralMeshBuilder(mChunkPosition)
    , mTerrainModel(std::move(rhs.mTerrainModel))
    , mChunkOfBlocks(std::move(rhs.mChunkOfBlocks))
    , mChunkManager(rhs.mChunkManager)
    , mTerrainGenerator(std::move(rhs.mTerrainGenerator))
{
}

void Chunk::overwriteChunk(const Chunk::ChunkArray1D& chunk) const
{
    for (auto x = 0; x < BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto y = 0; y < BLOCKS_PER_Y_DIMENSION; ++y)
        {
            for (auto z = 0; z < BLOCKS_PER_Z_DIMENSION; ++z)
            {
                (*mChunkOfBlocks)[x][y][z] = std::make_unique<Block>(
                    chunk[x + BLOCKS_PER_X_DIMENSION * (y + BLOCKS_PER_Y_DIMENSION * z)]);
            }
        }
    }
}

Chunk::ChunkArray1D Chunk::readSerializedChunk(std::ifstream& file) const
{
    // TODO: This thing is actually repeated in three places. Would be got to extract it
    std::istreambuf_iterator<char> iter(file);
    std::vector<char> vec(iter, std::istreambuf_iterator<char>{});
    std::vector<unsigned char> data(vec.begin(), vec.end());
    zpp::serializer::memory_input_archive in(data);
    // ==========
    ChunkArray1D chunkToRead;
    in(chunkToRead);
    return chunkToRead;
}

void Chunk::generateChunkTerrain()
{
    mTerrainGenerator->generateTerrain(*this, *mChunkOfBlocks);
}

void Chunk::createBlockMesh(const Block::Coordinate& pos)
{
    const auto& block = localBlock(pos);

    for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        if (doesBlockFaceHasTransparentNeighbor(static_cast<Block::Face>(i), pos))
        {
            if (block.id() == BlockId::Water)
            {
                if (!doesBlockFaceHasGivenBlockNeighbour(static_cast<Block::Face>(i), pos,
                                                         BlockId::Water))
                {
                    mFluidMeshBuilder.addQuad(
                        static_cast<Block::Face>(i),
                        mTexturePack.normalizedCoordinates(
                            block.blockTextureId(static_cast<Block::Face>(i))),
                        pos);
                }
            }
            else if (block.isFloral())
            {
                mFloralMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                           mTexturePack.normalizedCoordinates(
                                               block.blockTextureId(static_cast<Block::Face>(i))),
                                           pos);
            }
            else
            {
                mTerrainMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                            mTexturePack.normalizedCoordinates(
                                                block.blockTextureId(static_cast<Block::Face>(i))),
                                            pos);
            }
        }
    }
}

void Chunk::prepareMesh()
{
    for (auto x = 0; x < BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto y = 0; y < BLOCKS_PER_Y_DIMENSION; ++y)
        {
            for (auto z = 0; z < BLOCKS_PER_Z_DIMENSION; ++z)
            {
                if (localBlock({x, y, z}).id() == BlockId::Air)
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
    std::scoped_lock guard(mModelsAccessMutex);
    if (!mTerrainModel)
    {
        mTerrainModel = std::make_unique<Model3D>();
    }
    mTerrainModel->setMesh(mTerrainMeshBuilder.mesh3D());

    if (!mFluidModel)
    {
        mFluidModel = std::make_unique<Model3D>();
    }
    mFluidModel->setMesh(mFluidMeshBuilder.mesh3D());

    if (!mFloralModel)
    {
        mFloralModel = std::make_unique<Model3D>();
    }
    mFloralModel->setMesh(mFloralMeshBuilder.mesh3D());
}

void Chunk::fixedUpdate(const float& deltaTime)
{
}

bool Chunk::areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates)
{
    if (localCoordinates.x < BLOCKS_PER_X_DIMENSION && localCoordinates.x >= 0 &&
        localCoordinates.y < BLOCKS_PER_Y_DIMENSION && localCoordinates.y >= 0 &&
        localCoordinates.z < BLOCKS_PER_Z_DIMENSION && localCoordinates.z >= 0)
    {
        return true;
    }

    return false;
}

bool Chunk::isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates)
{
    if (localCoordinates.x == BLOCKS_PER_X_DIMENSION - 1 || localCoordinates.x == 0 ||
        localCoordinates.y == BLOCKS_PER_Y_DIMENSION - 1 || localCoordinates.y == 0 ||
        localCoordinates.z == BLOCKS_PER_Z_DIMENSION - 1 || localCoordinates.z == 0)
    {
        return true;
    }

    return false;
}

void Chunk::rebuildSlow()
{
    auto thisChunk = mParentContainer.findChunk(*this);
    mChunkManager.rebuildSlow(thisChunk);
}

void Chunk::rebuildFast()
{

    auto thisChunk = mParentContainer.findChunk(*this);
    mChunkManager.rebuildFast(thisChunk);
}

void Chunk::rebuildMesh()
{
    mTerrainMeshBuilder.resetMesh();
    mFluidMeshBuilder.resetMesh();
    mFloralMeshBuilder.resetMesh();
    prepareMesh();
}

void Chunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
    std::unique_lock guard(mChunkAccessMutex);
    (*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z]->setBlockType(
        BlockId::Air);
    guard.unlock();

    rebuildFast();
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
    std::unique_lock guard(mChunkAccessMutex);
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

Chunk::Chunk(std::shared_ptr<ChunkBlocks> chunkBlocks, Block::Coordinate blockPosition,
             const TexturePack& texturePack, ChunkContainer& parent, ChunkManager& manager)
    : mChunkPosition(std::move(blockPosition))
    , mTexturePack(texturePack)
    , mParentContainer(parent)
    , mTerrainMeshBuilder(mChunkPosition)
    , mFluidMeshBuilder(mChunkPosition)
    , mFloralMeshBuilder(mChunkPosition)
    , mChunkOfBlocks(std::move(chunkBlocks))
    , mChunkManager(manager)
{
    prepareMesh();
}

Block::Coordinate Chunk::localToGlobalCoordinates(const Block::Coordinate& localCoordinates) const
{
    return static_cast<Block::Coordinate>(mChunkPosition + localCoordinates);
}

std::vector<Direction> Chunk::directionOfBlockFacesInContactWithOtherChunk(
    const Block::Coordinate& localCoordinates)
{
    std::vector<Direction> directions;

    if (localCoordinates.x == BLOCKS_PER_X_DIMENSION - 1)
    {
        directions.emplace_back(Direction::ToTheRight);
    }
    if (localCoordinates.x == 0)
    {
        directions.emplace_back(Direction::ToTheLeft);
    }
    if (localCoordinates.y == BLOCKS_PER_Y_DIMENSION - 1)
    {
        directions.emplace_back(Direction::Above);
    }
    if (localCoordinates.y == 0)
    {
        directions.emplace_back(Direction::Below);
    }
    if (localCoordinates.z == BLOCKS_PER_Z_DIMENSION - 1)
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
        auto neighbour = neighbourBlockInGivenDirection(blockPos, face);
        return (neighbour.has_value() && neighbour.value().isTransparent());
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

bool Chunk::doesBlockFaceHasGivenBlockNeighbour(const Block::Face& blockFace,
                                                const Block::Coordinate& blockPos,
                                                const BlockId& blockId)
{
    auto isBlockOfGivenId = [&blockPos, &blockId, this](const Direction& face)
    {
        auto neighbour = neighbourBlockInGivenDirection(blockPos, face);
        return (neighbour.has_value() && neighbour.value().id() == blockId);
    };

    switch (blockFace)
    {
        case Block::Face::Top: return (isBlockOfGivenId(Direction::Above));
        case Block::Face::Left: return (isBlockOfGivenId(Direction::ToTheLeft));
        case Block::Face::Right: return (isBlockOfGivenId(Direction::ToTheRight));
        case Block::Face::Bottom: return (isBlockOfGivenId(Direction::Below));
        case Block::Face::Front: return (isBlockOfGivenId(Direction::InFront));
        case Block::Face::Back: return (isBlockOfGivenId(Direction::Behind));
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

std::optional<Block> Chunk::neighbourBlockInGivenDirection(const Block::Coordinate& blockPos,
                                                           const Direction& direction)
{
    const auto blockNeighborPosition = localNearbyBlockPosition(blockPos, direction);
    if (areLocalCoordinatesInsideChunk(blockNeighborPosition))
    {
        return (localBlock(blockNeighborPosition).id());
    }

    if (const auto& neighborBlock =
            mParentContainer.worldBlock(localToGlobalCoordinates(blockNeighborPosition)))
    {
        return neighborBlock->id();
    }

    return std::nullopt;
}

void Chunk::drawTerrain(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    std::scoped_lock guard(mModelsAccessMutex);
    if (mTerrainModel)
    {
        mTerrainModel->draw(renderer3d, shader);
    }
}

void Chunk::drawLiquids(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    std::scoped_lock guard(mModelsAccessMutex);
    if (mFluidModel)
    {
        mFluidModel->draw(renderer3d, shader);
    }
}

void Chunk::drawFlorals(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    std::scoped_lock guard(mModelsAccessMutex);
    if (mFloralModel)
    {
        mFloralModel->draw(renderer3d, shader);
    }
}

const Block::Coordinate& Chunk::positionInBlocks() const
{
    return mChunkPosition;
}

void Chunk::tryToPlaceBlock(const BlockId& blockId, const Block::Coordinate& localCoordinates,
                            std::vector<BlockId> blocksThatMightBeOverplaced,
                            const RebuildOperation& rebuildOperation)
{
    if (areLocalCoordinatesInsideChunk(localCoordinates))
    {
        tryToPlaceBlockInsideThisChunk(blockId, localCoordinates, blocksThatMightBeOverplaced,
                                       rebuildOperation);
    }
    else
    {
        auto globalCoordinates = localToGlobalCoordinates(localCoordinates);
        mParentContainer.tryToPlaceBlock(blockId, globalCoordinates, blocksThatMightBeOverplaced,
                                         rebuildOperation);
    }
}

void Chunk::tryToPlaceBlockInsideThisChunk(const BlockId& blockId,
                                           const Block::Coordinate& localCoordinates,
                                           std::vector<BlockId>& blocksThatMightBeOverplaced,
                                           const Chunk::RebuildOperation& rebuildOperation)
{
    std::unique_lock guard(mChunkAccessMutex);
    auto& block = (*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z];
    auto idOfTheBlockToOverplace = block->id();

    if (canGivenBlockBeOverplaced(blocksThatMightBeOverplaced, idOfTheBlockToOverplace))
    {
        block->setBlockType(blockId);
        switch (rebuildOperation)
        {
            case RebuildOperation::Fast: rebuildFast(); break;
            case RebuildOperation::Slow: rebuildSlow(); break;
        }
    }
    guard.unlock();
}

bool Chunk::canGivenBlockBeOverplaced(std::vector<BlockId>& blocksThatMightBeOverplaced,
                                      const BlockId& idOfTheBlockToOverplace) const
{
    return std::any_of(blocksThatMightBeOverplaced.begin(), blocksThatMightBeOverplaced.end(),
                       [&idOfTheBlockToOverplace](const auto& blockThatMightBeOverplaced)
                       {
                           return (blockThatMightBeOverplaced == idOfTheBlockToOverplace) ||
                                  (blockThatMightBeOverplaced == BlockId::AllBlocks);
                       });
}

void Chunk::saveChunkToFile()
{
    std::vector<unsigned char> serializedData = serializedChunk();

    std::filesystem::create_directories(mSavedWorldPath);
    std::ofstream outfile(chunkSaveFilePath(), std::ios::out | std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(serializedData.data()),
                  serializedData.size() * sizeof(char));
    outfile.close();
}

std::vector<unsigned char> Chunk::serializedChunk()
{
    auto chunkCoordinate = ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition);
    auto chunk = oneDimensionalChunkRepresentation();
    std::vector<unsigned char> serializedData;
    zpp::serializer::memory_output_archive out(serializedData);
    out(chunk);
    return serializedData;
}

std::string Chunk::chunkSaveFilePath()
{
    auto chunkCoordinate = ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition);
    return mSavedWorldPath + "/chunk_" + std::to_string(chunkCoordinate.x) + "_" +
           std::to_string(chunkCoordinate.y) + "_" + std::to_string(chunkCoordinate.z) + ".bin";
}

Chunk::ChunkArray1D Chunk::oneDimensionalChunkRepresentation()
{
    auto chunkCoordinate = ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition);
    ChunkArray1D chunk;
    for (auto x = 0; x < BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto y = 0; y < BLOCKS_PER_Y_DIMENSION; ++y)
        {
            for (auto z = 0; z < BLOCKS_PER_Z_DIMENSION; ++z)
            {
                chunk[x + BLOCKS_PER_X_DIMENSION * (y + BLOCKS_PER_Y_DIMENSION * z)] =
                    localBlock({x, y, z}).id();
            }
        }
    }
    return chunk;
}

Chunk::~Chunk()
{
    saveChunkToFile();
}
