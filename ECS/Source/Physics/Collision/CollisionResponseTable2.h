#pragma once

#include "CollisionCheckInfo.h"
#include "CollisionResponseInfo.h"

#include <vector>
#include <cassert>
#include <algorithm>

struct Entry
{
    CollisionCheckInfo Key;
    CollisionResponseInfo Value;
    bool Filled;
};

class CollisionResponseTable2
{
public:
    inline CollisionResponseTable2() : Capacity(StartCapacity), Mask(StartCapacity - 1), MaxLimit(StartCapacity / 2), MinLimit(StartCapacity / 4), EntryCount(0)
    {
        entries.resize(Capacity);
        Reset();
    }

    inline void CacheCollision(const CollisionCheckInfo& check, const CollisionResponseInfo& responseInfo)
    {
        if (EntryCount > MaxLimit)
        {
            UpdateCapacity(Capacity * 2);
        }

        for (uint32_t i = 0; i < Capacity; ++i)
        {
            Entry& entry = entries[(check.Hash + i) & Mask];

            assert((!entry.Filled || entry.Key != check) && "Already added collision to map. This case is not handled to improve performance");

            if (!entry.Filled)
            {
                entry.Key = check;
                entry.Value = responseInfo;
                entry.Filled = true;

                ++EntryCount;
                return;
            }
        }

        assert(false && "CollisionResponseTable overflow");
    }

    inline bool TryGetCollision(const CollisionCheckInfo& check, CollisionResponseInfo& outResponseInfo) const
    {
        for (uint32_t i = 0; i < Capacity; ++i)
        {
            const Entry& entry = entries[(check.Hash + i) & Mask];

            if (!entry.Filled) return false;

            if (entry.Key == check)
            {
                outResponseInfo = entry.Value;
                return true;
            }
        }

        return false;
    }

    inline void UpdateCapacity(uint32_t newCapacity)
    {
        CalculateCapacity(newCapacity);
        Rehash();
    }

    inline void CalculateCapacity(uint32_t newCapacity)
    {
        Capacity = newCapacity;
        Mask = Capacity - 1;
        MaxLimit = Capacity / 2;
        MinLimit = Capacity / 8;
    }

    inline void Rehash()
    {
        std::vector<Entry> newEntries(Capacity);

        for (const Entry& entry : entries)
        {
            if (!entry.Filled) continue;

            for (uint32_t i = 0; i < Capacity; ++i)
            {
                Entry& newEntry = newEntries[(entry.Key.Hash + i) & Mask];
                if (!newEntry.Filled)
                {
                    newEntry = entry;
                    break;
                }
            }
        }

        entries = std::move(newEntries);
    }

    inline void Reset()
    {
        if (Capacity > StartCapacity && EntryCount < MinLimit)
        {
            CalculateCapacity(Capacity / 2);
            entries = std::vector<Entry>(Capacity);
        }
        else
        {
            std::fill(entries.begin(), entries.end(), Entry());
        }

        EntryCount = 0;
    }

private:
    static constexpr uint32_t StartCapacity = 256; //Needs to be power of 2 and > 0

    uint32_t Capacity;
    uint32_t Mask;
    uint32_t MaxLimit;
    uint32_t MinLimit;
    uint32_t EntryCount;

    std::vector<Entry> entries;
};