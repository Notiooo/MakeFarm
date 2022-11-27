#pragma once


// Convention is that Blocks ID's are 1:1
// Non-block items starts with 1000

// clang-format off
enum class ItemId
{
    Grass_Block       = 1,
    Dirt_Block        = 2,
    Stone_Block       = 3,
    Sand_Block        = 4,
    Log_Block         = 5,
    Leaves_Block      = 6,
    Water_Block       = 7,

    Wooden_Sword        = 1000,
    Wooden_Pickaxe      = 1001,

    Counter,
    AllItems
};
// clang-format on