#include "InventorySlot.h"
#include "pch.h"

InventorySlot::InventorySlot(int amount, const ItemId& itemId)
    : amount(amount)
    , item(itemId)
{
}