#pragma once
#include "Physics/AABB.h"
#include "Player/GUI/Bars/Healthbar.h"
#include "Player/GUI/Bars/Oxygenbar.h"
#include "Player/GUI/Inventory.h"
#include "Utils/Serializer.h"
#include "World/Block/HighlightedBlock.h"
#include "World/Camera.h"
#include <SFML/Graphics/RectangleShape.hpp>

class ChunkContainer;

class Player
{
public:
    Player(const sf::Vector3f& position, const sf::RenderTarget& target, sf::Shader& shader,
           ChunkManager& chunkManager, const GameResources& gameResources,
           const std::string& savedWorldPath);

    ~Player();

    static constexpr float PLAYER_MAX_FALLING_SPEED = 20.f;
    static constexpr float PLAYER_WALKING_SPEED = 5.f;
    static constexpr float PLAYER_MAX_HORIZONTAL_SPEED = 5.f;
    static constexpr float PLAYER_WALKING_DECELERATE_RATIO = 10.f;
    static constexpr float PLAYER_WALKING_SPEED_IN_WATER = 2.f;
    static constexpr float PLAYER_JUMP_FORCE = 0.9f;
    static constexpr float PLAYER_HEIGHT = Block::BLOCK_SIZE * 1.8f;
    static constexpr float PLAYER_EYES_LEVEL = PLAYER_HEIGHT * 0.05f;

    /**
     * \brief Draws player to the passed target
     * \param target where it should be drawn to
     * \param states provides information about rendering process (transform, shader, blend mode)
     */
    void draw(const Renderer3D& renderer3D, sf::RenderTarget& target,
              sf::RenderStates states) const;

    /**
     * Updates the Player logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief Updates the Player logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    void fixedUpdate(const float& deltaTime, const ChunkContainer& chunkContainer);

    /**
     * @brief Handles keyboard states such as button presses and responds accordingly
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void handleMovementKeyboardInputs(const float& deltaTime);

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief Returns the player's current position in space. This is a non-metric value.
     * @return Non-metric position of player.
     */
    glm::vec3 position() const;

    /**
     * @brief Returns the player's collision box. The collision box is as fresh as it was last
     * updated by the functions that take care of updating it.
     * @return Const reference to collision box of the player.
     */
    const AABB& collisionBox() const;

    /**
     * @brief Returns the player's camera, which is in place of his head.
     * @return Reference to camera inside the player.
     */
    Camera& camera();

    /**
     * @brief Checks if the player is dead
     * @return True if the player is dead, false if alive
     */
    bool isDead() const;

    /**
     * @brief Respawns the player back to his starting point and clean their inventory.
     */
    void respawn();

private:
    /**
     * @brief Updates the player's velocity.
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void updateVelocity(const float& deltaTime);

    /**
     * @brief Updates physics-related things like updating the collision box and checking physical
     * dependencies.
     * @param chunkContainer Container storing chunks
     */
    void updatePhysics(const ChunkContainer& chunkContainer);

    /**
     * @brief It tries to move the player's positions by the indicated velocity as long as the
     * object does not collide.
     * @param chunkContainer Container storing chunks
     */
    void updatePositionCheckingPhysicalCollisions(const ChunkContainer& chunkContainer);

    /**
     * @brief Updates information on player being in water.
     * @param chunkContainer Container storing chunks
     */
    void updateInformationIfPlayerIsInWater(const ChunkContainer& chunkContainer);

    /**
     * @brief Checks if player's collision box collides with water.
     * @param chunkContainer Container storing chunks
     * @return True if player's collision box touches water.
     */
    bool doesPlayerTouchesWater(const ChunkContainer& chunkContainer) const;

    /**
     * @brief It tries to update the position with the given speed by checking beforehand if there
     * is a collision. Only one dimension should be given.
     * @param chunkContainer Container storing chunks.
     * @param position Position as one dimension (x, y, z) on which the player is located
     * @param velocity Velocity of the player given as one dimension (x, y, z)
     * @return True if the position was successfully updated, false if there was a collision and the
     * position was not updated.
     */
    bool tryUpdatePositionByApplyingVelocityIfCollisionAllows(const ChunkContainer& chunkContainer,
                                                              float& position, float& velocity);

    /**
     * @brief Limits the player's velocity by the specified maximum ranges.
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void limitVelocity(const float& deltaTime);

    /**
     * @brief Correspondingly updates the player's vertical position by, for example, gravity in air
     * or water
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void manageVerticalVelocity(const float& deltaTime);

    /**
     * @brief Appropriately slows down the player's speed so that he or she will lose speed.
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void decelerateVelocity(const float& deltaTime);

    /**
     * @brief Creates a collision head box starting from the player's eyes
     * @return Returns collision box of player's head starting from eyes
     */
    AABB aabbHeadAboveEyes() const;

    /**
     * @brief Checks if a given collision box collides with a block with the given id
     * @param aabb The collision box, the collision of which should be checked
     * @param chunkContainer Container storing chunks
     * @param blockId Identifier of the block with which the collision should be checked
     * @return True if the collision occurred, false otherwise
     */
    bool doesItCollideWithGivenNonAirBlock(const AABB& aabb, const ChunkContainer& chunkContainer,
                                           BlockId blockId) const;

    /**
     * @brief Updates whether the player's eyes are below the water level.
     * @param chunkContainer Container storing chunks
     */
    void updateInformationIfPlayersEyesAreInWater(const ChunkContainer& chunkContainer);

    /**
     * @brief He is trying to put a block through the player - where the player is looking.
     */
    void tryPlaceBlock();

    /**
     * @brief Destroys the block marked by the player.
     */
    void tryDestroyBlock();

    /**
     * @brief Player jumps if it is on the ground.
     */
    void tryJump();

    /**
     * @brief Handles mouse-related player events.
     * @param event user input
     */
    void handleMouseEvents(const sf::Event& event);

    /**
     * @brief Handles keyboard-related player events.
     * @param event user input
     */
    void handleKeyboardEvents(const sf::Event& event);

    /**
     * @brief Handles the inputs responsible for flying.
     * @param deltaTime the time that has passed since the game was last updated.
     */
    void handleFlyingKeyboardInputs(const float& deltaTime);

    /**
     * @brief Checks if the player collides with the indicated block
     * @param coordinates Global block coordinates
     * @return True if player collides with block, false otherwise
     */
    bool doesPlayerCollideWithBlock(const Block::Coordinate& coordinates) const;

    /**
     * @brief Checks and handles the case in which the player should get the damage of falling from
     * a height.
     */
    void checkFallingDamage();

    /**
     * @brief Converts the fall speed from altitude to the received damage given in value.
     * @param fallingVelocity The speed at which the player falls.
     * @return Damage dealt to player given in value
     */
    DiscreteBarValue fallingVelocityToDamage(float fallingVelocity);

    /**
     * @brief Deals damage to the player
     * @param takenDamage The number of value a player should lose.
     */
    void takeDamage(const DiscreteBarValue& takenDamage);

    /**
     * @brief The path to the file that should contain the saved information about the player
     * @return Character string, which is a path to the player settings file
     */
    std::string playerSaveFilePath();

    /**
     * @brief Saves the state of the player data to a file
     */
    void savePlayerDataToFile();

    /**
     * @brief Reads the saved state of the player from the file
     * @warning Nothing happens when the save file is not there
     */
    void loadSavedPlayerData();

    /**
     * @brief Updates player status related to drowning in water
     */
    void updatePlayerDrowingState();

    Camera mCamera;
    AABB mAABB;
    HighlightedBlock mSelectedBlock;
    ChunkManager& mChunkManager;
    sf::Shader mWireframeShader;
    Inventory mInventory;
    Healthbar mHealthbar;
    Oxygenbar mOxygenbar;
    Serializer mSerializer;
    sf::Vector3f mSpawnPoint;

    bool mIsPlayerOnGround = false;
    bool mWasPlayerOnGroundBefore = true;
    bool mIsPlayerInWater = false;
    bool mArePlayerEyesInWater = false;
    bool mIsFlying = false;
    DiscreteBarValue mPlayerHealth = 10;
    const std::string& mSavedWorldPath;

    sf::Clock mPlayerDrowingTimer;
    DiscreteBarValue mPlayerOxygen = 10;

    sf::RectangleShape mWaterInWaterEffect;
    sf::Sprite mCrosshair;
    sf::Texture mCrosshairTexture;

    glm::vec3 mPosition;
    glm::vec3 mPlayerMovementVelocity = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 mFallingVelocity = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 mVelocity = glm::vec3(0.f, 0.f, 0.f);
    float mFallingVelocityBeforeHittingGround = 0.f;
};
