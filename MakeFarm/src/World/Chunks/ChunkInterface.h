#pragma once
#include "Renderer3D/Renderer3D.h"
#include "Utils/MultiDimensionalArray.h"
#include "World/Block/Block.h"
#include "World/Chunks/RebuildOperation.h"
#include <memory>

/**
 * Chunk interface through which it is possible to mock chunk.
 */
class ChunkInterface
{
public:
    virtual ~ChunkInterface() = default;

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
    virtual void prepareMesh() = 0;

    /**
     * \brief Swaps the current chunk mesh with the latest, most recently generated one
     */
    virtual void updateMesh() = 0;

    /**
     * Updates the status/logic of the state at equal intervals independent of the frame rate.
     * @param deltaTime Time interval
     */
    virtual void fixedUpdate(const float& deltaTime) = 0;

    /**
     * Draws this chunk terrain to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    virtual void drawTerrain(const Renderer3D& renderer3d, const sf::Shader& shader) const = 0;

    /**
     * Draws this chunk liquids to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    virtual void drawLiquids(const Renderer3D& renderer3d, const sf::Shader& shader) const = 0;

    /**
     * Draws this chunk floral to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    virtual void drawFlorals(const Renderer3D& renderer3d, const sf::Shader& shader) const = 0;

    /**
     * \brief Returns the block according to the coordinates given relative to the chunk position.
     * \param localCoordinates Position in relation to the chunk
     * \return Block reference inside chunk
     */
    virtual Block& localBlock(const Block::Coordinate& localCoordinates) = 0;

    /**
     * \brief Removes a block on coordinates given relatively to the position of the chunk
     * \param localCoordinates Coordinates relative to the position of the chunk
     */
    virtual void removeLocalBlock(const Block::Coordinate& localCoordinates) = 0;

    /**
     * \brief It tries to insert the indicated block in the indicated place in the chunk.
     * \param blockId Block Identifier
     * \param localCoordinates Coordinates relative to the position of the chunk
     * \param blocksThatMightBeOverplaced Blocks that can be overwritten by a function. Others
     * cannot be overwritten.
     * \param rebuildOperation An operation that determines whether and how quickly a chunk should
     * be rebuilt after a block is placed.
     */
    virtual void tryToPlaceBlock(
        const BlockId& blockId, const Block::Coordinate& localCoordinates,
        std::vector<BlockId> blocksThatMightBeOverplaced = {BlockId::Air},
        const RebuildOperation& rebuildOperation = RebuildOperation::None) = 0;

    /**
     * \brief Returns the block according to the coordinates given relative to the chunk position.
     * \param localCoordinates Position in relation to the chunk
     * \return Block reference inside chunk
     */
    [[nodiscard]] virtual const Block& localBlock(
        const Block::Coordinate& localCoordinates) const = 0;

    /**
     * \brief Changes global world coordinates to local ones relative to chunk
     * \param worldCoordinates World coordinates of the block
     * \return Local coordinates of block relative to chunk
     */
    [[nodiscard]] virtual Block::Coordinate globalToLocalCoordinates(
        const Block::Coordinate& worldCoordinates) const = 0;

    /**
     * \brief Changes local chunk coordinates to global ones inside the world
     * \param localCoordinates Local coordinates of the block
     * \return World coordinates of block inside the world
     */
    [[nodiscard]] virtual Block::Coordinate localToGlobalCoordinates(
        const Block::Coordinate& localCoordinates) const = 0;

    /**
     * \brief Checks that the given local coordinates relative to the chunk are at its extremity.
     * \param localCoordinates Local coordinates relative to chunk
     * \return True if the coordinates are on the edge of the chunk, false otherwise
     */
    [[nodiscard]] virtual bool isLocalCoordinateOnChunkEdge(
        const Block::Coordinate& localCoordinates) = 0;

    /**
     * It is rebuilding this mesh fresh. Very expensive operation
     */
    virtual void rebuildMesh() = 0;

    /**
     * \brief Indicates chunk as willing to rebuild mesh in near future
     */
    virtual void rebuildSlow() = 0;

    /**
     * \brief Indicates chunk as willing to rebuild mesh in this, or next frame
     */
    virtual void rebuildFast() = 0;

    /**
     * Returns information whether any chunk is in contact with the listed block. This is important
     * information when a given block has been destroyed and there is a need to rebuild the chunk
     * next to it so that it does not display an empty hole.
     * @param localCoordinates Local block coordinates of the block to be checked
     * @return Information on whether the block is in contact with another chunk
     */
    virtual std::vector<Direction> directionOfBlockFacesInContactWithOtherChunk(
        const Block::Coordinate& localCoordinates) = 0;

    /**
     * Returns the position of the local block, located right next to the local block in the
     * indicated direction.
     * @param position Local block position
     * @param direction Direction next to which the block you are looking for is located
     * @return Block coordinate inside chunk
     */
    [[nodiscard]] virtual Block::Coordinate localNearbyBlockPosition(
        const Block::Coordinate& position, const Direction& direction) const = 0;

    /**
     * Returns the block that is close to it, in the direction determined relative to the
     * block on the local coordinates.
     * @param position Local block coordinates inside the chunk
     * @param direction Direction next to which the block are looking for is located
     * @return Block reference inside chunk
     */
    [[nodiscard]] virtual Block& localNearbyBlock(const Block::Coordinate& position,
                                                  const Direction& direction) = 0;

    /**
     * Returns the block that is close to it, in the direction determined relative to the block on
     * the local coordinates.
     * @param position Local block coordinates inside the chunk
     * @param direction Direction next to which the block are looking for is located
     * @return Block reference inside chunk
     */
    [[nodiscard]] virtual const Block& localNearbyBlock(const Block::Coordinate& localCoordinates,
                                                        const Direction& direction) const = 0;

    /**
     * @brief Returns the position on the block scale.
     * @return Coordinates of the block of the beginning of the chunk.
     */
    virtual const Block::Coordinate& positionInBlocks() const = 0;

    /**
     * @brief Finds a neighboring block located in the indicated direction
     * @param blockPos Position of the block for which the neighbor is sought
     * @param direction Direction from the block for which the neighbor is sought
     * @return Block if it exists, or nullopt if no such block exists.
     */
    virtual std::optional<Block> neighbourBlockInGivenDirection(const Block::Coordinate& blockPos,
                                                                const Direction& direction) = 0;

    /**
     * @brief Returns the highest located block in a given column with x, z coordinates.
     * @param blockPos Column of block at given x, z coordinates.
     * @return The highest located block in a given column of blocks.
     */
    virtual Block::Coordinate highestSetBlock(const Block::Coordinate& blockPos) = 0;
};
