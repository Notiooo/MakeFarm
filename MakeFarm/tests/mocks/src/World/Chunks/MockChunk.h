#pragma once
#include "World/Chunks/ChunkInterface.h"
#include <SFML/Window/Event.hpp>
#include <gmock/gmock.h>

class MockChunk : public ChunkInterface
{
public:
    MockChunk() = default;
    ~MockChunk() override = default;

    MOCK_METHOD(void, prepareMesh, (), (override));
    MOCK_METHOD(void, updateMesh, (), (override));
    MOCK_METHOD(void, fixedUpdate, (const float&), (override));
    MOCK_METHOD(void, drawTerrain, (const Renderer3D&, const sf::Shader&), (const, override));
    MOCK_METHOD(void, drawLiquids, (const Renderer3D&, const sf::Shader&), (const, override));
    MOCK_METHOD(void, drawFlorals, (const Renderer3D&, const sf::Shader&), (const, override));
    MOCK_METHOD(Block&, localBlock, (const Block::Coordinate&), (override));
    MOCK_METHOD(void, removeLocalBlock, (const Block::Coordinate&), (override));
    MOCK_METHOD(void, tryToPlaceBlock,
                (const BlockId&, const Block::Coordinate&, std::vector<BlockId>,
                 const RebuildOperation&),
                (override));
    MOCK_METHOD(const Block&, localBlock, (const Block::Coordinate&), (const, override));
    MOCK_METHOD(Block::Coordinate, globalToLocalCoordinates, (const Block::Coordinate&),
                (const, override));
    MOCK_METHOD(Block::Coordinate, localToGlobalCoordinates, (const Block::Coordinate&),
                (const, override));
    MOCK_METHOD(bool, isLocalCoordinateOnChunkEdge, (const Block::Coordinate&), (override));
    MOCK_METHOD(void, rebuildMesh, (), (override));
    MOCK_METHOD(void, rebuildSlow, (), (override));
    MOCK_METHOD(void, rebuildFast, (), (override));
    MOCK_METHOD(std::vector<Direction>, directionOfBlockFacesInContactWithOtherChunk,
                (const Block::Coordinate&), (override));
    MOCK_METHOD(Block::Coordinate, localNearbyBlockPosition,
                (const Block::Coordinate&, const Direction&), (const, override));
    MOCK_METHOD(Block&, localNearbyBlock, (const Block::Coordinate&, const Direction&), (override));
    MOCK_METHOD(const Block&, localNearbyBlock, (const Block::Coordinate&, const Direction&),
                (const, override));
    MOCK_METHOD(const Block::Coordinate&, positionInBlocks, (), (const, override));
    MOCK_METHOD(std::optional<Block>, neighbourBlockInGivenDirection,
                (const Block::Coordinate&, const Direction&), (override));
    MOCK_METHOD(Block::Coordinate, highestSetBlock, (const Block::Coordinate&), (override));
};