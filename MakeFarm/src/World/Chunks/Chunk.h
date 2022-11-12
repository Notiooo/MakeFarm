#pragma once
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>

#include "MeshBuilder.h"
#include "Renderer3D/Model3D.h"
#include "Renderer3D/Renderer3D.h"
#include "Utils/Direction.h"
#include "Utils/MultiDimensionalArray.h"
#include "World/Block/Block.h"
#include "World/Block/BlockId.h"

class ChunkContainer;
class ChunkManager;
class TerrainGenerator;
class AABB;


/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class Chunk
{
public:
    Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, ChunkContainer& parent,
          ChunkManager& manager);

    Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack);

    Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack, ChunkContainer& parent,
          ChunkManager& manager);

    Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack);
    Chunk(Chunk&& rhs) noexcept;
    ~Chunk();

    static constexpr int BLOCKS_PER_X_DIMENSION = 16;
    static constexpr int BLOCKS_PER_Y_DIMENSION = 127;
    static constexpr int BLOCKS_PER_Z_DIMENSION = 16;
    static constexpr int BLOCKS_IN_CHUNK =
        BLOCKS_PER_X_DIMENSION * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_Z_DIMENSION;

    using ChunkBlocks = MultiDimensionalArray<std::unique_ptr<Block>, BLOCKS_PER_X_DIMENSION,
                                              BLOCKS_PER_Y_DIMENSION, BLOCKS_PER_Z_DIMENSION>;

    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    void prepareMesh();

    /**
     * \brief Swaps the current chunk mesh with the latest, most recently generated one
     */
    void updateMesh();

    /**
     * Updates the status/logic of the state at equal intervals independent of the frame rate.
     * @param deltaTime Time interval
     */
    void fixedUpdate(const float& deltaTime);

    /**
     * Draws this chunk terrain to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawTerrain(const Renderer3D& renderer3d, const sf::Shader& shader) const;

    /**
     * Draws this chunk liquids to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawLiquids(const Renderer3D& renderer3d, const sf::Shader& shader) const;

    /**
     * Draws this chunk floral to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawFlorals(const Renderer3D& renderer3d, const sf::Shader& shader) const;

    /**
     * \brief Returns the block according to the coordinates given relative to the chunk position.
     * \param localCoordinates Position in relation to the chunk
     * \return Block reference inside chunk
     */
    Block& localBlock(const Block::Coordinate& localCoordinates);

    /**
     * \brief Removes a block on coordinates given relatively to the position of the chunk
     * \param localCoordinates Coordinates relative to the position of the chunk
     */
    void removeLocalBlock(const Block::Coordinate& localCoordinates);


    /**
     * @brief Inserts a block into the map without rebuilding the mesh. If another block was there
     * then it is replaced. The function is safe if the block goes beyond the current chunk.
     * @param blockId Block identifier
     * @param localCoordinates Coordinates relative to the position of the chunk
     */
    void placeBlockWithoutRebuild(const BlockId& blockId,
                                  const Block::Coordinate& localCoordinates);

    /**
     * \brief Tries to insert a block into the map without rebuilding the mesh. If in that place
     * there was different block than air it does nothing. The function is safe if the block goes
     * beyond the current chunk.
     * \param blockId Block Identifier
     * \param localCoordinates Coordinates relative to the position of the chunk
     */
    void tryToPlaceBlockWithoutRebuild(const BlockId& blockId,
                                       const Block::Coordinate& localCoordinates);

    /**
     * \brief Returns the block according to the coordinates given relative to the chunk position.
     * \param localCoordinates Position in relation to the chunk
     * \return Block reference inside chunk
     */
    [[nodiscard]] const Block& localBlock(const Block::Coordinate& localCoordinates) const;

    /**
     * \brief Changes global world coordinates to local ones relative to chunk
     * \param worldCoordinates World coordinates of the block
     * \return Local coordinates of block relative to chunk
     */
    [[nodiscard]] Block::Coordinate globalToLocalCoordinates(
        const Block::Coordinate& worldCoordinates) const;

    /**
     * \brief Changes local chunk coordinates to global ones inside the world
     * \param localCoordinates Local coordinates of the block
     * \return World coordinates of block inside the world
     */
    [[nodiscard]] Block::Coordinate localToGlobalCoordinates(
        const Block::Coordinate& localCoordinates) const;

    /**
     * \brief Checks whether the local coordinates relative to the chunk are inside it
     * \param localCoordinates Local coordinates relative to chunk
     * \return True if local coordinates are inside the chunk, false otherwise
     */
    [[nodiscard]] static bool areLocalCoordinatesInsideChunk(
        const Block::Coordinate& localCoordinates);

    /**
     * \brief Checks that the given local coordinates relative to the chunk are at its extremity.
     * \param localCoordinates Local coordinates relative to chunk
     * \return True if the coordinates are on the edge of the chunk, false otherwise
     */
    [[nodiscard]] bool isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates);

    /**
     * \brief Checks whether the chunk is inside any chunk container
     * \return True if the chunk is inside any chunk container, false otherwise
     */
    [[nodiscard]] bool belongsToAnyChunkContainer() const;

    /**
     * It is rebuilding this mesh fresh. Very expensive operation
     */
    void rebuildMesh();

    /**
     * \brief Indicates chunk as willing to rebuild mesh in near future
     */
    void rebuildSlow();

    /**
     * \brief Indicates chunk as willing to rebuild mesh in this, or next frame
     */
    void rebuildFast();

    /**
     * Returns information whether any chunk is in contact with the listed block.
     * This is important information when a given block has been destroyed and there
     * is a need to rebuild the chunk next to it so that it does not display an empty hole.
     * @param localCoordinates Block coordinates Coordinates of the block to be checked
     * @return Information on whether the block is in contact with another chunk
     */
    std::vector<Direction> directionOfBlockFacesInContactWithOtherChunk(
        const Block::Coordinate& localCoordinates);

    /**
     * Returns the position of the local block, located right next to the local block in the
     * indicated direction.
     * @param position Local block position
     * @param direction Direction next to which the block you are looking for is located
     * @return Block coordinate inside chunk
     */
    [[nodiscard]] Block::Coordinate localNearbyBlockPosition(const Block::Coordinate& position,
                                                             const Direction& direction) const;

    /**
     * Returns the block that is close to it, in the direction determined relative to the block on
     * the local coordinates.
     * @param position Local block coordinates inside the chunk
     * @param direction Direction next to which the block you are looking for is located
     * @return Block reference inside chunk
     */
    [[nodiscard]] Block& localNearbyBlock(const Block::Coordinate& position,
                                          const Direction& direction);

    /**
     * Returns the block that is close to it, in the direction determined relative to the block on
     * the local coordinates.
     * @param position Local block coordinates inside the chunk
     * @param direction Direction next to which the block you are looking for is located
     * @return Block reference inside chunk
     */
    [[nodiscard]] const Block& localNearbyBlock(const Block::Coordinate& localCoordinates,
                                                const Direction& direction) const;

    /**
     * @brief Returns the position on the block scale.
     * @return Coordinates of the block of the beginning of the chunk.
     */
    const Block::Coordinate& positionInBlocks() const;

private:
    Chunk(std::shared_ptr<ChunkBlocks> chunkBlocks, Block::Coordinate blockPosition,
          const TexturePack& texturePack, ChunkContainer& parent, ChunkManager& manager);

    /**
     * Generates natural world terrain on a given chunk
     */
    void generateChunkTerrain();

    /**
     * Rebuilds other chunks inside the same container around this chunk.
     */
    void rebuildChunksAround();

    /**
     * It checks whether a given block face has an "air" or other transparent face
     * next to it through which it can be seen at all.
     *
     * @param blockFace The face of the block to check
     * @param blockPos Local position of the block to be checked
     * @return Yes if the face next to the face of the indicated block is
     * transparent by which it is visible, false in the real case
     */
    [[nodiscard]] bool doesBlockFaceHasTransparentNeighbor(const Block::Face& blockFace,
                                                           const Block::Coordinate& blockPos);


    /**
     * @brief Checks whether a given block face is in contact with a block of the specified type
     * @param blockFace Face of the block to be checked
     * @param blockPos Block position to be checked
     * @param blockId Block identifier
     * @return
     */
    [[nodiscard]] bool doesBlockFaceHasGivenBlockNeighbour(const Block::Face& blockFace,
                                                           const Block::Coordinate& blockPos,
                                                           const BlockId& blockId);

    /**
     * Creates a block mesh on the indicated local coordinates
     * @param pos The indicated position of the block on which the mesh should be created
     */
    void createBlockMesh(const Block::Coordinate& pos);


private:
    mutable std::recursive_mutex mChunkAccessMutex;
    mutable std::recursive_mutex mModelsAccessMutex;

    std::unique_ptr<TerrainGenerator> mTerrainGenerator;
    Block::Coordinate mChunkPosition;
    const TexturePack& mTexturePack;

    ChunkContainer* const mParentContainer;
    ChunkManager* const mChunkManager;


    // TODO: This system should be changed to a better one. Consider distance.
    MeshBuilder mTerrainMeshBuilder;
    MeshBuilder mFluidMeshBuilder;
    MeshBuilder mFloralMeshBuilder;

    std::unique_ptr<Model3D> mTerrainModel;
    std::unique_ptr<Model3D> mFluidModel;
    std::unique_ptr<Model3D> mFloralModel;

    std::shared_ptr<ChunkBlocks> mChunkOfBlocks;
};
