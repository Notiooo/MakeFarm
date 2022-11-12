#include "MeshBuilder.h"

#include "pch.h"
#include <utility>

#include "Physics/AABB.h"
#include "World/Chunks/Chunk.h"

MeshBuilder::MeshBuilder(Block::Coordinate origin)
    : mOrigin(std::move(origin))
{
}

MeshBuilder::MeshBuilder()
    : mOrigin(Block::Coordinate{0, 0, 0})
{
}

void MeshBuilder::setFaceSize(const float& faceSize)
{
    mBlockFaceSize = faceSize;
}

void MeshBuilder::addQuad(const Block::Face& blockFace, const std::vector<GLfloat>& textureQuad,
                          const Block::Coordinate& blockPosition)
{
    auto& vertices = mMesh.vertices;
    auto& texCoords = mMesh.textureCoordinates;
    auto& indices = mMesh.indices;

    texCoords.insert(texCoords.end(), textureQuad.begin(), textureQuad.end());

    auto face = faceVertices(blockFace);
    const auto& originPos = mOrigin.nonBlockMetric();
    const auto& blockPos = blockPosition.nonBlockMetric();

    /*
     * Some blocks are larger than others.
     * It would be good if they were not just longer in one plane,
     * but it was spread out among all of them -- increased relative to the center.
     */
    const auto blockSizeDifference = (mBlockFaceSize - Block::BLOCK_SIZE) / 2.f;

    for (int i = 0; i < 3 * 4; i += 3)
    {
        // a row in given face (x,y,z)
        vertices.emplace_back(face[i] * mBlockFaceSize + originPos.x + blockPos.x -
                              blockSizeDifference);
        vertices.emplace_back(face[i + 1] * mBlockFaceSize + originPos.y + blockPos.y -
                              blockSizeDifference);
        vertices.emplace_back(face[i + 2] * mBlockFaceSize + originPos.z + blockPos.z -
                              blockSizeDifference);
    }

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

void MeshBuilder::resetMesh()
{
    mMesh.indices.clear();
    mMesh.textureCoordinates.clear();
    mMesh.vertices.clear();
    mIndex = 0;
}

Mesh3D MeshBuilder::mesh3D() const
{
    // std::lock_guard _(mRebuildMeshMutex);
    return mMesh;
}


std::vector<GLfloat> MeshBuilder::faceVertices(const Block::Face& blockFace) const
{
    switch (blockFace)
    {
        case Block::Face::Top:
            return {
                // x  y  z
                0, 1, 1,// top close left
                1, 1, 1,// top close right
                1, 1, 0,// top far right
                0, 1, 0,// top far left
            };

        case Block::Face::Left:
            return {// x  y  z
                    0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};

        case Block::Face::Right:
            return {// x  y  z
                    1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1};

        case Block::Face::Bottom:
            return {// x  y  z
                    0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};

        case Block::Face::Front:
            return {
                0, 0, 1,// front left bottom
                1, 0, 1,// front right bottom
                1, 1, 1,// front right top
                0, 1, 1 // front left top
            };

        case Block::Face::Back:
            return {
                1, 0, 0,// back right bottom
                0, 0, 0,// back left bottom
                0, 1, 0,// back left top
                1, 1, 0,// back right top
            };
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

void MeshBuilder::addAABB(const AABB& aabb)
{
    for (int blockFace = 0; blockFace < static_cast<int>(Block::Face::Counter); ++blockFace)
    {
        auto& vertices = mMesh.vertices;
        auto& indices = mMesh.indices;

        auto face = faceVertices(static_cast<Block::Face>(blockFace));
        auto [collisionBoxMin, collisionBoxMax] = aabb.collisionBox();

        auto collisionBoxSize = aabb.collisionBoxSize();
        const auto& originPos = collisionBoxMin;

        for (int i = 0; i < 3 * 4; i += 3)
        {
            // a row in given face (x,y,z)
            vertices.emplace_back(face[i] * collisionBoxSize.x + originPos.x);
            vertices.emplace_back(face[i + 1] * collisionBoxSize.y + originPos.y);
            vertices.emplace_back(face[i + 2] * collisionBoxSize.z + originPos.z);
        }

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
}
