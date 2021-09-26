#include "pch.h"
#include "Chunk.h"

#include <utility>

#include "ChunkContainer.h"
#include "MeshBuilder.h"
#include "Resources/TexturePack.h"
#include "World/Block/BlockType.h"

Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack, ChunkContainer& parent)
	: Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack, parent)
{

}

Chunk::Chunk(sf::Vector3i pixelPosition, const TexturePack& texturePack)
	: Chunk(Block::Coordinate::nonBlockToBlockMetric(pixelPosition), texturePack)
{
}

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePack& texturePack, ChunkContainer& parent)
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
	for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
	for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
	for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
	{
		// for test purposes
		if (y == BLOCKS_PER_DIMENSION - 1)
			(*mChunkOfBlocks)[x][y][z] = std::make_unique<Block>("Grass");
		else if (y > BLOCKS_PER_DIMENSION - 5)
			(*mChunkOfBlocks)[x][y][z] = std::make_unique<Block>("Dirt");
		else
			(*mChunkOfBlocks)[x][y][z] = std::make_unique<Block>("Stone");

	}
}

void Chunk::createBlockMesh(const Block::Coordinate& pos)
{
	const auto& block = getLocalBlock(pos);

	for(auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
	{
		if(faceHasTransparentNeighbor(static_cast<Block::Face>(i), pos))
		{
			mMeshBuilder.addQuad(static_cast<Block::Face>(i), 
				mTexturePack.getNormalizedCoordinates(block.getBlockTextureId(static_cast<Block::Face>(i))), pos);
		}
	}
	
}

void Chunk::prepareMesh()
{
	for (auto x = 0; x < BLOCKS_PER_DIMENSION; ++x)
	for (auto y = 0; y < BLOCKS_PER_DIMENSION; ++y)
	for (auto z = 0; z < BLOCKS_PER_DIMENSION; ++z)
	{
		if (getLocalBlock({ x, y, z }).getBlockId() == "Air")
			continue;

		createBlockMesh({ x, y, z });
	}
}

void Chunk::updateMesh()
{
	mModel.setMesh(mMeshBuilder.getMesh3D());
}

void Chunk::update(const float& deltaTime)
{
}

bool Chunk::areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates)
{
	if( localCoordinates.x < BLOCKS_PER_DIMENSION && localCoordinates.x >= 0 &&
		localCoordinates.y < BLOCKS_PER_DIMENSION && localCoordinates.y >= 0 &&
		localCoordinates.z < BLOCKS_PER_DIMENSION && localCoordinates.z >= 0)
	{
		return true;
	}

	return false;
}

bool Chunk::isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates)
{
	if (localCoordinates.x == BLOCKS_PER_DIMENSION-1 || localCoordinates.x == 0 ||
		localCoordinates.y == BLOCKS_PER_DIMENSION-1 || localCoordinates.y == 0 ||
		localCoordinates.z == BLOCKS_PER_DIMENSION-1 || localCoordinates.z == 0)
	{
		return true;
	}

	return false;
}

void Chunk::markToBeRebuildSlow() const
{
	if(mParentContainer)
	{
		if (auto foundChunk = mParentContainer->chunks.find(ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition)); 
				foundChunk != mParentContainer->chunks.end())
		{
			mParentContainer->chunksToRebuildQueue.push_back(foundChunk->second);
		}
	}
}

void Chunk::markToBeRebuildFast() const
{
	if (mParentContainer)
	{
		if (auto foundChunk = mParentContainer->chunks.find(ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition));
				foundChunk != mParentContainer->chunks.end())
		{
			mParentContainer->chunksToRebuildQueue.push_front(foundChunk->second);
		}
	}
}

void Chunk::rebuildMesh()
{
	static std::mutex rebuildMeshMutex;
	std::lock_guard _(rebuildMeshMutex);
	mMeshBuilder.resetMesh();
	prepareMesh();
}

void Chunk::rebuildChunksAround()
{
	for (auto direction = static_cast<int>(Direction::None) + 1; direction < static_cast<int>(Direction::Counter); ++direction)
	{
		if (const auto chunk = getChunk(static_cast<Direction>(direction)))
		{
			chunk->markToBeRebuildSlow();
		}
	}
}

void Chunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
	(*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z]->setBlockType("Air");

	markToBeRebuildFast();
}

Block::Coordinate Chunk::globalToLocalCoordinates(const Block::Coordinate& globalPosition) const
{
	return globalPosition - mChunkPosition;
}

Block& Chunk::getLocalBlock(const Block::Coordinate& position)
{
	return const_cast<Block&>(static_cast<const Chunk&>(*this).getLocalBlock(position));
}

Block& Chunk::getLocalBlock(const Block::Coordinate& position, const Direction& direction)
{
	return const_cast<Block&>(static_cast<const Chunk&>(*this).getLocalBlock(position, direction));
}

const Block& Chunk::getLocalBlock(const Block::Coordinate& localCoordinates) const
{
	return *(*mChunkOfBlocks)[localCoordinates.x][localCoordinates.y][localCoordinates.z];
}

const Block& Chunk::getLocalBlock(const Block::Coordinate& localCoordinates, const Direction& direction) const
{
	return getLocalBlock(getLocalBlockPosition(localCoordinates, direction));
}

Block::Coordinate Chunk::getLocalBlockPosition(const Block::Coordinate& position, const Direction& direction) const
{
	switch (direction)
	{
	case Direction::Above: 
		return { position.x, position.y + 1, position.z };
	case Direction::Below: 
		return { position.x, position.y - 1, position.z };
	case Direction::ToTheLeft: 
		return { position.x - 1, position.y, position.z };
	case Direction::ToTheRight: 
		return { position.x + 1, position.y, position.z };
	case Direction::InFront: 
		return { position.x, position.y, position.z + 1};
	case Direction::Behind: 
		return { position.x, position.y, position.z - 1};
	}
}

std::shared_ptr<Chunk> Chunk::getChunk(const Direction& direction)
{
	switch (direction)
	{
	case Direction::Above:
		return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({ 0, BLOCKS_PER_DIMENSION, 0 }));
	case Direction::Below:
		return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({ 0, -1, 0 }));
	case Direction::ToTheLeft:
		return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({ -1, 0, 0 }));
	case Direction::ToTheRight:
		return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({ BLOCKS_PER_DIMENSION, 0, 0 }));
	case Direction::InFront:
		return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({ 0, 0, BLOCKS_PER_DIMENSION }));
	case Direction::Behind:
		return mParentContainer->blockPositionToChunk(localToGlobalCoordinates({ 0, 0, -1 }));
	}
}

Chunk::Chunk(std::shared_ptr<ChunkBlocks> chunkBlocks, Block::Coordinate blockPosition, const TexturePack& texturePack, ChunkContainer& parent)
	: mChunkPosition(std::move(blockPosition))
	, mTexturePack(texturePack)
	, mParentContainer(&parent)
	, mMeshBuilder(mChunkPosition)
	, mChunkOfBlocks(std::move(chunkBlocks))
{
	prepareMesh();
}

Block::Coordinate Chunk::localToGlobalCoordinates(const Block::Coordinate& position) const
{
	return mChunkPosition + position;
}

std::vector<Direction> Chunk::getDirectionOfBlockFacesInContactWithOtherChunk(const Block::Coordinate& localCoordinates)
{
	std::vector<Direction> directions;

	if (localCoordinates.x == BLOCKS_PER_DIMENSION - 1)
		directions.emplace_back(Direction::ToTheRight);
	if (localCoordinates.x == 0)
		directions.emplace_back(Direction::ToTheLeft);
	if (localCoordinates.y == BLOCKS_PER_DIMENSION - 1)
		directions.emplace_back(Direction::Above);
	if (localCoordinates.y == 0)
		directions.emplace_back(Direction::Below);
	if (localCoordinates.z == BLOCKS_PER_DIMENSION - 1)
		directions.emplace_back(Direction::InFront);
	if (localCoordinates.z == 0)
		directions.emplace_back(Direction::Behind);

	return directions;
}

bool Chunk::faceHasTransparentNeighbor(const Block::Face& face, const Block::Coordinate& blockPos)
{
	auto isBlockTransparent = [&blockPos, this](const Direction& face)
	{
		const auto blockNeighborPosition = getLocalBlockPosition(blockPos, face);
		if (areLocalCoordinatesInsideChunk(blockNeighborPosition))
			return (getLocalBlock(blockNeighborPosition).isTransparent());

		if (belongsToContainer())
		{
			if (const auto& neighborBlock = mParentContainer->getWorldBlock(
							localToGlobalCoordinates(blockNeighborPosition)))
			{
				return neighborBlock->isTransparent();
			}
		}

		return true;
	};
	
	switch(face)
	{
	case Block::Face::Top:
		return (isBlockTransparent(Direction::Above));
	case Block::Face::Left:
		return (isBlockTransparent(Direction::ToTheLeft));
	case Block::Face::Right:
		return (isBlockTransparent(Direction::ToTheRight));
	case Block::Face::Bottom:
		return (isBlockTransparent(Direction::Below));
	case Block::Face::Front:
		return (isBlockTransparent(Direction::InFront));
	case Block::Face::Back:
		return (isBlockTransparent(Direction::Behind));
	}
	
}

bool Chunk::belongsToContainer() const
{
	return (mParentContainer != nullptr);
}

void Chunk::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	mModel.draw(renderer3d, shader);
}
