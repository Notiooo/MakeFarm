#include "MeshBuilder.h"
#include "pch.h"

MeshBuilder::MeshBuilder(Block::Coordinate origin)
    : mOrigin(std::move(origin))
{
}

MeshBuilder::MeshBuilder()
    : mOrigin(Block::Coordinate{0, 0, 0})
{
}
