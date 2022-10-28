#pragma once
#include <utility>

template<typename T>
std::pair<typename T::const_iterator, typename T::const_iterator> const_iterator_range(
    const T& container)
{
    return std::make_pair(std::cbegin(container), std::cend(container));
}

template<typename T>
std::pair<typename T::iterator, typename T::iterator> iterator_range(T& container)
{
    return std::make_pair(std::begin(container), std::end(container));
}