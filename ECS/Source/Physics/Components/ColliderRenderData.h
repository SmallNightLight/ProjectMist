#pragma once

struct ColliderRenderData
{
    float R, G, B;

    ColliderRenderData() : R(1.0f), G(1.0f), B(1.0f) { }
    ColliderRenderData(float r, float g, float b) : R(r), G(g), B(b) { }
};