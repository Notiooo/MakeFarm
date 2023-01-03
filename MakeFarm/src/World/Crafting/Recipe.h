#pragma once
#include "Utils/MultiDimensionalArray.h"
#include "World/Item/ItemId.h"

/**
 * @brief A recipe that contains information on what layout determines what resultant item and in
 * what quantity.
 */
struct Recipe
{
    MultiDimensionalArray<ItemId, 3, 3> layout;
    ItemId finalItem;
    int finalAmount = 1;
};