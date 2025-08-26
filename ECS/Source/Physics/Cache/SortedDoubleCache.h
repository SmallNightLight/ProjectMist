#pragma once

#include "../../ECS/ECS.h"

#include <array>
#include <cassert>
#include <type_traits>

template<typename KeyType, uint32_t Size>
class SortedDoubleCache
{
    static_assert(std::is_copy_assignable_v<KeyType>, "Key type must be copy assignable");
    static_assert(std::is_copy_constructible_v<KeyType>, "Key type must be copy constructible");
    static_assert(std::is_convertible_v<decltype(std::declval<const KeyType&>() < std::declval<const KeyType&>()), bool>, "KeyType must support operator< with KeyType");
    static_assert(std::is_convertible_v<decltype(std::declval<const KeyType&>() == std::declval<const KeyType&>()), bool>, "KeyType must support operator== with KeyType");

public:
    inline SortedDoubleCache() noexcept = default;

    ///Clears the cache completely
    inline constexpr void Initialize() noexcept
    {
        Reset();
    }

    ///Cache the key. Function needs to be called with sorted keys (with keyA1 < keyB1 || (keyA1 == keyB1 && keyA2 < keyB2)).
    ///From key with lsb to msb
    inline constexpr void Cache(const KeyType& key)
    {
        assert(count[flip] < Size && "Trying to cache key, but buffer is full");
        assert((count[flip] == 0 || GetFlip(count[flip] - 1) < key) && "Keys must be cached in sorted order");
        GetFlip(count[flip]++) = key;
    }

    ///Advances the cache to the key, expecting keys in order. Mirrors the keys to the secondary buffer if found.
    ///Returns if the key exists in the primary buffer
    inline constexpr bool AdvanceCache(KeyType key) noexcept
    {
        while (currentIndex < count[flop])
        {
            KeyType& currentKey = GetFlop(currentIndex);

            if (key < currentKey)
            {
                //key does not exist in cache
                return false;
            }

            if (currentKey == key)
            {
                //Mirror key into other buffer
                Cache(key);
                ++currentIndex;
                return true;
            }

            //Key in list is not anymore present
            ++currentIndex;
        }

        return false;
    }

    ///Clears the cache completely
    inline constexpr void Reset() noexcept
    {
        currentIndex = 0;
        count = {0, 0};
        flip = 0;
        flop = 1;
    }

    ///Flips the buffer, to access to the mirrored buffer
    inline constexpr void Flip() noexcept
    {
        flip ^= 1;
        flop = flip ^ 1;
        currentIndex = 0;
        count[flip] = 0;
    }

private:
    uint32_t currentIndex;
    std::array<uint32_t, 2> count;
    uint8_t flip;
    uint8_t flop;
    std::array<KeyType, 2 * Size> data;

    inline constexpr KeyType& GetFlip(uint32_t index) noexcept
    {
        return data[(index << 1) | flip];
    }

    inline constexpr KeyType& GetFlop(uint32_t index) noexcept
    {
        return data[(index << 1) | flop];
    }

    static_assert(std::is_trivially_default_constructible_v<KeyType>, "Type for sorted cache needs to be trivial");
};