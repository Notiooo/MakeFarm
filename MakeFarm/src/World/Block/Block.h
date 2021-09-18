#pragma once
#include "Renderer3D/Model3D.h"
#include "Renderer3D/Renderer3D.h"
#include "Resources/TexturePack.h"
#include <cassert>

class BlockType;

class Block
{
public:

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
	 * If you expand the texture file linearly and count from left to
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
	struct Coordinate
	{
		using IntegerUnit = int;

		Coordinate(IntegerUnit x, IntegerUnit y, IntegerUnit z);
		Coordinate(const sf::Vector3i& blockCoordinates);

		Coordinate(Coordinate&&) noexcept;
		Coordinate(const Coordinate&) noexcept;

		operator sf::Vector3<IntegerUnit>() const { return mBlockCoordinates; }

		Coordinate operator-(const Coordinate& rhs) const;
		Coordinate operator+(const Coordinate& rhs) const;
		bool operator==(const Coordinate& rhs) const;
		bool operator!=(const Coordinate& rhs) const;


		/**
		 * \brief Converts the current position of a block to its positional equivalent in space.
		 *
		 * For example, if a block has dimension 16x16 and is at block
		 * coordinates x: 0, y: 4, z: 0 then its position in space is:
		 * { x: 0, y: 64, z: 0 }
		 *
		 * \return Positions in non-block-grid space
		 */
		[[nodiscard]] sf::Vector3<Block::SizeType> getNonBlockMetric() const;
		static Coordinate nonBlockToBlockMetric(const sf::Vector3i& nonBlockVector);


	private:
		sf::Vector3<IntegerUnit> mBlockCoordinates;

	public:
		IntegerUnit& x = mBlockCoordinates.x;
		IntegerUnit& y = mBlockCoordinates.y;
		IntegerUnit& z = mBlockCoordinates.z;
	};

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

	Block();
	Block(const std::string& blockName);

	void setBlockType(const std::string& blockName);
	[[nodiscard]] TextureId getBlockTextureId(const Block::Face& blockFace) const;
	[[nodiscard]] std::string getBlockId() const;
	[[nodiscard]] bool isTransparent() const;


private:
	const BlockType* blockType;
};