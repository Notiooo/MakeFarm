#include "HighlightedBlock.h"
#include "pch.h"

#include "Renderer3D/Model3D.h"
#include "Resources/TexturePack.h"
#include "World/Camera.h"
#include "World/Chunks/ChunkManager.h"
#include "World/Chunks/MeshBuilder.h"

HighlightedBlock::HighlightedBlock()
{
    mWireframeShader.loadFromFile("resources/shaders/WireframeRenderer/VertexShader.shader",
                                  "resources/shaders/WireframeRenderer/GeometryShader.shader",
                                  "resources/shaders/WireframeRenderer/FragmentShader.shader");

    sf::Shader::bind(&mWireframeShader);
    mWireframeShader.setUniform("u_Color", sf::Glsl::Vec3(0.5f, 0.5f, 0.5f));
    sf::Shader::bind(nullptr);
}

void HighlightedBlock::markFacedBlock(const Camera& camera, const ChunkManager& chunkManager)
{
    static auto rayStep = 0.01f;
    for (float raySize = rayStep; raySize < MAX_RAY_SIZE; raySize += rayStep)
    {
        auto positionAtTheEndOfRay = camera.positionAtTheEndOfRayInfrontOfCamera<float>(raySize);
        auto blockAtPositionOfTheRay =
            Block::Coordinate::nonBlockToBlockMetric(positionAtTheEndOfRay);

        if (const auto blockPtr = chunkManager.chunks().worldBlock(blockAtPositionOfTheRay))
        {
            if (tryHighlightBlock(positionAtTheEndOfRay, blockAtPositionOfTheRay, *blockPtr))
            {
                return;
            }
        }
    }
    dehighlightCurrentlySelectedBlock();
}

bool HighlightedBlock::tryHighlightBlock(const sf::Vector3<float>& positionAtTheEndOfRay,
                                         const Block::Coordinate& blockAtPositionOfTheRay,
                                         const Block& block)
{
    if (isBlockHighlightable(block))
    {
        auto&& model3D = createWireframeBlock(blockAtPositionOfTheRay);
        auto&& blockPosition = blockAtPositionOfTheRay;
        auto&& blockFace =
            blockFaceClosestToThePoint(positionAtTheEndOfRay, blockAtPositionOfTheRay);

        mSelectedBlock.emplace(std::move(model3D), blockPosition, blockFace);
        return true;
    }
    return false;
}

bool HighlightedBlock::isBlockHighlightable(const Block& block)
{
    switch (block.blockId())
    {
        case BlockId::Air:
        case BlockId::Water://
            return false;
        default: return true;
    }
}

Block::Face HighlightedBlock::blockFaceClosestToThePoint(
    const sf::Vector3<float>& nonMetricPoint, const Block::Coordinate& blockAtPointPosition) const
{
    auto nonMetricPositionOfBlockAtRayPosition = blockAtPointPosition.nonBlockMetric();
    auto positionOfRayInsideTheBlock =
        nonMetricPoint - sf::Vector3f(nonMetricPositionOfBlockAtRayPosition);

    Block::Face closestBlockFaceToTheRay =
        closestFaceToLocalBlockPoint(positionOfRayInsideTheBlock);
    return closestBlockFaceToTheRay;
}

Block::Face HighlightedBlock::closestFaceToLocalBlockPoint(
    const sf::Vector3<float>& localPointInsideTheBlock) const
{
    auto smallestDistanceToBlockFace = MAX_RAY_SIZE;
    auto closestBlockFaceToTheRay = Block::Face::Top;
    for (auto blockFace = 0; blockFace < static_cast<int>(Block::Face::Counter); ++blockFace)
    {
        auto centerOfBlockFace = localPointInCenterOfBlockFace(static_cast<Block::Face>(blockFace));
        auto distanceToCenterOfBlockFace =
            glm::distance(glm::vec3{centerOfBlockFace.x, centerOfBlockFace.y, centerOfBlockFace.z},
                          glm::vec3{localPointInsideTheBlock.x, localPointInsideTheBlock.y,
                                    localPointInsideTheBlock.z});
        if (distanceToCenterOfBlockFace < smallestDistanceToBlockFace)
        {
            smallestDistanceToBlockFace = distanceToCenterOfBlockFace;
            closestBlockFaceToTheRay = static_cast<Block::Face>(blockFace);
        }
    }
    return closestBlockFaceToTheRay;
}

sf::Vector3f HighlightedBlock::localPointInCenterOfBlockFace(const Block::Face& blockFace) const
{
    switch (blockFace)
    {
        case Block::Face::Top:
            return {
                0.5f,
                1.0f,
                0.5f,
            };

        case Block::Face::Left:
            return {
                0.0f,
                0.5f,
                0.5f,
            };

        case Block::Face::Right:
            return {
                1.0f,
                0.5f,
                0.5f,
            };

        case Block::Face::Bottom:
            return {
                0.5f,
                0.0f,
                0.5f,
            };

        case Block::Face::Front:
            return {
                0.5f,
                0.5f,
                1.0f,
            };

        case Block::Face::Back:
            return {
                0.5f,
                0.5f,
                0.0f,
            };
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

void HighlightedBlock::dehighlightCurrentlySelectedBlock()
{
    if (mSelectedBlock)
    {
        mSelectedBlock.reset();
    }
}

Model3D HighlightedBlock::createWireframeBlock(const Block::Coordinate& facedBlock)
{
    auto wireframeBlockModel = Model3D();
    wireframeBlockModel.setLayout(createBufferLayout());
    wireframeBlockModel.setMesh(createMeshOfHighlightedBlock(facedBlock));
    return wireframeBlockModel;
}

BufferLayout HighlightedBlock::createBufferLayout() const
{
    BufferLayout bufferLayout;
    bufferLayout.push<GLfloat>(3);
    return bufferLayout;
}

Mesh3D HighlightedBlock::createMeshOfHighlightedBlock(const Block::Coordinate& facedBlock) const
{
    MeshBuilder wireframeMeshBuilder;
    auto facedBlockNonMetric = facedBlock.nonBlockMetric();
    auto wireframeBlock =
        WireframeBlock(static_cast<sf::Vector3f>(facedBlockNonMetric),
                       static_cast<sf::Vector3f>(facedBlockNonMetric) +
                           sf::Vector3f{Block::BLOCK_SIZE, Block::BLOCK_SIZE, Block::BLOCK_SIZE});

    wireframeMeshBuilder.addWireframeBlock(wireframeBlock);
    auto meshOfSelectedBlock = wireframeMeshBuilder.mesh3D();
    return meshOfSelectedBlock;
}

Block::Coordinate HighlightedBlock::blockPosition() const
{
    /*
     * TODO: Consider if this error is a good practice
     * This function may return a std::optional, but... for now
     * it provides isAnyBlockHighlighted() to avoid this error
     */
    if (!isAnyBlockHighlighted())
    {
        throw std::runtime_error(
            "No block is selected. Can not return position of selected coordinateInGivenDirection");
    }

    return mSelectedBlock->position;
}

Block::Face HighlightedBlock::blockFace() const
{
    /*
     * TODO: Consider if this error is a good practice
     * This function may return a std::optional, but... for now
     * it provides isAnyBlockHighlighted() to avoid this error
     */
    if (!isAnyBlockHighlighted())
    {
        throw std::runtime_error(
            "No block is selected. Can not return face of selected coordinateInGivenDirection");
    }

    return mSelectedBlock->face;
}

void HighlightedBlock::draw(const Renderer3D& renderer3d, const sf::Shader& wireframeShader) const
{
    if (isAnyBlockHighlighted())
    {
        glDisable(GL_DEPTH_TEST);
        mSelectedBlock->model3D.draw(renderer3d, mWireframeShader);
        glEnable(GL_DEPTH_TEST);
    }
}

bool HighlightedBlock::isAnyBlockHighlighted() const
{
    return mSelectedBlock.has_value();
}

void HighlightedBlock::update(const float& deltaTime, Camera& camera, ChunkManager& chunkManager)
{
    camera.updateViewProjection(mWireframeShader);
    markFacedBlock(camera, chunkManager);
}

HighlightedBlock::SelectedBlock::SelectedBlock(Model3D&& wireframeModel3d,
                                               const Block::Coordinate& positionOfBlock,
                                               const Block::Face& faceOfBlock)
    : model3D(std::move(wireframeModel3d))
    , position(positionOfBlock)
    , face(faceOfBlock)
{
}
