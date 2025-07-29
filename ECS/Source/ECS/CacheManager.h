#pragma once

#include "../Physics/Collision/CollisionCache.h"
#include "ECSSettings.h"

class CacheManager
{
public:
    CacheManager() { }

public:
    CollisionCache<MaxRollBackFrames> CollisionCache;
};