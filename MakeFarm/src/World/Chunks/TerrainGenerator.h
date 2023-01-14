#pragma once
#include "World/Biomes/Biome.h"
#include "World/Biomes/DesertBiome.h"
#include "World/Biomes/ForestBiome.h"
#include "World/Biomes/SnowyBiome.h"
#include "World/Biomes/VeryDryForestBiome.h"
#include "World/Block/Block.h"
#include "World/Chunks/Chunk.h"
#include <set>

class TerrainGenerator
{
public:
    explicit TerrainGenerator(int seed = 1337);

    /**
     * @brief Generates terrain for a given chunk with a given set of blocks
     * @param chunk Reference to the chunk on which the terrain is to be generated
     * @param chunkBlocks Collection of blocks of given chunk
     */
    void generateTerrain(ChunkInterface& chunk, ChunkInterface::ChunkBlocks& chunkBlocks);

    /**
     * @brief Returns a random seed that can be used to generate terrain
     * @return Random int value
     */
    static int randomSeed();

private:
    /**
     * @brief This includes biomes at individual local chunk coordinates, where (0,0) means  the
     * local position (-1, -1). This allows to know what biomes are one block away around the chunk.
     */
    using BiomesInsideChunkWithOneBlockAroundIt =
        MultiDimensionalArray<Biome*, ChunkInterface::BLOCKS_PER_X_DIMENSION + 2,
                              ChunkInterface::BLOCKS_PER_Z_DIMENSION + 2>;

    /**
     * @brief On the given global coordinates it determines what biome is located.
     * @param x Global coordinate at x on which the biome is checked
     * @param z Global coordinate at y on which the biome is checked
     * @return Biome, which is in the indicated position.
     */
    Biome& deduceBiome(int globalCoordinateX, int globalCoordinateZ);

    /**
     * @brief Returns what the humidity is at the indicated global coordinates.
     * @param globalCoordinate Global coordinate on which humidity is to be checked.
     * @return Humidity that is located at the indicated global coordinates.
     */
    float humidityAtGivenCoordinates(const Block::Coordinate& globalCoordinate);

    /**
     * @brief Returns what the temperature is at the indicated global coordinates.
     * @param globalCoordinate Global coordinate on which humidity is to be checked.
     * @return Humidity that is located at the indicated global coordinates.
     */
    float temperatureAtGivenCoordinates(const Block::Coordinate& globalCoordinate);

    /**
     * @brief Returns a collection of all biomes that are in the chunk and by 1 block away outside
     * the chunk.
     * @param biomesInChunkPerCoordinate A container with the dimensions of a chunk including the
     * border around the chunk of 1 block.
     * @return Collection of all biomes that are in the chunk and by 1 block away outside
     * the chunk.
     */
    std::set<Biome*> allBiomesInChunkAndOneBlockAroundIt(
        BiomesInsideChunkWithOneBlockAroundIt biomesInChunkPerCoordinate);

    /**
     * @brief Returns an array of all biomes that are in the chunk and by 1 block away outside
     * the chunk where each coordinate corresponds to a biome that is located on a given chunk.
     * @param chunk Chunk on which biomes are checked
     * @return Array of all biomes that are in the chunk and by 1 block away outside  the chunk
     * where each coordinate corresponds to a biome that is located on a given chunk.
     */
    BiomesInsideChunkWithOneBlockAroundIt biomePerLocalCoordinate(const ChunkInterface& chunk);

    /**
     * @brief Checks if the chunk contains only one biome.
     * @param biomesPerCoordinate Each coordinate corresponds to a biome that is located on a given
     * chunk, where (0,0) means  the local position (-1, -1). This allows to know what biomes are
     * one block away around the chunk.
     * @return True if the chunk contains only one biome, false otherwise.
     */
    bool doesChunkContainOnlyOneBiome(
        BiomesInsideChunkWithOneBlockAroundIt& biomesPerCoordinate) const;


    // TODO: It is possible that the following structures and related functions should be moved
    // outside this class.

    /**
     * @brief Structure defining the vertices of the rectangle
     */
    struct RectangleCorners
    {
        int topLeft;
        int topRight;
        int bottomLeft;
        int bottomRight;
    };

    /**
     * @brief Structure that defines a rectangle
     */
    struct Rectangle
    {
        int x;
        int z;
        int width;
        int height;
    };

    /**
     * @brief Converts bilinear interpolations based on noise at the corners of the rectangle to
     * create smoothed transitions between blocks by returning values at the individual coordinates
     * x and z
     * @param corners The corners of the rectangle defining the noise at the given positions.
     * @param rectangle A rectangle that is a representation of the chunk for which the noise per
     * coordinate is calculate.
     * @param x Local chunk x coordinate at the positions of which the smoothed new noise is
     * calculated.
     * @param z Local chunk z coordinate at the positions of which the smoothed new noise is
     * calculated.
     * @return A smoothed value that is a bilinear interpolation of the four corners of a rectangle
     * at a given x and z position.
     */
    int bilinearInterpolation(const RectangleCorners& corners, const Rectangle& rectangle, int x,
                              int z);

    /**
     * @brief Generates terrain on the indicated chunk using the indicated biome.
     * @param chunk Chunk on which the site is to be created.
     * @param chunkBlocks Chunk blocks that are overwritten thus creating terrain.
     * @param biome Biome to be used to create terrain.
     */
    void generateTerrainForChunkWithGivenBiome(const ChunkInterface& chunk,
                                               ChunkInterface::ChunkBlocks& chunkBlocks,
                                               Biome& biome) const;

    /**
     * @brief Calculate the noise on the corners of the chunk in the form of corners of a rectangle.
     * @param chunk Chunk for which corner noise is calculated.
     * @param allBiomesInChunk 2D array structure that determines the biom per coordinate taking
     * into account the outside of the chunk, that is, 1 block beyond the chunk inclusive.
     * @return Noise at the corners of the rectangle
     */
    RectangleCorners calculateNoiseAtChunkCorners(
        const ChunkInterface& chunk,
        const BiomesInsideChunkWithOneBlockAroundIt& allBiomesInChunk) const;

    /**
     * @brief Generates terrain on the indicated chunk using the indicated biome.
     * @param chunkBlocks Blocks of the chunk that are overwritten thus creating terrain.
     * @param allBiomesInChunkPerCoordinate 2D array structure that determines the biom per
     * coordinate taking into account the outside of the chunk, that is, 1 block beyond the chunk
     * inclusive.
     * @param cornerNoises Noise at the corners of the rectangle
     */
    void generateTerrainForChunkWithDifferentBiomes(
        ChunkInterface::ChunkBlocks& chunkBlocks,
        const BiomesInsideChunkWithOneBlockAroundIt& allBiomesInChunkPerCoordinate,
        const RectangleCorners& cornerNoises);


private:
    ForestBiome mForestBiome;
    VeryDryForestBiome mVeryDryForestBiome;
    DesertBiome mDesertBiome;
    SnowyBiome mSnowyBiome;

    FastNoiseLite mHumidity;
    FastNoiseLite mTemperature;
};