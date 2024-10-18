#pragma once

#include "FixedTypes.h"

struct Rect
{
    Vector2 Position;
    Vector2 Size;

    constexpr Rect() : Position(0, 0), Size(1, 1) { }
    explicit constexpr Rect(const Vector2& position, const Vector2& size = {1, 1}) : Position(position), Size(size) { }

    [[nodiscard]] constexpr bool Contains(const Vector2& point) const
    {
        return !(point.X < Position.X || point.Y < Position.Y || point.X >= (Position.X + Size.X) || point.Y >= (Position.Y + Size.Y));
    }

    [[nodiscard]] constexpr bool Contains(const Rect& rect) const
    {
        return (rect.Position.X >= Position.X) && (rect.Position.X + rect.Size.X < Position.X + Size.X) &&
        (rect.Position.Y >= Position.Y) && (rect.Position.Y + rect.Size.Y < Position.Y + Size.Y);
    }

    [[nodiscard]] constexpr  bool Overlaps(const Rect& rect) const
    {
        return (Position.X < rect.Position.X + rect.Size.X && Position.X + Size.X >= rect.Position.X &&
        Position.Y < rect.Position.Y + rect.Size.Y && Position.Y + Size.Y >= rect.Position.Y);
    }
};