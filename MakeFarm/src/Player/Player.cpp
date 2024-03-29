#include "Player.h"
#include "Player/GUI/Bars/Hotbar.h"
#include "Utils/Mouse.h"
#include "World/Chunks/ChunkContainer.h"
#include "pch.h"
#include "utils/utils.h"
#include <SFML/Graphics/Shader.hpp>
#include <filesystem>

Player::Player(const sf::Vector3f& position, sf::RenderWindow& gameWindow, sf::Shader& shader,
               ChunkManager& chunkManager, const GameResources& gameResources,
               const std::string& savedWorldPath)
    : mCamera(gameWindow, shader)
    , mPosition({position.x, position.y, position.z})
    , mAABB({Block::BLOCK_SIZE * 0.5f, Block::BLOCK_SIZE * 1.8f, Block::BLOCK_SIZE * 0.5f})
    , mWaterInWaterEffect(sf::Vector2f(gameWindow.getSize().x, gameWindow.getSize().y))
    , mSelectedBlock()
    , mChunkManager(chunkManager)
    , mCrosshairTexture()
    , mCrosshair()
    , mSpawnPoint(position)
    , mInventory(gameWindow, gameResources, savedWorldPath)
    , mHealthbar(gameResources.textureManager, mInventory.hotbar().position())
    , mOxygenbar(gameResources.textureManager, {0, 0})
    , mSavedWorldPath(savedWorldPath)
    , mGameWindow(gameWindow)
{
    auto newOxygenPosition = mInventory.hotbar().position();
    mOxygenbar.position(
        {newOxygenPosition.x, newOxygenPosition.y - mOxygenbar.size().y + DiscreteBar::OFFSET_Y});

    mCrosshairTexture.loadFromFile("resources/textures/crosshair.png");
    mCrosshair.setTexture(mCrosshairTexture);
    centerOrigin(mCrosshair);
    mCrosshair.setPosition(gameWindow.getSize().x / 2.f, gameWindow.getSize().y / 2.f);

    mWireframeShader.loadFromFile("resources/shaders/WireframeRenderer/VertexShader.shader",
                                  "resources/shaders/WireframeRenderer/GeometryShader.shader",
                                  "resources/shaders/WireframeRenderer/FragmentShader.shader");

    auto waterColor = sf::Color(49, 103, 189, 150);
    mWaterInWaterEffect.setFillColor(waterColor);

    loadSavedPlayerData();
}

void Player::update(const float& deltaTime)
{
    mCamera.update(deltaTime);
    mCamera.updateViewProjection(mWireframeShader);
    mSelectedBlock.update(deltaTime, mCamera, mChunkManager);
    mInventory.update(deltaTime);
    checkFallingDamage();
    updatePlayerDrowingState();
}

void Player::updateDebugMenu()
{
    mCamera.updateDebugMenu();
}

void Player::updatePlayerDrowingState()
{
    if (mArePlayerEyesInWater)
    {
        if (mPlayerDrowingTimer.getElapsedTime().asSeconds() > 0.5f)
        {
            if (mPlayerOxygen == 0)
            {
                takeDamage(0.5f);
            }

            mPlayerOxygen -= 0.5f;
            mOxygenbar.oxygen(mPlayerOxygen);
            mPlayerDrowingTimer.restart();
        }
    }
    else
    {
        mPlayerOxygen = 10;
        mOxygenbar.oxygen(mPlayerOxygen);
        mPlayerDrowingTimer.restart();
    }
}

void Player::checkFallingDamage()
{
    if (mIsPlayerOnGround && !mWasPlayerOnGroundBefore && !mIsPlayerInWater)
    {
        auto takenDamage = fallingVelocityToDamage(mFallingVelocityBeforeHittingGround);
        if (takenDamage.atLeastHalf())
        {
            takeDamage(takenDamage);
        }
    }
    mWasPlayerOnGroundBefore = mIsPlayerOnGround;
}

void Player::takeDamage(const DiscreteBarValue& takenDamage)
{
    mPlayerHealth -= takenDamage;
    mHealthbar.hearts(mPlayerHealth);
}

DiscreteBarValue Player::fallingVelocityToDamage(float fallingVelocity)
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
    return true;
}

void Player::handleMovementKeyboardInputs(const float& deltaTime)
{
    if (mAreControlsEnabled)
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
    if (mAreControlsEnabled)
    {
        Mouse::handleFirstPersonBehaviour(event, mGameWindow);
    }

    switch (event.type)
    {
        case sf::Event::KeyPressed: handleKeyboardEvents(event); break;
        case sf::Event::MouseButtonPressed: handleMouseEvents(event); break;
    }
}

void Player::handleKeyboardEvents(const sf::Event& event)
{
    if (mAreControlsEnabled)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Space: tryJump(); break;

#ifdef _DEBUG
            case sf::Keyboard::F2: takeDamage(0.5); break;
            case sf::Keyboard::F3: mIsFlying = !mIsFlying; break;
#endif
        }
    }

    if (event.key.code == sf::Keyboard::E)
    {
        toggleControls();
        mInventory.toggleInventory();
        mCamera.toggleControls();
    }
}

void Player::handleMouseEvents(const sf::Event& event)
{
    if (mAreControlsEnabled)
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
                auto itemInHand = mInventory.hotbar().itemInHand();
                if (itemInHand.has_value() && itemInHand.value().isBlock())
                {
                    tryPlaceBlock();
                }
                break;
            }
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
                // TODO: Check if block was placed first so it can be decided
                //       later to remove item from inventory
                mChunkManager.chunks().tryToPlaceBlock(
                    static_cast<BlockId>(removedItem.value()), coordinatesOfBlockToBePlaced,
                    {HighlightedBlock::BLOCKS_THAT_MIGHT_BE_OVERPLACED}, RebuildOperation::Fast);
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
    if (mArePlayerEyesInWater)
    {
        mOxygenbar.draw(target, states);
    }
}

bool Player::isDead() const
{
    return mPlayerHealth == 0;
}

std::string Player::playerSaveFilePath()
{
    return mSavedWorldPath + "/player.bin";
}

void Player::savePlayerDataToFile()
{
    auto playerPosition = position();
    mSerializer.serialize(playerPosition.x, playerPosition.y, playerPosition.z,
                          static_cast<float>(mPlayerHealth), mSpawnPoint.x, mSpawnPoint.y,
                          mSpawnPoint.z);
    mSerializer.saveToFile(playerSaveFilePath());
}

void Player::loadSavedPlayerData()
{
    std::ifstream file(playerSaveFilePath(), std::ios::binary);
    if (file.is_open())
    {
        auto playerHealth = 0.f;
        mSerializer.readSerialized(file, mPosition.x, mPosition.y, mPosition.z, playerHealth,
                                   mSpawnPoint.x, mSpawnPoint.y, mSpawnPoint.z);
        // TODO: Position has a little problem when reading that may cause the player to stuck
        //       in the ground for some reason. This little offset helps with it slightly
        mPosition += glm::vec3{0, Block::BLOCK_SIZE / 2.f, 0};
        mPlayerHealth = playerHealth;
        mHealthbar.hearts(mPlayerHealth);
    }
}

void Player::respawn()
{
    mHealthbar.hearts(10);
    mPlayerHealth = 10;
    mPosition = {mSpawnPoint.x, mSpawnPoint.y, mSpawnPoint.z};
    mInventory.clear();
    mCamera.rotation(0);
}

void Player::toggleControls()
{
    mAreControlsEnabled = !mAreControlsEnabled;
}


Player::~Player()
{
    savePlayerDataToFile();
}
