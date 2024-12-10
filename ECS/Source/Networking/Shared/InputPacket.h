#pragma once

#include <bitset>

template <std::size_t KeyCount>
class InputPackage
{
    std::bitset<KeyCount> Keys;
};