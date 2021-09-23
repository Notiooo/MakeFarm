#include "pch.h"
#include "ChunkContainer.h"

#include "World/Block/BlockMap.h"

ChunkContainer::ChunkContainer(const TexturePack& texturePack)
	: texturePack(texturePack)
{
	//chunks.reserve(25);

	addChunk({ 0, 0, 0 });

	addChunk({ 0, 0, 1 });
	addChunk({ 0, 1, 0 });
	addChunk({ 1, 0, 0 });

	addChunk({ 1, 1, 0 });
	addChunk({ 1, 0, 1 });

	addChunk({ 1, 1, 1 });
	addChunk({ 1, 1, 2 });
	

	//=========

	addChunk({ 0, 0, -1 });
	addChunk({ 0, -1, 0 });
	addChunk({ -1, 0, 0 });

	addChunk({ -1, -1, 0 });
	addChunk({ -1, 0, -1 });

	addChunk({ -1, -1, -1 });

	for(auto& [coordinate, chunk] : chunks)
	{
		chunk.createMesh();
	}
}

void ChunkContainer::draw(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
	texturePack.bind();
	for (auto& [coordinate, chunk] : chunks)
		chunk.draw(renderer3D, shader);
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

const Chunk* ChunkContainer::blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates) const
{
	const auto foundChunk = chunks.find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
	if(foundChunk != chunks.cend())
	{
		return &foundChunk->second;
	}
	return nullptr;
}

Chunk* ChunkContainer::blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates)
{
	return const_cast<Chunk*>(static_cast<const ChunkContainer&>(*this).blockPositionToChunk(worldBlockCoordinates));
}


void ChunkContainer::removeWorldBlock(const Block::Coordinate& worldBlockCoordinates)
{
	if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
	{
		auto localCoordinates = chunk->globalToLocalCoordinates(worldBlockCoordinates);
		chunk->removeLocalBlock(localCoordinates);
		auto chunksDirections = chunk->getCollidingChunkDirections(localCoordinates);
		for (auto& chunkDirection : chunksDirections)
		{
			auto collidingChunkBlockPosition = chunk->localToGlobalCoordinates(
				chunk->getLocalBlockPosition(localCoordinates, chunkDirection));

			if (const auto chunkPtr = blockPositionToChunk(collidingChunkBlockPosition))
			{
				chunkPtr->rebuildChunk();
			}
		}
	}
}

void ChunkContainer::addChunk(ChunkContainer::Coordinate chunkPosition)
{
	chunks.emplace(chunkPosition, 
		Chunk(sf::Vector3i(chunkPosition.getNonChunkMetric()), texturePack, *this));
}
