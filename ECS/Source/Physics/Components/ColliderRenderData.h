#pragma once

#include "../../Math/Stream.h"

struct ColliderRenderData
{
    uint8_t R, G, B;

    inline ColliderRenderData() noexcept = default;

    inline constexpr explicit ColliderRenderData(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) { }

    inline explicit ColliderRenderData(Stream& stream)
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

static_assert(std::is_trivially_default_constructible_v<ColliderRenderData>, "Needs to be trivial");