#pragma once
#include <array>

/*
 * Example usage:
 *
 * I want to create array of ints of dimension: 5x2x6x9
 * Weird huh? But not really problematic. What I do is:
 *
 * MultiDimensionalArray<int, 5, 2, 6, 9> array;
 *
 * Lets follow the steps how does it unroll
 *
 * MultiDimensionalArray = std::array<MultiDimensionalArray<int, 2, 6, 9>, 5>;
 * MultiDimensionalArray = std::array<std::array<MultiDimensionalArray<int, 6, 9>, 2>, 5>;
 * MultiDimensionalArray = std::array<std::array<std::array<MultiDimensionalArray<int, 9>, 6>, 2>,
 * 5>; MultiDimensionalArray = std::array<std::array<std::array<std::array<int, 9>, 6>, 2>, 5>;
 *
 */

template<class Type, size_t ThisDimension, size_t... NextDimensions>
struct MultiDimensionalArrayImpl
{
    MultiDimensionalArrayImpl() = delete;
    using type = std::array<typename MultiDimensionalArrayImpl<Type, NextDimensions...>::type,
                            ThisDimension>;
};

template<class Type, size_t ThisDimension>
struct MultiDimensionalArrayImpl<Type, ThisDimension>
{
    MultiDimensionalArrayImpl() = delete;
    using type = std::array<Type, ThisDimension>;
};

template<class Type, size_t... Dimensions>
using MultiDimensionalArray = typename MultiDimensionalArrayImpl<Type, Dimensions...>::type;