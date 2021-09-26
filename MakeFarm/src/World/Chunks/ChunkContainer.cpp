#include "pch.h"
#include "ChunkContainer.h"

#include "Resources/TexturePack.h"
#include "World/Block/BlockMap.h"

ChunkContainer::ChunkContainer(const TexturePack& texturePack)
	: texturePack(texturePack)
{
	
}

void ChunkContainer::draw(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
	texturePack.bind();
	for (auto& [coordinate, chunk] : chunks)
		chunk->draw(renderer3D, shader);
}

void ChunkContainer::rebuildChunks()
{
	
	static auto rebuildMesh = [](std::shared_ptr<Chunk> chunk)
	{
		chunk->rebuildMesh();
		return chunk;
	};

	/*
	 * Prepare only one chunk to rebuild per call.
	 * This way very important modifications like user
	 * interaction with the world will be processed faster!
	 *
	 * Note that their meshes are only converted, not yet swapped.
	 */ 
	if (!chunksToRebuildQueue.empty())
	{

		chunkCreateMeshProcesses.emplace_back(
			std::async(std::launch::async, std::ref(rebuildMesh), chunksToRebuildQueue.front()));

		chunksToRebuildQueue.pop_front();
	}

	/*
	 * applyMesh uses the various calls to OpenGL directly.
	 * As we know OpenGL is a state machine and it's not very
	 * nice to execute it asynchronously, so the buffer swap
	 * of chunks is executed synchronously as it's fast anyway
	 */
	for (auto chunkIterator = chunkCreateMeshProcesses.begin(), 
			endChunkIterator = chunkCreateMeshProcesses.end(); 
			chunkIterator != endChunkIterator; )
	{
		if(chunkIterator->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			chunkIterator->get()->updateMesh();
			chunkCreateMeshProcesses.erase(chunkIterator++);
		}
		else
		{
			++chunkIterator;
		}
	}
}

void ChunkContainer::fixedUpdate(const float& deltaTime)
{
	rebuildChunks();
}

sf::Vector3i ChunkContainer::Coordinate::getNonChunkMetric() const
{
	return sf::Vector3i(
		x * Chunk::CHUNK_WALL_SIZE,
		y * Chunk::CHUNK_WALL_SIZE,
		z * Chunk::CHUNK_WALL_SIZE
		);
}

const Block* ChunkContainer::getWorldBlock(const Block::Coordinate& worldBlockCoordinates) const
{
	if(const auto chunk = blockPositionToChunk(worldBlockCoordinates))
	{
		return &chunk->getLocalBlock(chunk->globalToLocalCoordinates(worldBlockCoordinates));
	}
	return nullptr;
}

bool ChunkContainer::doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const
{
	std::lock_guard guard(chunksAccessMutex);

	const auto foundChunk = chunks.find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
	return foundChunk != chunks.cend();
}

std::shared_ptr<const Chunk> ChunkContainer::blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates) const
{
	std::lock_guard guard(chunksAccessMutex);

	const auto foundChunk = chunks.find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
	if(foundChunk != chunks.cend())
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
		const auto directions = chunk->getDirectionOfBlockFacesInContactWithOtherChunk(localCoordinates);
		for (auto& blockDirection : directions)
		{
			auto neighboringBlockInOtherChunk = chunk->localToGlobalCoordinates(
				chunk->getLocalBlockPosition(localCoordinates, blockDirection));

			if (const auto neighboringChunk = blockPositionToChunk(neighboringBlockInOtherChunk))
			{
				neighboringChunk->markToBeRebuildFast();
			}
		}
	}
}

void ChunkContainer::generateChunksAround(const Camera& camera)
{
	const auto cameraPosition = camera.getCameraPosition();
	const auto currentChunkOfCamera = ChunkContainer::Coordinate::blockToChunkMetric(
		Block::Coordinate::nonBlockToBlockMetric(sf::Vector3i(cameraPosition.x, cameraPosition.y, cameraPosition.z)));


	for (int x = -WORLD_GENERATION_CHUNK_NUMBER; x < WORLD_GENERATION_CHUNK_NUMBER; ++x)
	for (int y = -WORLD_GENERATION_CHUNK_NUMBER; y < WORLD_GENERATION_CHUNK_NUMBER; ++y)
	for (int z = -WORLD_GENERATION_CHUNK_NUMBER; z < WORLD_GENERATION_CHUNK_NUMBER; ++z)
	{
		addChunkIfNotExist({ currentChunkOfCamera.x + x, currentChunkOfCamera.y + y, currentChunkOfCamera.z + z});
	}
}

void ChunkContainer::clearFarAwayChunks(const Camera& camera)
{
	static auto minimalDistanceToDelete = static_cast<float>(
		WORLD_GENERATION_CHUNK_NUMBER * std::sqrt(WORLD_GENERATION_CHUNK_NUMBER) * Chunk::CHUNK_WALL_SIZE);

	std::vector<decltype(chunks)::key_type> coordinateOfChunksToDelete;
	for(auto& [coordinate, chunk] : chunks)
	{
		const auto chunkPosition = coordinate.getNonChunkMetric();
		const auto distanceFromChunk = glm::distance(
			camera.getCameraPosition(), glm::vec3(chunkPosition.x, chunkPosition.y, chunkPosition.z));

		if(distanceFromChunk > minimalDistanceToDelete)
		{
			coordinateOfChunksToDelete.emplace_back(coordinate);
		}
	}

	std::lock_guard guard(chunksAccessMutex);
	for(auto& coordinate : coordinateOfChunksToDelete)
	{
		chunks.erase(coordinate);
	}
}

void ChunkContainer::addChunk(ChunkContainer::Coordinate chunkPosition)
{
	chunks.emplace(chunkPosition, 
		std::make_shared<Chunk>(sf::Vector3i(chunkPosition.getNonChunkMetric()), texturePack, *this));
}

void ChunkContainer::addChunkIfNotExist(ChunkContainer::Coordinate chunkPosition)
{
	if (chunkPosition.y > 0) return;

	if(chunks.find(chunkPosition) == chunks.end())
	{
		addChunk(chunkPosition);
	}
}
