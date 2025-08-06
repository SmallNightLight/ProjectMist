#pragma once

#include <array>

template <typename T>
struct Span
{
    T* data = nullptr;
    uint32_t size = 0;

    inline constexpr Span() = default;

    inline constexpr Span(T* ptr, uint32_t count) : data(ptr), size(count) { }

    template <uint32_t N>
    explicit inline constexpr Span(const std::array<T, N>& array) : data(array.data()), size(N) { }

    template <uint32_t N>
    explicit inline constexpr Span(const std::array<T, N>& array, uint32_t count) : data(array.data()), size(count) { }

    const T* begin() const { return data; }
    const T* end() const { return data + size; }

    T& operator[](uint32_t index) { return data[index]; }

    bool empty() const { return size == 0; }
};