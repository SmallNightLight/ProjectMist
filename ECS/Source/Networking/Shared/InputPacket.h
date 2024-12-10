#pragma once

#include <bitset>
#include "../../Game/Input/Input.h"

template <std::size_t KeyCount>
struct InputPackage
{
    InputPackage(u_int32_t frame, const Input& input)
    {
        Frame = frame;
    }

    u_int32_t Frame;
    std::bitset<KeyCount> Keys;

    std::vector<char> Serialize()
    {

    }
};