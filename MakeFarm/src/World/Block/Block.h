#pragma once

#include "BlockId.h"
#include "Utils/CoordinateBase.h"

class BlockType;
class TexturePack;

class Block
{
public:
    Block();
    Block(const BlockId& blockId);

    /**
     * \brief The type of variable that is used to define the side of the block
     */
    using SizeType = int;

    /**
     * \brief Length of side in the space the block has
     */
    static constexpr SizeType BLOCK_SIZE = 1;

    /**
     * \brief Texture ID that corresponds to the texture by the position it is in the file.
     *
     * If you expand the texture file linearly and size from left to
     * right starting from zero, the position of the texture will
     * correspond to its identifier.
     *
     * An example:
     * Each '#' is an example texture
     *
     * #####
     * #####
     * #####
     *    ^
     *	This texture has an identifier of 14
     *
     */
    using TextureId = unsigned int;


    /**
     * \brief A spatial unit measured in blocks.
     *
     * For example when I want to get the fourth block from the bottom
     * it would be {0, 4, 0}.
     *
     * It is useful to distinguish this unit from the usual spatial unit
     * where the block is at position {0, 4 * Block::BLOCK_SIZE, 0}.
     */
    struct Coordinate : public CoordinateBase
    {

        using CoordinateBase::CoordinateBase;

        /**
         * \brief Converts the current position of a block to its positional equivalent in space.
         *
         * For example, if a block has dimension 16x16 and is at block
         * coordinates x: 0, y: 4, z: 0 then its position in space is:
         * { x: 0, y: 64, z: 0 }
         *
         * \return Positions in non-block-grid space
         */
        [[nodiscard]] sf::Vector3<Block::SizeType> nonBlockMetric() const;

        template<typename T>
        static Coordinate nonBlockToBlockMetric(const sf::Vector3<T>& nonBlockVector)
        {
            // -0.50 ; 73.98 ; -0.07
            // -1    ; 73    ; -1

            static auto fastFloor = [](const float& numberToFloor)
            {
                const auto truncated = static_cast<int>(numberToFloor);
                return truncated - (truncated > numberToFloor);
            };

            return Block::Coordinate(
                static_cast<SizeType>(fastFloor(nonBlockVector.x)) / BLOCK_SIZE,
                static_cast<SizeType>(fastFloor(nonBlockVector.y)) / BLOCK_SIZE,
                static_cast<SizeType>(fastFloor(nonBlockVector.z)) / BLOCK_SIZE);
        };
    };

    /**
     * Defines the face of the block.
     * Each block consists of 6 faces. Often there is no need to draw more faces,
     * so this allows to be more precise about which face referring to
     */
    enum class Face
    {
        Top = 0,
        Left,
        Right,
        Bottom,
        Front,
        Back,

        Counter
    };

    /**
     * Sets the block's settings to match those in the cfg files of the resources/blocks folder
     * @param blockId The identifier of the block
     */
    void setBlockType(const BlockId& blockId);

    /**
     * Retrieves the ID of the texture that is on the given block face
     * @param blockFace Selected block face from which texture will be read
     * @return ID of the corresponding texture face
     */
    [[nodiscard]] TextureId blockTextureId(const Block::Face& blockFace) const;

    /**
     * Returns a block identifier
     * @return Identifier of the block
     */
    [[nodiscard]] BlockId blockId() const;

    /**
     * Returns information about whether the block is transparent. For example, it can be glass.
     * @return True if the block is transparent, false otherwise
     */
    [[nodiscard]] bool isTransparent() const;

    /**
     * Returns information about whether the block is collidable. For example, it can be water/air.
     * @return True if the block is collidable, false otherwise
     */
    [[nodiscard]] bool isCollidable() const;

    /**
     * @brief Returns information whether the object is a plant part of the environment, such as
     * leaves or flowers. Such things are drawn separately so that under their transparency the area
     * behind them should be visible.
     */
    [[nodiscard]] bool isFloral() const;


private:
    const BlockType* mBlockType;
};
