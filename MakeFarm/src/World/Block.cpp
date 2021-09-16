#include "pch.h"
#include "Block.h"

#include "Renderer3D/BufferLayout.h"

Block::Block(const TexturePack& texturePack)
	: texturePack(texturePack)
{
}

bool Block::isActive()
{
	return true;
}

void Block::setActive(bool active)
{
}

void Block::generateCube(unsigned x, unsigned y, unsigned z)
{
	mesh.vertices =
	{
	//  x  y  z

		// Top
		0, 1, 1, // top close left
		1, 1, 1, // top close right
		1, 1, 0, // top far right
		0, 1, 0, // top far left

		// Back
		1, 0, 0, // back right bottom
		0, 0, 0, // back left bottom
		0, 1, 0, // back left top
		1, 1, 0, // back right top

		// Front
		0, 0, 1, // front left bottom
		1, 0, 1, // front right bottom
		1, 1, 1, // front right top
		0, 1, 1, // front left top

		// Left
		0, 0, 0,
		0, 0, 1,
		0, 1, 1,
		0, 1, 0,

		// Right
		1, 0, 1,
		1, 0, 0,
		1, 1, 0,
		1, 1, 1,

		// Bottom
		0, 0, 0,
		1, 0, 0,
		1, 0, 1,
		0, 0, 1
	};

	for (auto& vertic : mesh.vertices)
	{
		vertic = (vertic ? (BLOCK_SIZE / 2) : -(BLOCK_SIZE / 2));
	}

	for (auto i = 0; i < mesh.vertices.size(); i += 3)
	{
		mesh.vertices[i] += x * BLOCK_SIZE;
		mesh.vertices[i + 1] += y * BLOCK_SIZE;
		mesh.vertices[i + 2] += z * BLOCK_SIZE;
	}

	auto top = texturePack.getNormalizedCoordinates(BlockList::GrassTop);
	auto side = texturePack.getNormalizedCoordinates(BlockList::GrassSide);
	auto bottom = texturePack.getNormalizedCoordinates(BlockList::Dirt);

	auto& texCoords = mesh.textureCoordinates;

	texCoords.insert(texCoords.end(), top.begin(), top.end());
	texCoords.insert(texCoords.end(), side.begin(), side.end());
	texCoords.insert(texCoords.end(), side.begin(), side.end());
	texCoords.insert(texCoords.end(), side.begin(), side.end());
	texCoords.insert(texCoords.end(), side.begin(), side.end());
	texCoords.insert(texCoords.end(), bottom.begin(), bottom.end());

	mesh.indices = 
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	model.setMesh(mesh);
}

void Block::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
	texturePack.bind();
	model.draw(renderer3d, shader);
}
