#pragma once
#include "World/Biomes/BiomeId.h"
#include "World/Chunks/Chunk.h"
#include <FastNoiseLite.h>

/**
 * @brief
 */
class Biome
{
public:
    Biome(int seed);
    virtual ~Biome() = default;

    static constexpr auto SEA_LEVEL = 60;
    static constexpr auto MINIMAL_TERRAIN_LEVEL = 20;

    /**
     * \brief This is the maximum site height that can be generated.
     *
     * For example:
     * Chunk::BLOCKS_PER_Y_DIMENSION - 10 means that the highest height
     * that the highest mountain will reach is maximum chunk height - 10
     */
    static constexpr int MAX_HEIGHT_MAP = Chunk::BLOCKS_PER_Y_DIMENSION - 10;

    /**
     * @brief Determines the level of terrain/grass at given x and z positions inside the chunk
     * @param blockCoordinateX Position x in the space for which the grass/floor level is
     * determined.
     * @param blockCoordinateZ Position z in the space for which the grass/floor level is
     * determined.
     * @return The level of terrain/grass at a given position.
     */
    virtual int surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ) = 0;

    /**
     * @brief Generates a whole row y of blocks in a given position with a certain level of
     * terrain/grass.
     * @param chunkBlocks A collection of blocks in chunk.
     * @param blockCoordinateX The x position on which the column of blocks should appear.
     * @param blockCoordinateZ The z position on which the column of blocks should appear.
     * @param surfaceLevel Ground level/grass level.
     */
    virtual void generateColumnOfBlocks(Chunk::ChunkBlocks& chunkBlocks, int surfaceLevel,
                                        int blockCoordinateX, int blockCoordinateZ) = 0;

    /**
     * @brief It contains the steps that need to be done after generating the terrain. These include
     * such things as putting up vegetation and general beautification of the world.
     * @param chunk Chunk to be beautified
     */
    virtual void postGenerationPlacements(Chunk& chunk) = 0;

    /**
     * @brief The seed with which the biome is created
     * @return Returns a seed with which the biome is created
     */
    [[nodiscard]] int seed() const;

    /**
     * @brief Returns a biome identifier that specifies what biome it is.
     * @return Biome identifier
     */
    virtual BiomeId biomeId() = 0;

private:
    const int mSeed;
};
