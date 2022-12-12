#include "Biome.h"
#include "pch.h"

Biome::Biome(int seed)
    : mSeed(seed)
{
}

int Biome::seed() const
{
    return mSeed;
}
