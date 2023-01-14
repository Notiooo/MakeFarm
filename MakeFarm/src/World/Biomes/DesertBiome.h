#pragma once
#include "World/Biomes/Biome.h"

/**
 * @brief The desert biome can generate desert-like terrain through appropriate terrain height as
 * well as additional environmental elements.
 */
class DesertBiome : public Biome
{
public:
    explicit DesertBiome(int seed);

    /**
     * @brief Determines the level of terrain at given x and z positions inside the chunk
     * @param blockCoordinateX Position x in the space for which the floor level is determined.
     * @param blockCoordinateZ Position z in the space for which the floor level is determined.
     * @return The level of terrain at a given position.
     */
    int surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ) override;

    /**
     * @brief Generates a whole row y of blocks in a given position with a certain level of
     * terrain.
     * @param chunkBlocks A collection of blocks in chunk.
     * @param blockCoordinateX The x position on which the column of blocks should appear.
     * @param blockCoordinateZ The z position on which the column of blocks should appear.
     * @param surfaceLevel Ground/Terrain level.
     */
    void generateColumnOfBlocks(ChunkInterface::ChunkBlocks& chunkBlocks, int surfaceLevel,
                                int blockCoordinateX, int blockCoordinateZ) override;

    /**
     * @brief It contains the steps that need to be done after generating the terrain. These include
     * such things as putting up vegetation and general beautification of the world.
     * @param chunk Chunk to be beautified
     */
    void postGenerationPlacements(ChunkInterface& chunk) override;

    /**
     * @brief Returns a biome identifier that specifies what biome it is.
     * @return Biome identifier
     */
    BiomeId biomeId() override;

private:
    /**
     * @brief It places cactuses on the given chunk.
     * @param chunk Chunk on which cactuses should appear.
     */
    void placeCactuses(ChunkInterface& chunk);

private:
    /**
     * @brief This is the factor responsible for how compacted the basic terrain is in the game
     * world. The lower it is, the less bumpy the terrain. The higher, the more bumpy the terrain.
     */
    static constexpr auto BASIC_TERRAIN_SQUASHING_FACTOR = 0.15f;
    FastNoiseLite mBasicTerrain;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> mCactusLengthGenerator;
    std::vector<Block::Coordinate> mCactusToPlace;

protected:// Ugly testing purpose :c
    std::bernoulli_distribution mCactusDecider;
};