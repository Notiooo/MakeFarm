#pragma once
#include "World/Camera.h"
#include <World/Chunks/ChunkManager.h>
#include <optional>


class Renderer3D;
class Model3D;
class ChunkContainer;


/**
 * \brief Creates a block pointed to by the current camera inside the specified ChunkContainer
 */
class HighlightedBlock
{
public:
    HighlightedBlock();

    constexpr static auto BLOCKS_THAT_MIGHT_BE_OVERPLACED = {BlockId::Air, BlockId::Water};

    /**
     * Indicates whether any block is currently highlighted
     * @return True if yes, false otherwise
     */
    [[nodiscard]] bool isAnyBlockHighlighted() const;

    /**
     * Returns the position of the highlighted block
     * @return Position of the highlighted block
     * @warning If no block is highlighted it throws a runtime_error.
     */
    Block::Coordinate blockPosition() const;

    /**
     * @brief Returns the face of the block the player is currently looking at
     * @return Face of the block the player is currently looking at
     * @warning If no block is highlighted it throws a runtime_error.
     */
    Block::Face blockFace() const;

    /**
     * \brief Updates the Faced Block logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime, Camera& camera, ChunkManager& chunkManager);

    /**
     * Draws this block to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param wireframeShader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer3D& renderer3d, const sf::Shader& wireframeShader) const;

private:
    /**
     * The maximum ray length to which blocks in front of the camera should be checked.
     */
    static constexpr float MAX_RAY_SIZE = 6 * Block::BLOCK_SIZE;

    /**
     * Highlights the block that the indicated camera is looking at and that is inside the indicated
     * ChunkContainer.
     * @param camera The camera on the basis of which are looking for the block to which it points
     * @param chunkManager ChunkContainer, which contains the chunk with the block we are looking at
     */
    void markFacedBlock(const Camera& camera, const ChunkManager& chunkManager);

    /**
     * @brief Creates a mesh of the currently highlighted block
     * @param facedBlock Coordinates of the highlighted block
     * @return 3D Mesh of the highlighted block on the given coordinates
     */
    [[nodiscard]] std::unique_ptr<Mesh3D> createMeshOfHighlightedBlock(
        const Block::Coordinate& facedBlock) const;

    /**
     * @brief Creates a 3D model of the highlighted block at the indicated position.
     * @param facedBlock Coordinates of the highlighted block
     * @return 3D Model of the highlighted block on the given coordinates
     */
    Model3D createWireframeBlock(const Block::Coordinate& facedBlock);

    /**
     * @brief It dehiglights the currently selected block, so that nothing is highlighted anymore.
     */
    void dehighlightCurrentlySelectedBlock();

    /**
     * @brief Creates a buffer layout describing the mesh of the highlighted block
     * @return Buffer layout describing the mesh of the highlighted block
     */
    [[nodiscard]] BufferLayout createBufferLayout() const;

    /**
     * @brief Returns the local coordinate of a point inside the very center of the indicated block
     * face.
     * @param blockFace The face of the block whose point is to be placed in the middle.
     * @return Local coordinate of a point inside the very center of the indicated block.
     */
    [[nodiscard]] sf::Vector3f localPointInCenterOfBlockFace(const Block::Face& blockFace) const;

    /**
     * @brief Determines which face of the block is closest to the local point inside the block.
     * @param localPointInsideTheBlock Local coordinates located inside the block.
     * @return The face of the block that is closest to the local coordinates inside the block.
     */
    [[nodiscard]] Block::Face closestFaceToLocalBlockPoint(
        const sf::Vector3<float>& localPointInsideTheBlock) const;

    /**
     * @brief Determines the face of the block that is closest to the point with global non-metric
     * coordinates for the block with the indicated coordinates inside which the point is located.
     * @param nonMetricPoint Non-metric point coordinate in the game world.
     * @param blockAtPointPosition Block coordinates of the block inside which the point is located.
     * @return The face of the block that is closest to the indicated point inside the block.
     */
    [[nodiscard]] Block::Face blockFaceClosestToThePoint(
        const sf::Vector3<float>& nonMetricPoint,
        const Block::Coordinate& blockAtPointPosition) const;

    /**
     * @brief Attempts to highlight a block with the indicated coordinates touched by a ray with the
     * indicated coordinates. Backlighting can fail if the block is not highlightable - for example,
     * it is air or water block.
     * @param positionAtTheEndOfRay Position at the end of the ray that hit the block.
     * @param blockAtPositionOfTheRay Coordinate the block that was hit by the ray.
     * @param block A block that was hit by a ray.
     * @return True if the block was successfully highlighted, false otherwise.
     */
    bool tryHighlightBlock(const sf::Vector3<float>& positionAtTheEndOfRay,
                           const Block::Coordinate& blockAtPositionOfTheRay, const Block& block);

    /**
     * @brief Determines whether the block is highlightable. For example, air or water are not.
     * @param block Block to determine whether it is highlightable.
     * @return True if it is highlightable, false otherwise.
     */
    [[nodiscard]] static bool isBlockHighlightable(const Block& block);

private:
    /**
     * @brief The structure describes the highlighted block selected by the player.
     */
    struct SelectedBlock
    {
        SelectedBlock(Model3D&& wireframeModel3d, const Block::Coordinate& positionOfBlock,
                      const Block::Face& faceOfBlock);

        Model3D model3D;
        Block::Coordinate position;
        Block::Face face;
    };

    std::optional<SelectedBlock> mSelectedBlock;
    sf::Shader mWireframeShader;
};
