#include "pch.h"
#include "FacedBlock.h"

#include "World/Camera.h"
#include "World/Chunks/ChunkContainer.h"
#include "World/Chunks/MeshBuilder.h"

void FacedBlock::markFacedBlock(const Camera& camera, const ChunkContainer& chunkContainer)
{
	static auto rayStep = Block::BLOCK_SIZE * 0.01f;
	for (float raySize = rayStep; raySize < Camera::MAX_RAY_SIZE; raySize += rayStep)
	{
		auto facedPosition = camera.getPointingPosition<float>(raySize);

		/*
		 * Block origin is in left bottom corner of cube. Now think about it what happens
		 * when we look at specific point in place. Lets assume BlockSize is 1 per side.
		 * y =  2.16 -> 2 as an int
		 * y =  1.90 -> 1 as an int
		 * y =  0.46 -> 0 as an int  <----
		 * y = -0.16 -> 0 as an int  <----
		 * y = -1.26 -> -1 as an int
		 *
		 * Ahh not good! There is small miss-align here. Thats why the correction below
		 * is needed!
		 */
		facedPosition.x = (facedPosition.x < 0) ? facedPosition.x - Block::BLOCK_SIZE : facedPosition.x;
		facedPosition.y = (facedPosition.y < 0) ? facedPosition.y - Block::BLOCK_SIZE : facedPosition.y;
		facedPosition.z = (facedPosition.z < 0) ? facedPosition.z - Block::BLOCK_SIZE : facedPosition.z;

		auto facedBlock = Block::Coordinate::nonBlockToBlockMetric(facedPosition);

		if (const auto blockPtr = chunkContainer.getWorldBlock(facedBlock))
		{
			if (blockPtr->getBlockId() != "Air")
			{
				const Block::Coordinate& blockPosition = std::move(facedBlock);
				MeshBuilder cubeBuilder(Block::Coordinate(0, 0, 0));
				cubeBuilder.setFaceSize(static_cast<float>(Block::BLOCK_SIZE) * 1.05f);
				for (int face = 0; face < static_cast<int>(Block::Face::Counter); ++face)
				{
					cubeBuilder.addQuad(static_cast<Block::Face>(face),
						mTexturePack.getNormalizedCoordinates(5), blockPosition);
				}
				mSelectedBlock = std::make_unique<Model3D>();
				mSelectedBlock->setMesh(cubeBuilder.getMesh3D());
				mSelectedBlockPosition = facedBlock;
				return;
			}
		}
	}

	// If there was no particular block to highlight
	// then sadly it should be unhighlight
	if (mSelectedBlock)
	{
		mSelectedBlock.reset();
	}
}

Block::Coordinate& FacedBlock::getBlockPosition()
{
	/* 
	 * TODO: Consider if this error is a good practice
	 * This function may return a nullptr, but... for now
	 * it provides isAnyBlockSelected() to avoid this error
	 */
	if (!isAnyBlockSelected())
		throw std::runtime_error("No block is selected. Can not return position of selected block");

	return mSelectedBlockPosition;
}

void FacedBlock::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	if (isAnyBlockSelected())
	{
		mSelectedBlock->draw(renderer3d, shader);
	}
}

FacedBlock::FacedBlock(const TexturePack& texturePack)
	: mTexturePack(texturePack)
{

}

bool FacedBlock::isAnyBlockSelected() const
{
	return mSelectedBlock != nullptr;
}
