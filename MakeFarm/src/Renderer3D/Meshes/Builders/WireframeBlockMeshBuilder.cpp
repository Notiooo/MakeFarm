#include "WireframeBlockMeshBuilder.h"

WireframeBlockMeshBuilder::WireframeBlockMeshBuilder(Block::Coordinate origin)
    : BlockMeshBuilder(origin)
    , mMesh(std::make_unique<WireframeBlockMesh>())
{
}

WireframeBlockMeshBuilder::WireframeBlockMeshBuilder()
    : BlockMeshBuilder()
    , mMesh(std::make_unique<WireframeBlockMesh>())
{
}

void WireframeBlockMeshBuilder::resetMesh()
{
    mMesh = std::make_unique<WireframeBlockMesh>();
    mIndex = 0;
}

void WireframeBlockMeshBuilder::addWireframeBlock(const WireframeBlock& wireframeBlock)
{
    for (int blockFace = 0; blockFace < static_cast<int>(Block::Face::Counter); ++blockFace)
    {
        auto& vertices = mMesh->vertices;
        auto& indices = mMesh->indices;
        addBlockFaceVertices(wireframeBlock, blockFace, vertices);
        addBlockFaceIndices(indices);
    }
}

void WireframeBlockMeshBuilder::addBlockFaceVertices(const WireframeBlock& wireframeBlock,
                                                     int blockFace,
                                                     std::vector<float>& vertices) const
{
    auto face = faceVertices(static_cast<Block::Face>(blockFace));
    auto collisionBoxSize = wireframeBlock.max - wireframeBlock.min;
    const auto& originPos = wireframeBlock.min;

    for (int i = 0; i < 3 * 4; i += 3)
    {
        // a row in given face (x,y,z)
        vertices.emplace_back(face[i] * collisionBoxSize.x + originPos.x);
        vertices.emplace_back(face[i + 1] * collisionBoxSize.y + originPos.y);
        vertices.emplace_back(face[i + 2] * collisionBoxSize.z + originPos.z);
    }
}

void WireframeBlockMeshBuilder::addBlockFaceIndices(std::vector<GLuint>& indices)
{
    // clang-format off
    indices.insert(indices.end(),
               {
                mIndex,
                mIndex + 1,
                mIndex + 2,

                mIndex + 2,
                mIndex + 3,
                mIndex
               });
    // clang-format on
    mIndex += 4;
}

std::unique_ptr<Mesh3D> WireframeBlockMeshBuilder::mesh3D()
{
    return mMesh->clone();
}