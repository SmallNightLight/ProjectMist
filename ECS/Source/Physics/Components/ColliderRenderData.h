#pragma once

#include "../../Math/Stream.h"

struct ColliderRenderData
{
    uint8_t R, G, B;

    ColliderRenderData() : R(255), G(255), B(255) { }
    ColliderRenderData(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) { }
    explicit ColliderRenderData(Stream& stream)
    {
        R = stream.ReadInteger<uint8_t>();
        G = stream.ReadInteger<uint8_t>();
        B = stream.ReadInteger<uint8_t>();
    }

    void Serialize(Stream& stream) const
    {
        stream.WriteInteger<uint8_t>(R);
        stream.WriteInteger<uint8_t>(G);
        stream.WriteInteger<uint8_t>(B);
    }
};