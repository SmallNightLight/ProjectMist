#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/FixedTypes.h"

struct BoxCollider
{
    Fixed16_16 Width;
    Fixed16_16 Height;

    BoxCollider() : Width(0), Height(0) { }
    BoxCollider(Fixed16_16 width, Fixed16_16 height) : Width(width), Height(height) { }
};