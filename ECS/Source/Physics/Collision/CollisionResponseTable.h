#pragma once

#include "CollisionCheckInfo.h"
#include "CollisionResponseInfo.h"

#include "unordered_dense.h"

class CollisionResponseTable
{
public:
    inline CollisionResponseTable()
    {
        collisions.reserve(64);
    }

    inline void CacheCollision(const CollisionCheckInfo& check, const CollisionResponseInfo& responseInfo)
    {
        collisions[check] = responseInfo;
    }

    inline bool TryGetCollision(const CollisionCheckInfo& check, CollisionResponseInfo& outResponseInfo) const
    {
        auto collisionResponseData = collisions.find(check);
        if (collisionResponseData != collisions.end())
        {
            outResponseInfo = collisionResponseData->second;
            return true;
        }

        return false;
    }

    inline void Reset()
    {
        collisions.clear();
    }

private:
    ankerl::unordered_dense::segmented_map<CollisionCheckInfo, CollisionResponseInfo, CollisionCheckInfoHash> collisions;
};