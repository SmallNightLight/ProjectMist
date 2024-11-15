#pragma once
#include <array>

template <std::size_t KeyCount>
struct Input
{
    std::array<bool, KeyCount> Keys { };
};
