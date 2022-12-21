#pragma once

#include "World/Biomes/Biome.h"

/**
 * @brief The forest biome can generate forest-like terrain through appropriate terrain height as
 * well as additional environmental elements.
 */
class ForestBiome : public Biome
{
public:
    explicit ForestBiome(int seed);

    /**
     * @brief Determines the level of terrain at given x and z positions inside the chunk
     * @param blockCoordinateX Position x in the space for which the grass/floor level is
     * determined.
     * @param blockCoordinateZ Position z in the space for which the grass/floor level is
     * determined.
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
    void generateColumnOfBlocks(Chunk::ChunkBlocks& chunkBlocks, int surfaceLevel,
                                int blockCoordinateX, int blockCoordinateZ) override;

    /**
     * @brief It contains the steps that need to be done after generating the terrain. These include
     * such things as putting up vegetation and general beautification of the world.
     * @param chunk Chunk to be beautified
     */
    void postGenerationPlacements(Chunk& chunk) override;

    /**
     * @brief Returns a biome identifier that specifies what biome it is.
     * @return Biome identifier
     */
    BiomeId biomeId() override;

private:
    /**
     * @brief Places trees on the indicated chunk
     * @param chunk Chunk on which trees should be placed
     */
    void placeTrees(Chunk& chunk);

    /**
     * @brief Places tree on the indicated chunk in indicated position
     * @param chunk Chunk on which tree should be placed
     * @param block Block on which a new tree should be built
     */
    void placeTree(Chunk& chunk, const Block::Coordinate& block);

    /**
     * @brief Places the main root of the tree, i.e. the column of logs.
     * @param chunk Chunk on which the main tree root is to be placed
     * @param block Block on which the main tree root is to be placed
     * @param treeLength The length the tree should be
     */
    void placeLogOfTheTree(Chunk& chunk, const Block::Coordinate& block, int treeLength) const;

    /**
     * @brief Stands the upper crown of the tree created from leaves. It is a crown consisting of
     * four blocks in one y-plane.
     * @param chunk Chunk on which the upper crown is to be placed
     * @param block Block around which leaves are placed
     * @param treeLength Length of tree
     */
    void placeTopPartOfTreeTopMadeOfLeaves(Chunk& chunk, const Block::Coordinate& block,
                                           int treeLength) const;
    /**
     * @brief Stands the bottom part of crown of the tree created from leaves. It is a crown
     * consisting of 24 blocks in one y-plane.
     * @param chunk Chunk on which the bottom crown is to be placed
     * @param block Block around which leaves are placed
     * @param treeLength Length of tree
     */
    void placeBottomPartOfTreeTopMadeOfLeaves(Chunk& chunk, const Block::Coordinate& block,
                                              int treeLength) const;

private:
    /**
     * @brief This is the factor responsible for how compacted the basic terrain is in the game
     * world. The lower it is, the less bumpy the terrain. The higher, the more bumpy the terrain.
     */
    static constexpr auto BASIC_TERRAIN_SQUASHING_FACTOR = 0.25f;

    FastNoiseLite mBasicTerrain;
    FastNoiseLite mHillsAndValleys;

    std::random_device rd;
    std::mt19937 gen;
    std::bernoulli_distribution mTreeDecider;
    std::uniform_int_distribution<int> mTreeLengthGenerator;
    std::vector<Block::Coordinate> mTreesToPlace;
};