#pragma once
#include "Chunk.h"
#include "World/Camera.h"

/*
 * This class is so far a resource-intensive bottleneck
 * that does not scale well. For the time being it is
 * used in the form of enabling further development,
 * but it must be optimized!
 *
 * I was thinking of an unordered_map that could collapse
 * chunk positions to slightly smaller numbers based on the
 * CHUNK_SIZE. Then access to those chunks would be pretty easy.
 *
 * TODO: Returning pointers to chunks and blocks should be
 * considered to change as in some cases a weak_ptr could be used.
 * Dangling pointer is a bad pointer! :<
 * In some cases, also need to pay attention to performance.
 * Therefore, this will be done at a later stage.
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
	void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates);

private:
	void addChunk(Block::Coordinate chunkPosition);
	[[nodiscard]] const Chunk* blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates) const;
	Chunk* blockPositionToChunk(const Block::Coordinate& worldBlockCoordinates);

private:
	std::vector<Chunk> chunks;
	const TexturePack& texturePack;
};