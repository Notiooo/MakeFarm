#include "pch.h"
#include "Block.h"

#include "Renderer3D/BufferLayout.h"

bool Block::isActive()
{
	return true;
}

void Block::setActive(bool active)
{
}

void Block::generateCube(unsigned x, unsigned y, unsigned z)
{
    float vertices[] =
    {
        // bottom
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 1.f, // further left bottom
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 1.f, // further right bottom
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 0.f, // closer left bottom
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 0.f, // closer right bottom
        //                       + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
        // upper                 + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 1.f, // further left upper
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 1.f, // further right upper
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 0.f, // closer left upper
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 0.f,  // closer right upper
        //                       + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
        // sides                 + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 1.f, //left up (8)
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 1.f, //right up (9)
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 0.f, //left down (10)
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 0.f, //right down (11)
        //                       + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 1.f, //left up (12)
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 1.f, //right up (13)
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 0.f, //left down (14)
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 0.f,  //right down (15)
        //                       + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
        -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 1.f, 1.f,
        //                       + (Block::BLOCK_SIZE * x)                            + (Block::BLOCK_SIZE * y)                            + (Block::BLOCK_SIZE * z)
         1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * x),  -1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * y),   1.f * Block::BLOCK_SIZE + (Block::BLOCK_SIZE * z), 0.f, 0.f
    };

    unsigned int indices[] = {
        0, 1, 2, //bottom
        2, 3, 1,

        4, 5, 6, //upper
        6, 7, 5,

        14, 15, 13, // front
        13, 12, 14,

        10, 11, 9, //back
        9, 8, 10,

        14, 0, 16, // left
        16, 15, 14,

        17, 8, 9, // right
        9, 7, 17

    };

    vb = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    BufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    va.setBuffer(*vb, layout);

    ib = std::make_unique<IndexBuffer>(indices, 36);

    va.unbind();
    vb->unbind();
    ib->unbind();
}

void Block::draw(const Renderer3D& renderer3d, const sf::Shader& shader) const
{
    renderer3d.draw(va, *ib, shader);
}
