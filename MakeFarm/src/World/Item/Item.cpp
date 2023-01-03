#include "Item.h"
#include "ItemMap.h"
#include "World/Block/BlockMap.h"
#include "pch.h"

Item::Item(const ItemId& itemId)
    : mItemType(&ItemMap::itemMap().itemType(itemId))
{
}

void Item::setItemType(const ItemId& itemId)
{
    mItemType = &ItemMap::itemMap().itemType(itemId);
}

ItemId Item::id() const
{
    return mItemType->id;
}

int Item::maximumAmountInSlot() const
{
    return mItemType->maxAmountInSlot;
}

Item::Render Item::renderType() const
{
    return mItemType->render;
}

std::string Item::name() const
{
    return mItemType->name;
}

unsigned int Item::textureId() const
{
    switch (renderType())
    {
        case Item::Render::Block:
            return static_cast<unsigned int>(BlockMap::blockMap()
                                                 .blockType(static_cast<BlockId>(id()))
                                                 .textureId.at(Block::Face::Front));
        case Item::Render::Item: return static_cast<unsigned int>(id()) % 1000;
    }
}
bool Item::isBlock() const
{
    return renderType() == Item::Render::Block;
}
