#pragma once

template<class T>
struct IntVector2
{
    T X;
    T Y;

    constexpr inline IntVector2() : X(T(0)), Y(T(0)) { }
    constexpr inline IntVector2(const T& x, const T& y) : X(x), Y(y) { }
};