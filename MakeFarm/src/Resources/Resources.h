#ifndef RESOURCES_H
#define RESOURCES_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Resources/ResourceManager.h"

// ====== Textures ======= //

/**
 * \brief Texture IDs under which individual textures can be saved and read.
 */
enum class Textures_ID
{
	// No Textures yet
};

/**
 * \brief Object storing textures of the game
 */
using TextureManager = ResourceManager<sf::Texture, Textures_ID>;

// ====== Fonts ======= //

/**
 * \brief Fonts IDs under which individual font can be saved and read.
 */
enum class Fonts_ID
{
	ArialNarrow,
};

/**
 * \brief Object storing fonts of the game
 */
using FontManager = ResourceManager<sf::Font, Fonts_ID>;

#endif