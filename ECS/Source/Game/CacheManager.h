#pragma once

#include "../Physics/Collision/CollisionCache.h"
#include "../Physics/PhysicsSettings.h"
#include "../ECS/ECSSettings.h"

class CacheManager
{
public:
    CacheManager() : collisionCache(MaxRollBackFrames * PhysicsIterations) { }

    inline CollisionCache* GetCollisionCache()
    {
        return &collisionCache;
    }

private:
    CollisionCache collisionCache;
};