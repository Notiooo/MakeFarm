#include "pch.h"
#include "ChunkContainer.h"

#include "CoordinatesAroundOriginGetter.h"
#include "Resources/TexturePack.h"
#include "World/Block/BlockMap.h"

ChunkContainer::ChunkContainer(const TexturePack& texturePack)
	: mTexturePack(texturePack)
{
	
}

void ChunkContainer::draw(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
	mTexturePack.bind();
	for (auto& [coordinate, chunk] : mChunks)
		chunk->draw(renderer3D, shader);
}

void ChunkContainer::rebuildImportantChunks()
{
	for(const auto& chunkToRebuild : mChunksToRebuildFast)
	{
		chunkToRebuild->rebuildMesh();
		chunkToRebuild->updateMesh();
	}
	mChunksToRebuildFast.clear();
}

void ChunkContainer::rebuildInsignificantChunks()
{
	static auto rebuildMeshes = [](std::vector<std::shared_ptr<Chunk>> chunks)
	{
		for (const auto& chunk : chunks)
		{
			chunk->rebuildMesh();
		}
		return chunks;
	};

	if(!mChunksToRebuildSlow.empty())
	{
		std::scoped_lock guard(mChunksToRebuildSlowAccessMutex);
		mChunkCreateMeshProcessesSlow.emplace_back(
			std::async(std::launch::async, std::ref(rebuildMeshes), std::move(mChunksToRebuildSlow)));

		mChunksToRebuildSlow.clear();
	}
}

void ChunkContainer::updateInsignificantChunksMeshes()
{
	/*
	 * applyMesh uses the various calls to OpenGL directly.
	 * As we know OpenGL is a state machine and it's not very
	 * nice to execute it asynchronously, so the buffer swap
	 * of chunks is executed synchronously as it's fast anyway
	 */

	for (auto chunkVectorIterator = mChunkCreateMeshProcessesSlow.begin(),
	          endChunkIterator = mChunkCreateMeshProcessesSlow.end();
	     chunkVectorIterator != endChunkIterator; )
	{
		if (chunkVectorIterator->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			auto chunkVector = chunkVectorIterator->get();
			for(const auto& chunk : chunkVector)
			{
				chunk->updateMesh();
			}
			mChunkCreateMeshProcessesSlow.erase(chunkVectorIterator++);
		}
		else
		{
			++chunkVectorIterator;
		}
	}
}

void ChunkContainer::rebuildChunks()
{
	rebuildImportantChunks();
	rebuildInsignificantChunks();
	updateInsignificantChunksMeshes();
}

void ChunkContainer::fixedUpdate(const float& deltaTime)
{
	rebuildChunks();
}

sf::Vector3i ChunkContainer::Coordinate::nonChunkMetric() const
{
	return sf::Vector3i(
		x * Chunk::CHUNK_WALL_SIZE,
		y * Chunk::CHUNK_WALL_SIZE,
		z * Chunk::CHUNK_WALL_SIZE
		);
}

const Block* ChunkContainer::worldBlock(const Block::Coordinate& worldBlockCoordinates) const
{
	if(const auto chunk = blockPositionToChunk(worldBlockCoordinates))
	{
		return &chunk->localBlock(chunk->globalToLocalCoordinates(worldBlockCoordinates));
	}
	return nullptr;
}

bool ChunkContainer::doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const
{
	std::shared_lock guard(mChunksAccessMutex);
	const auto foundChunk = mChunks.find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
	return foundChunk != mChunks.cend();
}

std::shared_ptr<const Chunk> ChunkContainer::blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates) const
{
	std::shared_lock guard(mChunksAccessMutex);
	const auto foundChunk = mChunks.find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
	if(foundChunk != mChunks.cend())
	{
		return foundChunk->second;
	}
	return nullptr;
}

std::shared_ptr<Chunk> ChunkContainer::blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates)
{
	return std::const_pointer_cast<Chunk>(static_cast<const ChunkContainer&>(*this).blockPositionToChunk(worldBlockCoordinates));
}


void ChunkContainer::removeWorldBlock(const Block::Coordinate& worldBlockCoordinates)
{
	if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
	{
		auto localCoordinates = chunk->globalToLocalCoordinates(worldBlockCoordinates);
		chunk->removeLocalBlock(localCoordinates);

		/*
		 * When removing a block, you may find that it is in contact with an adjacent chunk.
		 * Rebuilding one chunk doesn't help, because the neighboring chunk remains in the form
		 * where it assumes the block is there. This leads to a hole in the chunk. For this reason,
		 * you must rebuild all chunks that are in contact with the block being removed.
		 */
		const auto directions = chunk->directionOfBlockFacesInContactWithOtherChunk(localCoordinates);
		for (auto& blockDirection : directions)
		{
			auto neighboringBlockInOtherChunk = chunk->localToGlobalCoordinates(
                    chunk->localNearbyBlockPosition(localCoordinates, blockDirection));

			if (const auto neighboringChunk = blockPositionToChunk(neighboringBlockInOtherChunk))
			{
				neighboringChunk->markToBeRebuildFast();
			}
		}
	}
}

void ChunkContainer::generateChunksAround(const Camera& camera)
{
	const auto cameraPosition = camera.cameraPosition();
	const auto currentChunkOfCamera = ChunkContainer::Coordinate::blockToChunkMetric(
		Block::Coordinate::nonBlockToBlockMetric(sf::Vector3i(cameraPosition.x, cameraPosition.y, cameraPosition.z)));

	static std::future<void> chunksThatDoesNotExistProcesses = std::async(std::launch::async, 
		&ChunkContainer::generatesNewChunksAtAndAroundOrigin, this, currentChunkOfCamera);

	if (chunksThatDoesNotExistProcesses.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		chunksThatDoesNotExistProcesses = std::async(std::launch::async, 
			&ChunkContainer::generatesNewChunksAtAndAroundOrigin, this, currentChunkOfCamera);
	}
}

void ChunkContainer::clearFarAwayChunks(const Camera& camera)
{
	std::vector<decltype(mChunks)::key_type> coordinateOfChunksToDelete;

	const auto cameraPosition = camera.cameraPosition();
	const auto currentChunkOfCamera = ChunkContainer::Coordinate::blockToChunkMetric(
		Block::Coordinate::nonBlockToBlockMetric(sf::Vector3i(cameraPosition.x, cameraPosition.y, cameraPosition.z)));

	/*
	 * In the current drawing stage, chunks are not drawn according to the distance from the camera.
	 * It is easier to imagine a cube around the camera with side length 2 * WORLD_GENERATION_CHUNK_DISTANCE.
	 * It is in its area that the chunks are drawn. Here it should be checked in the same way.
	 * I don't want a situation where a barely created chunk is immediately deleted.
	 */
	std::shared_lock guard_shared(mChunksAccessMutex);
	for(auto& [coordinate, chunk] : mChunks)
	{
		sf::Vector3i distanceBetweenCameraAndChunk;

		distanceBetweenCameraAndChunk.x = std::abs(coordinate.x - currentChunkOfCamera.x);
		distanceBetweenCameraAndChunk.y = std::abs(coordinate.y - currentChunkOfCamera.y);
		distanceBetweenCameraAndChunk.z = std::abs(coordinate.z - currentChunkOfCamera.z);

		if(distanceBetweenCameraAndChunk.x > WORLD_GENERATION_CHUNK_DISTANCE+1 || 
			distanceBetweenCameraAndChunk.y > WORLD_GENERATION_CHUNK_DISTANCE+1 ||
			distanceBetweenCameraAndChunk.z > WORLD_GENERATION_CHUNK_DISTANCE+1)
		{
			coordinateOfChunksToDelete.emplace_back(coordinate);
		}
	}
	guard_shared.unlock();

	std::scoped_lock guard(mChunksAccessMutex);
	for(auto& coordinate : coordinateOfChunksToDelete)
	{
		mChunks.erase(coordinate);
	}
}

void ChunkContainer::generateChunk(ChunkContainer::Coordinate chunkPosition)
{
	auto newChunk = std::make_shared<Chunk>(sf::Vector3i(chunkPosition.nonChunkMetric()), mTexturePack, *this);

	std::scoped_lock guard(mChunksAccessMutex);
	mChunks.emplace(chunkPosition, std::move(newChunk));
}

void ChunkContainer::generatesNewChunksAtAndAroundOrigin(const ChunkContainer::Coordinate& origin)
{
	auto chunkPositionGetter = CoordinatesAroundOriginGetter(origin);
	auto globalAddedChunks = 0;
	auto currentHeight = 0;

	// 1 Layer below, and another above in Y axis
	for (auto heightLayer = 0; heightLayer < WORLD_GENERATION_CHUNK_DISTANCE * 2; ++heightLayer)
	{
		auto addedChunks = 0;

		// Plane of side 2 * WORLD_GENERATION_CHUNK_DISTANCE
		// Creates a single plane of chunks in specific point of Y axis
		for (auto i = 0; i < WORLD_GENERATION_CHUNK_DISTANCE * WORLD_GENERATION_CHUNK_DISTANCE * 2 * 2; ++i)
		{
			auto nextChunkPosition = chunkPositionGetter.nextValue();
			const bool isAdded = generateChunkIfNotExist({ nextChunkPosition.x, currentHeight + origin.y, nextChunkPosition.z });
			addedChunks += isAdded;
			globalAddedChunks += isAdded;

			/*
			 * If creates a large number of chunks in this dimension,
			 * it already moves on to the next dimension to draw all
			 * dimensions equally and quickly
			 */
			if (addedChunks == MAX_NEW_CHUNKS_PER_HEIGHT_AT_ONCE)
				break;

			/*
			 * If it draws too many chunks, it stops drawing at further
			 * distances so that the sketches do not take too long to
			 * create it at once.
			 */
			if (globalAddedChunks == MAX_NEW_CHUNK_AT_ONCE)
				return;
		}
		// Further heights are: 1, -1, 2, -2, 3, -3... etc
		currentHeight = -currentHeight;
		if (currentHeight >= 0)
			++currentHeight;

		chunkPositionGetter.resetToStart();
	}
}

bool ChunkContainer::generateChunkIfNotExist(ChunkContainer::Coordinate chunkPosition)
{
	std::shared_lock guard(mChunksAccessMutex);
	if(mChunks.find(chunkPosition) == mChunks.end())
	{
		guard.unlock();
		generateChunk(chunkPosition);
		return true;
	}
	return false;
}
