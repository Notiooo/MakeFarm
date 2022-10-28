#include "MeshBuilder.h"
#include "pch.h"

#include "World/Chunks/Chunk.h"

MeshBuilder::MeshBuilder(const Block::Coordinate& origin)
    : mOrigin(origin)
{
}

void MeshBuilder::setFaceSize(const float& faceSize)
{
    mBlockFaceSize = faceSize;
}

void MeshBuilder::addQuad(const Block::Face& blockFace, const std::vector<GLfloat>& textureQuad,
                          const Block::Coordinate& blockPosition)
{
    auto& vertices = mChunkMesh.vertices;
    auto& texCoords = mChunkMesh.textureCoordinates;
    auto& indices = mChunkMesh.indices;

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
    mChunkMesh.indices.clear();
    mChunkMesh.textureCoordinates.clear();
    mChunkMesh.vertices.clear();
    mIndex = 0;
}

Mesh3D MeshBuilder::mesh3D() const
{
    // std::lock_guard _(mRebuildMeshMutex);
    return mChunkMesh;
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

        case Block::Face::Left: return {0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};

        case Block::Face::Right: return {1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1};

        case Block::Face::Bottom: return {0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};

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

void MeshBuilder::blockMesh() const
{
    mRebuildMeshMutex.lock();
}

void MeshBuilder::unblockMesh() const
{
    mRebuildMeshMutex.unlock();
}
