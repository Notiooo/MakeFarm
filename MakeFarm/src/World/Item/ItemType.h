#pragma once
#include "World/Item/Item.h"
#include "string"
#include "unordered_map"
#include <optional>

struct ItemType
{
    static constexpr auto DEFAULT_MAXIMUM_AMOUNT_IN_SLOT = 64;

    ItemId id;
    std::string fileName;
    std::string name;
    Item::Render render;
    std::optional<Item::TextureId> textureId;
    int maxAmountInSlot;
};
