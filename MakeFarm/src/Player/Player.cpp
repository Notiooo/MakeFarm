#include "Player.h"
#include "Player/GUI/Hotbar.h"
#include "World/Chunks/ChunkContainer.h"
#include "pch.h"
#include "utils/utils.h"
#include <SFML/Graphics/Shader.hpp>
#include <filesystem>
#include <serializer.h>

Player::Player(const sf::Vector3f& position, const sf::RenderTarget& target, sf::Shader& shader,
               ChunkManager& chunkManager, const GameResources& gameResources,
               const std::string& savedWorldPath)
    : mCamera(target, shader)
    , mPosition({position.x, position.y, position.z})
    , mAABB({Block::BLOCK_SIZE * 0.5f, Block::BLOCK_SIZE * 1.8f, Block::BLOCK_SIZE * 0.5f})
    , mWaterInWaterEffect(sf::Vector2f(target.getSize().x, target.getSize().y))
    , mSelectedBlock()
    , mChunkManager(chunkManager)
    , mCrosshairTexture()
    , mCrosshair()
    , mInventory(sf::Vector2i(target.getSize()), gameResources, savedWorldPath)
    , mHealthbar(gameResources.textureManager, mInventory.hotbar().position())
    , mSavedWorldPath(savedWorldPath)
{
    mCrosshairTexture.loadFromFile("resources/textures/crosshair.png");
    mCrosshair.setTexture(mCrosshairTexture);
    centerOrigin(mCrosshair);
    mCrosshair.setPosition(target.getSize().x / 2.f, target.getSize().y / 2.f);

    mWireframeShader.loadFromFile("resources/shaders/WireframeRenderer/VertexShader.shader",
                                  "resources/shaders/WireframeRenderer/GeometryShader.shader",
                                  "resources/shaders/WireframeRenderer/FragmentShader.shader");

    auto waterColor = sf::Color(49, 103, 189, 120);
    mWaterInWaterEffect.setFillColor(waterColor);

    std::ifstream file(playerSaveFilePath(), std::ios::binary);
    if (file.is_open())
    {
        readSerializedPlayer(file);
    }
}

void Player::update(const float& deltaTime)
{
    mCamera.update(deltaTime);
    mCamera.updateViewProjection(mWireframeShader);
    mSelectedBlock.update(deltaTime, mCamera, mChunkManager);
    mInventory.update(deltaTime);
    checkFallingDamage();
}

void Player::checkFallingDamage()
{
    if (mIsPlayerOnGround && !mWasPlayerOnGroundBefore && !mIsPlayerInWater)
    {
        auto takenDamage = fallingVelocityToDamage(mFallingVelocityBeforeHittingGround);
        if (takenDamage.atLeastHalfHeart())
        {
            takeDamage(takenDamage);
        }
    }
    mWasPlayerOnGroundBefore = mIsPlayerOnGround;
}

void Player::takeDamage(const Hearts& takenDamage)
{
    mPlayerHealth -= takenDamage;
    mHealthbar.hearts(mPlayerHealth);
}

Hearts Player::fallingVelocityToDamage(float fallingVelocity)
{
    auto takenDamage = std::abs(fallingVelocity);
    takenDamage *= 10;
    takenDamage -= 1.5f;
    return takenDamage;
}

void Player::fixedUpdate(const float& deltaTime, const ChunkContainer& chunkContainer)
{
    updateVelocity(deltaTime);
    updatePhysics(chunkContainer);

    mCamera.cameraPosition({mPosition.x, mPosition.y + PLAYER_HEIGHT, mPosition.z});
}

void Player::updatePhysics(const ChunkContainer& chunkContainer)
{
    updatePositionCheckingPhysicalCollisions(chunkContainer);
    updateInformationIfPlayerIsInWater(chunkContainer);
}

void Player::updateInformationIfPlayerIsInWater(const ChunkContainer& chunkContainer)
{
    if (doesPlayerTouchesWater(chunkContainer))
    {
        mIsPlayerInWater = true;
        updateInformationIfPlayersEyesAreInWater(chunkContainer);
    }
    else
    {
        mIsPlayerInWater = false;
        mArePlayerEyesInWater = false;
    }
}

void Player::updateInformationIfPlayersEyesAreInWater(const ChunkContainer& chunkContainer)
{
    mArePlayerEyesInWater =
        doesItCollideWithGivenNonAirBlock(aabbHeadAboveEyes(), chunkContainer, BlockId::Water);
}

bool Player::doesItCollideWithGivenNonAirBlock(const AABB& aabb,
                                               const ChunkContainer& chunkContainer,
                                               BlockId blockId) const
{
    auto nonAirBlocksPlayerTouched = chunkContainer.nonAirBlocksItTouches(aabb);
    return std::find_if(nonAirBlocksPlayerTouched.cbegin(), nonAirBlocksPlayerTouched.cend(),
                        [&blockId](const auto& block)
                        {
                            return block.id() == blockId;
                        }) != nonAirBlocksPlayerTouched.cend();
}

AABB Player::aabbHeadAboveEyes() const
{
    auto collisionBoxSize = mAABB.collisionBoxSize();
    collisionBoxSize.y = PLAYER_EYES_LEVEL;

    auto position = mPosition;
    position.y += PLAYER_HEIGHT - PLAYER_EYES_LEVEL;

    auto aabb = AABB(collisionBoxSize);
    aabb.updatePosition(position, AABB::RelativeTo::BottomCenter);
    return aabb;
}

bool Player::doesPlayerTouchesWater(const ChunkContainer& chunkContainer) const
{
    return doesItCollideWithGivenNonAirBlock(collisionBox(), chunkContainer, BlockId::Water);
}

void Player::updatePositionCheckingPhysicalCollisions(const ChunkContainer& chunkContainer)
{
    tryUpdatePositionByApplyingVelocityIfCollisionAllows(chunkContainer, mPosition.x, mVelocity.x);

    float fallingVelocity = mVelocity.y;
    if (tryUpdatePositionByApplyingVelocityIfCollisionAllows(chunkContainer, mPosition.y,
                                                             mVelocity.y))
    {
        mIsPlayerOnGround = false;
    }
    else
    {
        mIsPlayerOnGround = true;
        mFallingVelocityBeforeHittingGround = fallingVelocity;
    }

    tryUpdatePositionByApplyingVelocityIfCollisionAllows(chunkContainer, mPosition.z, mVelocity.z);
}

bool Player::tryUpdatePositionByApplyingVelocityIfCollisionAllows(
    const ChunkContainer& chunkContainer, float& position, float& velocity)
{
    position += velocity;
    mAABB.updatePosition(mPosition, AABB::RelativeTo::BottomCenter);
    if (chunkContainer.doesItCollide(collisionBox()))
    {
        position -= velocity;
        velocity = 0;
        return false;
    }
    mAABB.updatePosition(mPosition, AABB::RelativeTo::BottomCenter);
    return true;
}

void Player::handleMovementKeyboardInputs(const float& deltaTime)
{
    constexpr auto ACCELERATION_RATIO = 0.1f;
    auto playerSpeed = mIsPlayerInWater ? PLAYER_WALKING_SPEED_IN_WATER : PLAYER_WALKING_SPEED;
    const auto finalSpeed = playerSpeed * ACCELERATION_RATIO * deltaTime;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        mVelocity += finalSpeed * mCamera.directionWithoutPitch();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        mVelocity += -(finalSpeed * mCamera.directionWithoutPitch());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        mVelocity += finalSpeed * mCamera.rightDirectionWithoutPitch();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        mVelocity += -(finalSpeed * mCamera.rightDirectionWithoutPitch());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && mIsPlayerInWater)
    {
        mVelocity += (finalSpeed * 0.5f * mCamera.upwardDirection());
    }
    handleFlyingKeyboardInputs(deltaTime);
}

void Player::handleFlyingKeyboardInputs(const float& deltaTime)
{
    if (mIsFlying)
    {
        const auto& horizontalSpeed = PLAYER_WALKING_SPEED * deltaTime;
        auto upVelocity = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ? horizontalSpeed : 0;
        auto downVelocity = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? -horizontalSpeed : 0;
        mVelocity.y = upVelocity + downVelocity;
    }
}

void Player::handleEvent(const sf::Event& event)
{
    mInventory.handleEvent(event);

    switch (event.type)
    {
        case sf::Event::KeyPressed: handleKeyboardEvents(event); break;
        case sf::Event::MouseButtonPressed: handleMouseEvents(event); break;
    }
}
void Player::handleKeyboardEvents(const sf::Event& event)
{
    switch (event.key.code)
    {
        case sf::Keyboard::Space: tryJump(); break;
#ifdef _DEBUG
        case sf::Keyboard::F1: takeDamage(0.5); break;
        case sf::Keyboard::F2: mIsFlying = !mIsFlying; break;
#endif
    }
}

void Player::handleMouseEvents(const sf::Event& event)
{
    switch (event.mouseButton.button)
    {
        case sf::Mouse::Left:
        {
            tryDestroyBlock();
            break;
        }
        case sf::Mouse::Right:
        {
            tryPlaceBlock();
            break;
        }
    }
}

void Player::tryJump()
{
    if (mIsPlayerOnGround && !mIsPlayerInWater && !mIsFlying)
    {
        mVelocity.y = PLAYER_JUMP_FORCE * 0.1f;
    }
}

void Player::tryDestroyBlock()
{
    if (mSelectedBlock.isAnyBlockHighlighted())
    {
        auto itemItDrops =
            mChunkManager.chunks().worldBlock(mSelectedBlock.blockPosition())->itemItDrops();
        mChunkManager.chunks().removeWorldBlock(mSelectedBlock.blockPosition());
        if (itemItDrops.has_value())
        {
            mInventory.tryAddItem(itemItDrops.value());
        }
    }
}

void Player::tryPlaceBlock()
{
    if (mSelectedBlock.isAnyBlockHighlighted())
    {
        auto relativeDirectionWhereBlockToBePlaced =
            Block::directionOfFace(mSelectedBlock.blockFace());

        auto coordinatesOfBlockToBePlaced =
            mSelectedBlock.blockPosition().coordinateInGivenDirection(
                relativeDirectionWhereBlockToBePlaced);

        if (!doesPlayerCollideWithBlock(coordinatesOfBlockToBePlaced))
        {
            auto removedItem = mInventory.hotbar().tryRemoveItemInHand(1);
            if (removedItem.has_value())
            {
                // TODO: Check if block was placed first so it can be decided later to remove item
                // from inventory
                mChunkManager.chunks().tryToPlaceBlock(
                    static_cast<BlockId>(removedItem.value()), coordinatesOfBlockToBePlaced,
                    {HighlightedBlock::BLOCKS_THAT_MIGHT_BE_OVERPLACED},
                    Chunk::RebuildOperation::Fast);
            }
        }
    }
}

bool Player::doesPlayerCollideWithBlock(const Block::Coordinate& coordinates) const
{
    auto blockToBePlacedAABB =
        AABB(sf::Vector3f(Block::BLOCK_SIZE, Block::BLOCK_SIZE, Block::BLOCK_SIZE));

    blockToBePlacedAABB.updatePosition(coordinates.nonBlockMetric(),
                                       AABB::RelativeTo::LeftBottomBack);

    return collisionBox().intersect(blockToBePlacedAABB);
}

void Player::updateVelocity(const float& deltaTime)
{
    handleMovementKeyboardInputs(deltaTime);
    decelerateVelocity(deltaTime);
    manageVerticalVelocity(deltaTime);
    limitVelocity(deltaTime);
}

void Player::decelerateVelocity(const float& deltaTime)
{
    mVelocity.x = mVelocity.x - mVelocity.x * PLAYER_WALKING_DECELERATE_RATIO * deltaTime;
    mVelocity.z = mVelocity.z - mVelocity.z * PLAYER_WALKING_DECELERATE_RATIO * deltaTime;
}

void Player::manageVerticalVelocity(const float& deltaTime)
{
    if (mIsFlying)
    {
        return;
    }

    if (mIsPlayerInWater)
    {
        mVelocity.y -= 0.004f * deltaTime;
    }
    else
    {
        mVelocity.y -= (abs(mVelocity.y) * 0.1f + 0.3f) * deltaTime;
    }
}

void Player::limitVelocity(const float& deltaTime)
{
    static auto limitVelocitySpeed = [](auto& velocity, const auto& maxVelocity)
    {
        if (velocity > maxVelocity)
        {
            velocity = maxVelocity;
        }
        else if (-velocity > maxVelocity)
        {
            velocity = -maxVelocity;
        }
    };

    auto playerVelocity = PLAYER_MAX_HORIZONTAL_SPEED * deltaTime;
    limitVelocitySpeed(mVelocity.x, playerVelocity);
    limitVelocitySpeed(mVelocity.z, playerVelocity);

    if (mIsPlayerInWater && mVelocity.y > playerVelocity)
    {
        mVelocity.y = playerVelocity;
    }

    if (-mVelocity.y > PLAYER_MAX_FALLING_SPEED)
    {
        mVelocity.y = -PLAYER_MAX_FALLING_SPEED;
    }
}

glm::vec3 Player::position() const
{
    return mPosition;
}

const AABB& Player::collisionBox() const
{
    return mAABB;
}

Camera& Player::camera()
{
    return mCamera;
}

void Player::draw(const Renderer3D& renderer3D, sf::RenderTarget& target,
                  sf::RenderStates states) const
{
    if (mIsPlayerInWater && mArePlayerEyesInWater)
    {
        SfmlDraw(mWaterInWaterEffect, target, states);
    }

    mSelectedBlock.draw(renderer3D, mWireframeShader);
    SfmlDraw(mCrosshair, target, states);
    mInventory.draw(target, states);
    mHealthbar.draw(target, states);
}

bool Player::isDead() const
{
    return mPlayerHealth == 0;
}

std::vector<unsigned char> Player::serializedPlayer()
{
    auto playerPosition = position();
    std::vector<unsigned char> serializedData;
    zpp::serializer::memory_output_archive out(serializedData);
    out(playerPosition.x, playerPosition.y, playerPosition.z, static_cast<float>(mPlayerHealth));
    return serializedData;
}

Player::~Player()
{
    std::vector<unsigned char> serializedData = serializedPlayer();

    std::filesystem::create_directories(mSavedWorldPath);
    std::ofstream outfile(playerSaveFilePath(), std::ios::out | std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(serializedData.data()),
                  serializedData.size() * sizeof(char));
    outfile.close();
}

std::string Player::playerSaveFilePath()
{
    return mSavedWorldPath + "/player.bin";
}

void Player::readSerializedPlayer(std::ifstream& file)
{
    // TODO: This thing is actually repeated in three places. Would be got to extract it
    std::istreambuf_iterator<char> iter(file);
    std::vector<char> vec(iter, std::istreambuf_iterator<char>{});
    std::vector<unsigned char> data(vec.begin(), vec.end());
    zpp::serializer::memory_input_archive in(data);
    // ==========
    auto playerHealth = 0.f;
    in(mPosition.x, mPosition.y, mPosition.z, playerHealth);
    mPlayerHealth = playerHealth;
    mHealthbar.hearts(mPlayerHealth);
}
