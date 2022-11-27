#pragma once
#include "World/Item/Item.h"

/**
 * @brief Representation of a slot in an inventory
 */
struct InventorySlot
{
    InventorySlot(int amount, const ItemId& itemId);

    int amount;
    Item item;
};