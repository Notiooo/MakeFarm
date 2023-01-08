#include "TerrainGenerator.h"
#include "World/Chunks/Chunk.h"
#include "World/Chunks/ChunkManager.h"
#include "pch.h"
#include <set>

TerrainGenerator::TerrainGenerator(int seed)
    : mForestBiome(seed)
    , mDesertBiome(seed)
    , mVeryDryForestBiome(seed)
    , mSnowyBiome(seed)
    , mHumidity()
    , mTemperature()
{
    mHumidity.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mHumidity.SetFrequency(0.001);
    mHumidity.SetFractalGain(0);
    mHumidity.SetFractalLacunarity(0.f);
    mHumidity.SetFractalOctaves(1);
    mHumidity.SetSeed(seed);

    mTemperature.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mTemperature.SetFrequency(0.001);
    mTemperature.SetFractalGain(0);
    mTemperature.SetFractalLacunarity(0.f);
    mTemperature.SetFractalOctaves(1);
    mTemperature.SetSeed(seed / 2);
}

void TerrainGenerator::generateTerrain(Chunk& chunk, Chunk::ChunkBlocks& chunkBlocks)
{
    auto allBiomesInChunkPerCoordinate = biomePerLocalCoordinate(chunk);
    auto biomesToPostProcess = allBiomesInChunkAndOneBlockAroundIt(allBiomesInChunkPerCoordinate);
    if (doesChunkContainOnlyOneBiome(allBiomesInChunkPerCoordinate))
    {
        auto& biome = **biomesToPostProcess.begin();
        generateTerrainForChunkWithGivenBiome(chunk, chunkBlocks, biome);
    }
    else
    {
        auto cornerNoises = calculateNoiseAtChunkCorners(chunk, allBiomesInChunkPerCoordinate);
        generateTerrainForChunkWithDifferentBiomes(chunkBlocks, allBiomesInChunkPerCoordinate,
                                                   cornerNoises);
    }
    for (auto biome: biomesToPostProcess)
    {
        biome->postGenerationPlacements(chunk);
    }
    biomesToPostProcess.clear();
}

void TerrainGenerator::generateTerrainForChunkWithDifferentBiomes(
    Chunk::ChunkBlocks& chunkBlocks,
    const BiomesInsideChunkWithOneBlockAroundIt& allBiomesInChunkPerCoordinate,
    const TerrainGenerator::RectangleCorners& cornerNoises)
{
    Rectangle smallerChunk;
    smallerChunk.x = 0;
    smallerChunk.width = Chunk::BLOCKS_PER_X_DIMENSION;

    smallerChunk.z = 0;
    smallerChunk.height = Chunk::BLOCKS_PER_Z_DIMENSION;

    for (auto x = smallerChunk.x; x < smallerChunk.x + smallerChunk.width; ++x)
    {
        for (auto z = smallerChunk.z; z < smallerChunk.z + smallerChunk.height; ++z)
        {
            auto surfaceLevel = bilinearInterpolation(cornerNoises, smallerChunk, x, z);
            auto& biome = *allBiomesInChunkPerCoordinate[x][z];
            biome.generateColumnOfBlocks(chunkBlocks, surfaceLevel, x, z);
        }
    }
}
TerrainGenerator::RectangleCorners TerrainGenerator::calculateNoiseAtChunkCorners(
    const Chunk& chunk, const BiomesInsideChunkWithOneBlockAroundIt& allBiomesInChunk) const
{
    auto calculateNoise = [this, &chunk, &allBiomesInChunk](Block::Coordinate localCoord)
    {
        auto& biome = *allBiomesInChunk[localCoord.x + 1][localCoord.z + 1];
        auto globalCoord = chunk.localToGlobalCoordinates({localCoord.x, 0, localCoord.z});
        int surfaceLevel = biome.surfaceLevelAtGivenPosition(globalCoord.x, globalCoord.z);
        return surfaceLevel;
    };

    auto averageTopLeftCorner = [&calculateNoise](const Block::Coordinate& localCoord)
    {
        auto totalNoise = (calculateNoise(localCoord) +//
                           calculateNoise({localCoord.x - 1, 0, localCoord.z}) +
                           calculateNoise({localCoord.x, 0, localCoord.z - 1}) +
                           calculateNoise({localCoord.x - 1, 0, localCoord.z - 1}));
        return static_cast<float>(totalNoise) / 4.f;
    };

    auto averageTopRightCorner = [&calculateNoise](const Block::Coordinate& localCoord)
    {
        auto totalNoise = (calculateNoise(localCoord) +//
                           calculateNoise({localCoord.x + 1, 0, localCoord.z}) +
                           calculateNoise({localCoord.x, 0, localCoord.z - 1}) +
                           calculateNoise({localCoord.x + 1, 0, localCoord.z - 1}));
        return static_cast<float>(totalNoise) / 4.f;
    };

    auto averageBottomLeftCorner = [&calculateNoise](const Block::Coordinate& localCoord)
    {
        auto totalNoise = (calculateNoise(localCoord) +//
                           calculateNoise({localCoord.x - 1, 0, localCoord.z}) +
                           calculateNoise({localCoord.x, 0, localCoord.z + 1}) +
                           calculateNoise({localCoord.x - 1, 0, localCoord.z + 1}));
        return static_cast<float>(totalNoise) / 4.f;
    };

    auto averageBottomRightCorner = [&calculateNoise](const Block::Coordinate& localCoord)
    {
        auto totalNoise = (calculateNoise(localCoord) +//
                           calculateNoise({localCoord.x + 1, 0, localCoord.z}) +
                           calculateNoise({localCoord.x, 0, localCoord.z + 1}) +
                           calculateNoise({localCoord.x + 1, 0, localCoord.z + 1}));
        return static_cast<float>(totalNoise) / 4.f;
    };

    RectangleCorners cornerNoises;
    cornerNoises.topLeft = averageTopLeftCorner({0, 0, 0});
    cornerNoises.topRight = averageTopRightCorner({Chunk::BLOCKS_PER_X_DIMENSION - 1, 0, 0});
    cornerNoises.bottomLeft = averageBottomLeftCorner({0, 0, Chunk::BLOCKS_PER_Z_DIMENSION - 1});
    cornerNoises.bottomRight = averageBottomRightCorner(
        {Chunk::BLOCKS_PER_X_DIMENSION - 1, 0, Chunk::BLOCKS_PER_Z_DIMENSION - 1});
    return cornerNoises;
}

void TerrainGenerator::generateTerrainForChunkWithGivenBiome(const Chunk& chunk,
                                                             Chunk::ChunkBlocks& chunkBlocks,
                                                             Biome& biome) const
{
    for (auto x = 0; x < Chunk::BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto z = 0; z < Chunk::BLOCKS_PER_Z_DIMENSION; ++z)
        {
            auto globalCoord = chunk.localToGlobalCoordinates({x, 0, z});
            auto surfaceLevel = biome.surfaceLevelAtGivenPosition(globalCoord.x, globalCoord.z);
            biome.generateColumnOfBlocks(chunkBlocks, surfaceLevel, x, z);
        }
    }
}

bool TerrainGenerator::doesChunkContainOnlyOneBiome(
    BiomesInsideChunkWithOneBlockAroundIt& biomesPerCoordinate) const
{
    auto& sampleBiome = biomesPerCoordinate[0][0];
    for (auto& columnOfBiomes: biomesPerCoordinate)
    {
        for (auto& biome: columnOfBiomes)
        {
            if (biome->biomeId() != sampleBiome->biomeId())
            {
                return false;
            }
        }
    }
    return true;
}

TerrainGenerator::BiomesInsideChunkWithOneBlockAroundIt TerrainGenerator::biomePerLocalCoordinate(
    const Chunk& chunk)
{
    BiomesInsideChunkWithOneBlockAroundIt biomeInCoordinate;

    for (auto x = 0; x < Chunk::BLOCKS_PER_X_DIMENSION + 2; ++x)
    {
        for (auto z = 0; z < Chunk::BLOCKS_PER_Z_DIMENSION + 2; ++z)
        {
            auto globalCoord = chunk.localToGlobalCoordinates({x - 1, 0, z - 1});
            biomeInCoordinate[x][z] = &(deduceBiome(globalCoord.x, globalCoord.z));
        }
    }
    return biomeInCoordinate;
}


int TerrainGenerator::randomSeed()
{
    static std::random_device r;
    static std::default_random_engine e1(r());

    std::uniform_int_distribution<int> uniformDist(INT_MIN, INT_MAX);
    return uniformDist(e1);
}

Biome& TerrainGenerator::deduceBiome(int globalCoordinateX, int globalCoordinateZ)
{
    auto& x = globalCoordinateX;
    auto& z = globalCoordinateZ;

    auto humidity = humidityAtGivenCoordinates({x, 0, z});
    auto temperature = temperatureAtGivenCoordinates({x, 0, z});

    if (humidity <= 20 && temperature >= 20)
    {
        return mDesertBiome;
    }
    else if (humidity <= 30 && temperature >= 15)
    {
        return mVeryDryForestBiome;
    }
    else if (temperature < -5)
    {
        return mSnowyBiome;
    }
    else
    {
        return mForestBiome;
    }
}

float TerrainGenerator::temperatureAtGivenCoordinates(const Block::Coordinate& globalCoordinate)
{
    auto temperature = mTemperature.GetNoise(static_cast<float>(globalCoordinate.x),
                                             static_cast<float>(globalCoordinate.z));
    temperature *= 50;

    return temperature;// range -50 to 50
}

float TerrainGenerator::humidityAtGivenCoordinates(const Block::Coordinate& globalCoordinate)
{
    auto humidity = mHumidity.GetNoise(static_cast<float>(globalCoordinate.x),
                                       static_cast<float>(globalCoordinate.z));
    humidity += 1;
    humidity /= 2;
    humidity *= 90;

    return humidity;// 0 - 90
}

int TerrainGenerator::bilinearInterpolation(const TerrainGenerator::RectangleCorners& corners,
                                            const TerrainGenerator::Rectangle& rectangle, int x,
                                            int z)
{
    auto distanceToRectangleRightSide = rectangle.x + rectangle.width - x;
    auto distanceToRectangleLeftSide = x - rectangle.x;

    auto distanceToRectangleBottomSide = rectangle.z + rectangle.height - z;
    auto distanceToRectangleTopSide = z - rectangle.z;

    return static_cast<int>(
        1.0f / (rectangle.width * rectangle.height) *
        (corners.bottomLeft * distanceToRectangleRightSide * distanceToRectangleTopSide +
         corners.bottomRight * distanceToRectangleLeftSide * distanceToRectangleTopSide +
         corners.topLeft * distanceToRectangleRightSide * distanceToRectangleBottomSide +
         corners.topRight * distanceToRectangleLeftSide * distanceToRectangleBottomSide));
}

std::set<Biome*> TerrainGenerator::allBiomesInChunkAndOneBlockAroundIt(
    BiomesInsideChunkWithOneBlockAroundIt biomesInChunkPerCoordinate)
{
    std::set<Biome*> allBiomes;
    for (auto x = 0; x < Chunk::BLOCKS_PER_X_DIMENSION + 2; ++x)
    {
        for (auto z = 0; z < Chunk::BLOCKS_PER_Z_DIMENSION + 2; ++z)
        {
            allBiomes.emplace(biomesInChunkPerCoordinate[x][z]);
        }
    }
    return allBiomes;
}
