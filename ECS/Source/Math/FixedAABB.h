#pragma once

template<class T>
struct FixedAABB
{
    T Min;
    T Max;

    inline FixedAABB() noexcept = default;
    inline constexpr explicit FixedAABB(const T& min, const T& max) : Min(min), Max(max) { }

    [[nodiscard]] constexpr bool Contains(const T& point) const
    {
        return !(point.X < Min.X || point.Y < Min.Y || point.X >= (Max.X) || point.Y >= (Max.Y));
    }

    [[nodiscard]] constexpr bool Contains(const FixedAABB& other) const
    {
        return (other.Min.X >= Min.X) && (other.Max.X < Max.X) &&
        (other.Min.Y >= Min.Y) && (other.Max.Y < Max.Y);
    }

    [[nodiscard]] constexpr  bool Overlaps(const FixedAABB& other) const
    {
        return Min.X < other.Max.X && Max.X > other.Min.X &&
        Min.Y < other.Max.Y && Max.Y > other.Min.Y;
    }
};