#include "Renderer3D/Meshes/Builders/BlockMeshBuilder.h"
#include "gtest/gtest.h"

namespace
{

class UglyTestableBlockMeshBuilder : public BlockMeshBuilder
{
public:
    using BlockMeshBuilder::BlockMeshBuilder;

    std::unique_ptr<WorldBlockMesh>& testableMesh()
    {
        return mMesh;
    }
};

class BlockMeshBuilderTest : public ::testing::Test
{
protected:
    Block::Coordinate blockCoordinate{0, 0, 0};
    float left = 0;
    float right = 0.0625;
    float bottom = 0;
    float top = 0.0625;
    std::vector<GLfloat> sampleTexture{right, top, left, top, left, bottom, right, bottom};
};

TEST_F(BlockMeshBuilderTest, ResetOnEmptyBuilderShouldNotThrowError)
{
    BlockMeshBuilder blockMeshBuilder;
    EXPECT_NO_THROW(blockMeshBuilder.resetMesh());
}

TEST_F(BlockMeshBuilderTest, EmptyBuilderShouldReturnNoIndices)
{
    BlockMeshBuilder blockMeshBuilder;
    EXPECT_TRUE(blockMeshBuilder.mesh3D()->indices.empty());
}

TEST_F(BlockMeshBuilderTest, EmptyBuilderShouldReturnNoVertices)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    EXPECT_TRUE(blockMeshBuilder.testableMesh()->vertices.empty());
}

TEST_F(BlockMeshBuilderTest, EmptyBuilderShouldReturnNoTextureCoordinates)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    EXPECT_TRUE(blockMeshBuilder.testableMesh()->textureCoordinates.empty());
}

TEST_F(BlockMeshBuilderTest, EmptyBuilderShouldReturnNoDirectionalLightning)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    EXPECT_TRUE(blockMeshBuilder.testableMesh()->directionalLightning.empty());
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsCorrectIndices)
{
    BlockMeshBuilder blockMeshBuilder;

    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    auto expectedIndices = std::vector<GLuint>{0, 1, 2, 2, 3, 0};
    EXPECT_EQ(blockMeshBuilder.mesh3D()->indices, expectedIndices);
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsCorrectVertices)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;

    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    auto expectedVertices = std::vector<float>{0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(BlockMeshBuilderTest,
       ReturnsCorrectIndicesAfterAddingSingleFaceResetAndAddingSingleFaceAgain)
{
    BlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);

    auto expectedIndices = std::vector<GLuint>{0, 1, 2, 2, 3, 0};
    EXPECT_EQ(blockMeshBuilder.mesh3D()->indices, expectedIndices);
}

TEST_F(BlockMeshBuilderTest, ReturnsEmptyIndeciesAfterAddingSingleFaceAndReset)
{
    BlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();

    EXPECT_TRUE(blockMeshBuilder.mesh3D()->indices.empty());
}

TEST_F(BlockMeshBuilderTest,
       ReturnsCorrectVerticesAfterAddingSingleFaceResetAndAddingSingleFaceAgain)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);

    auto expectedVertices = std::vector<float>{0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(BlockMeshBuilderTest, ReturnsEmptyVerticesAfterAddingSingleFaceReset)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();
    EXPECT_TRUE(blockMeshBuilder.testableMesh()->vertices.empty());
}

TEST_F(BlockMeshBuilderTest, AllFacesReturnsCorrectIndices)
{
    BlockMeshBuilder blockMeshBuilder;

    for (int i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        blockMeshBuilder.addQuad(static_cast<Block::Face>(i), sampleTexture, blockCoordinate);
    }

    auto expectedIndices =
        std::vector<GLuint>{0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    EXPECT_EQ(blockMeshBuilder.mesh3D()->indices, expectedIndices);
}

TEST_F(BlockMeshBuilderTest, AllFacesReturnsCorrectVertices)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;

    for (int i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        blockMeshBuilder.addQuad(static_cast<Block::Face>(i), sampleTexture, blockCoordinate);
    }

    auto expectedVertices =
        std::vector<float>{0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0,
                           1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
                           0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->vertices, expectedVertices);
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsCorrectLightning)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);

    auto expectedLightning = std::vector<float>{0.65, 0.65, 0.65, 0.65};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->directionalLightning, expectedLightning);
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsCorrectLightningAfterAddingAndResetingAndAddingAgain)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);

    auto expectedLightning = std::vector<float>{0.65, 0.65, 0.65, 0.65};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->directionalLightning, expectedLightning);
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsEmptyLightningAfterReset)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();
    EXPECT_TRUE(blockMeshBuilder.testableMesh()->directionalLightning.empty());
}

TEST_F(BlockMeshBuilderTest, AllFacesReturnsCorrectLightning)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;

    for (int i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        blockMeshBuilder.addQuad(static_cast<Block::Face>(i), sampleTexture, blockCoordinate);
    }

    std::vector<float> expectedLightning{1,    1,    1,    1,    0.65, 0.65, 0.65, 0.65,
                                         0.65, 0.65, 0.65, 0.65, 0.5,  0.5,  0.5,  0.5,
                                         0.8,  0.8,  0.8,  0.8,  0.5,  0.5,  0.5,  0.5};
    EXPECT_EQ(blockMeshBuilder.testableMesh()->directionalLightning, expectedLightning);
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsCorrectTextureCoordinates)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);

    EXPECT_EQ(blockMeshBuilder.testableMesh()->textureCoordinates, sampleTexture);
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsEmptyTextureCoordinatesAfterReset)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();

    EXPECT_TRUE(blockMeshBuilder.testableMesh()->textureCoordinates.empty());
}

TEST_F(BlockMeshBuilderTest, SingleFaceReturnsCorrectTextureCoordinatesAfterResetAddingAndReset)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);
    blockMeshBuilder.resetMesh();
    blockMeshBuilder.addQuad(Block::Face::Left, sampleTexture, blockCoordinate);

    EXPECT_EQ(blockMeshBuilder.testableMesh()->textureCoordinates, sampleTexture);
}

TEST_F(BlockMeshBuilderTest, AllFacesReturnsCorrectTextureCoordinates)
{
    UglyTestableBlockMeshBuilder blockMeshBuilder;

    for (int i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        blockMeshBuilder.addQuad(static_cast<Block::Face>(i), sampleTexture, blockCoordinate);
    }

    std::vector<float> expectedTextureCoordinates;
    for (int i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        expectedTextureCoordinates.insert(expectedTextureCoordinates.end(), sampleTexture.begin(),
                                          sampleTexture.end());
    }
    EXPECT_EQ(blockMeshBuilder.testableMesh()->textureCoordinates, expectedTextureCoordinates);
}

}// namespace