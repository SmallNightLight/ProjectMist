#pragma once

#include "CollisionPairData.h"
#include "unordered_dense.h"

#include <array>
#include <cstdint>
#include <cstring>

class CollisionTable2
{
public:
    inline CollisionTable2()
    {
        data = std::array<uint8_t, Capacity>();
        backupCollisions.reserve(64);
    }

    inline bool CacheCollisionPair(const CollisionPairData& pairData, uint8_t collision)
    {
        uint8_t& flag = data[pairData.Hash & Mask];

        if (flag & InvalidBit)
        {
            backupCollisions[pairData] = collision;
            return false;
        }

        if (flag & UsedBit)
        {
            if ((flag & ResultBit) == collision) return true;

            //Collision between hashes
            flag |= InvalidBit;
            backupCollisions[pairData] = collision;

            return false;
        }

        flag |= UsedBit | ((0u - collision) & ResultBit);

        return true;
    }

    inline bool TryGetCollisionPair(const CollisionPairData& pairData, bool& collision) const
    {
        uint8_t flag = data[pairData.Hash & Mask];

        if (flag == 0) return false;

        if (flag & InvalidBit)
        {
            auto collisionResponseData = backupCollisions.find(pairData);
            if (collisionResponseData != backupCollisions.end())
            {
                collision = collisionResponseData->second;
                return true;
            }
        }

        collision = (flag & ResultBit) != 0;
        return true;
    }

    inline void Reset()
    {
        std::memset(data.data(), 0, sizeof(data));
        backupCollisions.clear();
    }

    constexpr static uint8_t UsedBit      = 1 << 0;
    constexpr static uint8_t InvalidBit   = 1 << 1;
    constexpr static uint8_t ResultBit    = 1 << 2;

private:
    static constexpr uint32_t Capacity = 4096;
    static constexpr uint32_t Mask = Capacity - 1;

    alignas(64) std::array<uint8_t, Capacity> data;

    ankerl::unordered_dense::map<CollisionPairData, bool, CollisionPairDataHash> backupCollisions;
};