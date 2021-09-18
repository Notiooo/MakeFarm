#include "pch.h"
#include "ChunkContainer.h"

#include "World/Block/BlockMap.h"

ChunkContainer::ChunkContainer(const TexturePack& texturePack)
	: texturePack(texturePack)
{
	//chunks.reserve(25);

	addChunk({ 0, 0, 0 });

	addChunk({ 0, 0, Chunk::BLOCKS_PER_DIMENSION });
	addChunk({ 0, Chunk::BLOCKS_PER_DIMENSION, 0 });
	addChunk({ Chunk::BLOCKS_PER_DIMENSION, 0, 0 });

	addChunk({ Chunk::BLOCKS_PER_DIMENSION, Chunk::BLOCKS_PER_DIMENSION, 0 });
	addChunk({ Chunk::BLOCKS_PER_DIMENSION, 0, Chunk::BLOCKS_PER_DIMENSION });
	//addChunk({ 0, Chunk::BLOCKS_IN_CHUNK, Chunk::BLOCKS_IN_CHUNK });

	addChunk({ Chunk::BLOCKS_PER_DIMENSION, Chunk::BLOCKS_PER_DIMENSION, Chunk::BLOCKS_PER_DIMENSION });

	//=========

	addChunk({ 0, 0, -Chunk::BLOCKS_PER_DIMENSION });
	addChunk({ 0, -Chunk::BLOCKS_PER_DIMENSION, 0 });
	addChunk({ -Chunk::BLOCKS_PER_DIMENSION, 0, 0 });

	addChunk({ -Chunk::BLOCKS_PER_DIMENSION, -Chunk::BLOCKS_PER_DIMENSION, 0 });
	addChunk({ -Chunk::BLOCKS_PER_DIMENSION, 0, -Chunk::BLOCKS_PER_DIMENSION });
	//addChunk({ 0, Chunk::BLOCKS_IN_CHUNK, Chunk::BLOCKS_IN_CHUNK });

	addChunk({ -Chunk::BLOCKS_PER_DIMENSION, -Chunk::BLOCKS_PER_DIMENSION, -Chunk::BLOCKS_PER_DIMENSION });

	for(auto& chunk : chunks)
	{
		chunk.createMesh();
	}
}

void ChunkContainer::draw(const Renderer3D& renderer3D, const sf::Shader& shader) const
{
	texturePack.bind();
	for (auto& chunk : chunks)
		chunk.draw(renderer3D, shader);
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
	for (auto& chunk : chunks)
	{
		if (Chunk::areLocalCoordinatesInsideChunk(chunk.globalToLocalCoordinates(worldBlockCoordinates)))
		{
			return &chunk;
		}
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

void ChunkContainer::addChunk(Block::Coordinate chunkPosition)
{
	chunks.emplace_back(std::move(chunkPosition), texturePack, *this);
}
