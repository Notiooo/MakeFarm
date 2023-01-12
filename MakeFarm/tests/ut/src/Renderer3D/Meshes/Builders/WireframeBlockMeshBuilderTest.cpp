#include "Renderer3D/Meshes/Builders/WireframeBlockMeshBuilder.h"
#include "gtest/gtest.h"

namespace
{

class UglyTestableWireframeBlockMeshBuilder : public WireframeBlockMeshBuilder
{
public:
    using WireframeBlockMeshBuilder::WireframeBlockMeshBuilder;

    std::unique_ptr<WireframeBlockMesh>& testableMesh()
    {
        return mMesh;
    }
};

class WireframeBlockMeshBuilderTest : public ::testing::Test
{
protected:
    Block::Coordinate blockCoordinate{0, 0, 0};
    float left = 0;
    float right = 0.0625;
    float bottom = 0;
    float top = 0.0625;
    std::vector<GLfloat> sampleTexture{right, top, left, top, left, bottom, right, bottom};
};

TEST_F(WireframeBlockMeshBuilderTest, ResetOnEmptyBuilderShouldNotThrowError)
{
    BlockMeshBuilder blockMeshBuilder;
    EXPECT_NO_THROW(blockMeshBuilder.resetMesh());
}

TEST_F(WireframeBlockMeshBuilderTest, EmptyBuilderShouldReturnNoIndices)
{
    BlockMeshBuilder blockMeshBuilder;
    auto indices = blockMeshBuilder.mesh3D()->indices;
    auto expectedIndices = std::vector<GLuint>{};
    EXPECT_EQ(indices, expectedIndices);
}

TEST_F(WireframeBlockMeshBuilderTest, EmptyBuilderShouldReturnNoVertices)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    auto expectedVertices = std::vector<float>{};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}


TEST_F(WireframeBlockMeshBuilderTest, EmptyWireframeReturnsCorrectIndices)
{
    WireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{0, 0, 0});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto indices = blockMeshBuilder.mesh3D()->indices;
    auto expectedIndices =
        std::vector<GLuint>{0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    EXPECT_EQ(indices, expectedIndices);
}

TEST_F(WireframeBlockMeshBuilderTest, EmptyWireframeReturnsCorrectVertices)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{0, 0, 0});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto expectedVertices = std::vector<float>(3 * 4 * 6, 0);
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeAtOriginReturnsCorrectIndices)
{
    WireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{12, 6, 7});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto indices = blockMeshBuilder.mesh3D()->indices;
    auto expectedIndices =
        std::vector<GLuint>{0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    EXPECT_EQ(indices, expectedIndices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeAtOriginReturnsCorrectIndicesAfterResetAndFilling)
{
    WireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{12, 6, 7});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    blockMeshBuilder.resetMesh();
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto indices = blockMeshBuilder.mesh3D()->indices;
    auto expectedIndices =
        std::vector<GLuint>{0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    EXPECT_EQ(indices, expectedIndices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeAtOriginReturnsNoIndicesAfterReset)
{
    WireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{12, 6, 7});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    blockMeshBuilder.resetMesh();
    EXPECT_TRUE(blockMeshBuilder.mesh3D()->indices.empty());
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeAtOriginReturnsCorrectVerticesAfterResetAndFilling)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{12, 6, 7});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    blockMeshBuilder.resetMesh();
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto expectedVertices = std::vector<float>{
        0,  6, 7, 12, 6, 7, 12, 6, 0, 0,  6, 0, 0,  0, 0, 0,  0, 7, 0,  6, 7, 0,  6, 0,
        12, 0, 7, 12, 0, 0, 12, 6, 0, 12, 6, 7, 0,  0, 0, 12, 0, 0, 12, 0, 7, 0,  0, 7,
        0,  0, 7, 12, 0, 7, 12, 6, 7, 0,  6, 7, 12, 0, 0, 0,  0, 0, 0,  6, 0, 12, 6, 0};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeAtOriginReturnsCorrectVertices)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{12, 6, 7});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto expectedVertices = std::vector<float>{
        0,  6, 7, 12, 6, 7, 12, 6, 0, 0,  6, 0, 0,  0, 0, 0,  0, 7, 0,  6, 7, 0,  6, 0,
        12, 0, 7, 12, 0, 0, 12, 6, 0, 12, 6, 7, 0,  0, 0, 12, 0, 0, 12, 0, 7, 0,  0, 7,
        0,  0, 7, 12, 0, 7, 12, 6, 7, 0,  6, 7, 12, 0, 0, 0,  0, 0, 0,  6, 0, 12, 6, 0};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeAtOriginReturnsNoVerticesAfterReset)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{0, 0, 0}, sf::Vector3f{12, 6, 7});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    blockMeshBuilder.resetMesh();
    EXPECT_TRUE(blockMeshBuilder.testableMesh()->vertices.empty());
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeDeepInSpaceReturnsCorrectIndices)
{
    WireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{120, 121, 123}, sf::Vector3f{140, 135, 142});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto indices = blockMeshBuilder.mesh3D()->indices;
    auto expectedIndices =
        std::vector<GLuint>{0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    EXPECT_EQ(indices, expectedIndices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeDeepInSpaceReturnsCorrectVertices)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{120, 121, 123}, sf::Vector3f{140, 135, 142});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto expectedVertices = std::vector<float>{
        120, 135, 142, 140, 135, 142, 140, 135, 123, 120, 135, 123, 120, 121, 123, 120, 121, 142,
        120, 135, 142, 120, 135, 123, 140, 121, 142, 140, 121, 123, 140, 135, 123, 140, 135, 142,
        120, 121, 123, 140, 121, 123, 140, 121, 142, 120, 121, 142, 120, 121, 142, 140, 121, 142,
        140, 135, 142, 120, 135, 142, 140, 121, 123, 120, 121, 123, 120, 135, 123, 140, 135, 123};

    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeDeepInSpaceAtNegativesReturnsCorrectIndices)
{
    WireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{-120, -121, -123}, sf::Vector3f{-140, -135, -142});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto indices = blockMeshBuilder.mesh3D()->indices;
    auto expectedIndices =
        std::vector<GLuint>{0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    EXPECT_EQ(indices, expectedIndices);
}

TEST_F(WireframeBlockMeshBuilderTest, WireframeDeepInSpaceAtNegativesReturnsCorrectVertices)
{
    UglyTestableWireframeBlockMeshBuilder blockMeshBuilder;
    WireframeBlock wireframeBlock(sf::Vector3f{-120, -121, -123}, sf::Vector3f{-140, -135, -142});
    blockMeshBuilder.addWireframeBlock(wireframeBlock);
    auto expectedVertices = std::vector<float>{
        -120, -135, -142, -140, -135, -142, -140, -135, -123, -120, -135, -123, -120, -121, -123,
        -120, -121, -142, -120, -135, -142, -120, -135, -123, -140, -121, -142, -140, -121, -123,
        -140, -135, -123, -140, -135, -142, -120, -121, -123, -140, -121, -123, -140, -121, -142,
        -120, -121, -142, -120, -121, -142, -140, -121, -142, -140, -135, -142, -120, -135, -142,
        -140, -121, -123, -120, -121, -123, -120, -135, -123, -140, -135, -123};

    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

}// namespace