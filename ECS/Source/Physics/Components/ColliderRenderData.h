#pragma once

struct ColliderRenderData
{
    float R, G, B;
    bool Outline;

    ColliderRenderData() : R(1.0f), G(1.0f), B(1.0f), Outline(false){ }
    ColliderRenderData(float r, float g, float b) : R(r), G(g), B(b), Outline(false) { }
};