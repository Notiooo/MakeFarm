#pragma once
#include "World/Camera.h"
#include <World/Chunks/ChunkManager.h>


class Renderer3D;
class Model3D;
class ChunkContainer;


/**
 * \brief Creates a block pointed to by the current camera inside the specified ChunkContainer
 */
class FacedBlock
{
public:
    FacedBlock(const TexturePack& texturePack);

    /**
     * Indicates whether any block is currently highlighted
     * @return True if yes, false otherwise
     */
    [[nodiscard]] bool isAnyBlockSelected() const;

    /**
     * Highlights the block that the indicated camera is looking at and that is inside the indicated
     * ChunkContainer.
     * @param camera The camera on the basis of which we are looking for the block to which it
     * points
     * @param chunkManager ChunkContainter, which contains the chunk with the block we are looking
     * at
     */
    void markFacedBlock(const Camera& camera, const ChunkManager& chunkManager);

    /**
     * Returns the position of the highlighted block
     * @return Position of the highlighted block
     */
    Block::Coordinate& blockPosition();

    /**
     * Draws this block to the game screen
     * @param renderer3d Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer3D& renderer3d, const sf::Shader& shader) const;

private:
    std::unique_ptr<Model3D> mSelectedBlock;
    Block::Coordinate mSelectedBlockPosition{0, 0, 0};

    // TODO: Later on this class should not use any textures
    const TexturePack& mTexturePack;
};
