#ifndef RESOURCES_H
#define RESOURCES_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Resources/ResourceManager.h"
#include "TexturePack.h"

// ====== Spritesheet ======= //

/**
 * \brief Texture IDs under which individual textures can be saved and read.
 */
enum class TextureManagerId
{
    // GUI
    GUI_Inventory,
    GUI_Inventory_Selected_Block,
    GUI_Inventory_Unselected_Block
};

/**
 * \brief Object storing textures of the game
 */
using TextureManager = ResourceManager<sf::Texture, TextureManagerId>;

// ====== Fonts ======= //

/**
 * \brief Fonts IDs under which individual font can be saved and read.
 */
enum class FontId
{
    ArialNarrow,
};

/**
 * \brief Object storing fonts of the game
 */
using FontManager = ResourceManager<sf::Font, FontId>;

/**
 * @brief Any game assets from textures or fonts
 */
struct GameResources
{
    TexturePack texturePack;
    TextureManager textureManager;
    FontManager fontManager;
};

#endif