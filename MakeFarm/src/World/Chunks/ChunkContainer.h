#pragma once
#include "Chunk.h"

/*
 * This class is so far a resource-intensive bottleneck
 * that does not scale well. For the time being it is
 * used in the form of enabling further development,
 * but it must be optimized!
 *
 * I was thinking of an unordered_map that could collapse
 * chunk positions to slightly smaller numbers based on the
 * CHUNK_SIZE. Then access to those chunks would be pretty easy.
 */

class ChunkContainer
{
public:
	ChunkContainer(const TexturePack& texturePack);
	void draw(const Renderer3D& renderer3D, const sf::Shader& shader) const;

	/**
	 * \brief Finds a block inside a container based on the global position of the block
	 * \param worldBlockCoordinates Global position of the block inside the game world
	 * \return Pointer to block found, or nullptr if not found
	 */
	[[nodiscard]] const Block* getWorldBlock(const Block::Coordinate& worldBlockCoordinates) const;

private:
	void addChunk(Block::Coordinate chunkPosition);

private:
	std::vector<Chunk> chunks;
	const TexturePack& texturePack;
};
