#pragma once

#include "CollisionCheckInfo.h"
#include "ContactPair.h"
#include "CollisionResponseTable.h"
#include "CollisionResponseTable2.h"
#include "CollisionResponseTable3.h"
#include "../Cache/TransformCache.h"
#include "../../ECS/ECSSettings.h"

#include <vector>

class CollisionCache
{
public:
    explicit CollisionCache(FrameNumber depth, FrameNumber startFrame = 0) : frameDepth(depth), oldestFrame(startFrame), startIndex(0), frameCount(0)
    {
        transformData.resize(frameDepth);
        collisionPairData.resize(frameDepth);
        collisionData.resize(frameDepth);

        //Initialize caches
        for (FrameNumber i = 0; i < frameDepth; ++i)
        {
            transformData[i].Initialize();
            collisionPairData[i].Initialize();
            collisionData[i].Reset(); //? todo
        }
    }

    inline void CacheTransformCollection(FrameNumber frame, ComponentCollection<Transform>* transformCollection)
    {
        transformData[GetIndex(frame)].Cache(transformCollection);
    }

    inline void CacheCollision(FrameNumber frame, const CollisionCheckInfo& check, const ContactPair& responseInfo)
    {
        collisionData[GetIndex(frame)].CacheCollision(check, responseInfo);
    }

    inline void CacheCollisionPair(FrameNumber frame, EntityPair entityPair)
    {
        collisionPairData[GetIndex(frame)].Cache(entityPair);
    }

    bool UpdateFrame(FrameNumber frame)
    {
        if (frame < oldestFrame) return false;

        if (frameCount < frameDepth)
        {
            FrameNumber newFrameCount = std::min(frame - oldestFrame + 1, frameDepth); //ToDO: Check if input collection can also have this

            if (newFrameCount > frameCount)
            {
                frameCount = newFrameCount;
                return false;
            }

            return true;
        }

        if (frame >= oldestFrame + frameDepth)
        {
            //Calculate how many frames the buffer is advancing
            uint32_t framesAdvanced = frame - (oldestFrame + frameDepth - 1);

            for (int i = 0; i < framesAdvanced; ++i)
            {
                //Clear cache
                uint32_t clearIndex = (startIndex + i) % frameDepth;
                transformData[clearIndex].Reset();
                collisionPairData[clearIndex].Reset();
                collisionData[clearIndex].Reset();
            }

            //Update the oldestFrame and adjust startIndex accordingly
            oldestFrame = frame - frameDepth + 1;
            startIndex = (startIndex + framesAdvanced) % frameDepth;

            return false;
        }

        return true;
    }

    bool TryGetTransform(FrameNumber frame, Entity entity, Transform& transform)
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldest frame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        return transformData[GetIndex(frame)].TryGetTransform(entity, transform);
    }

    bool TryGetPairData(FrameNumber frame, EntityPair entityPair)
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldest frame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        return collisionPairData[GetIndex(frame)].HasKey(entityPair);
    }

    bool TryGetCollisionData(FrameNumber frame, const CollisionCheckInfo& check, ContactPair& outResponseInfo) const
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldestFrame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        return collisionData[GetIndex(frame)].TryGetCollision(check, outResponseInfo);
    }

    inline constexpr void ResetIndex(FrameNumber frame) noexcept
    {
        collisionPairData[GetIndex(frame)].ResetIndex();
    }

private:
    constexpr inline uint32_t GetIndex(FrameNumber frame) const
    {
        return (startIndex + (frame - oldestFrame)) % frameDepth;
    }

private:
    //Can use non-deterministic map as it not used in iteration
    std::vector<TransformCache> transformData;
    std::vector<CollisionPairCache> collisionPairData;
    std::vector<CollisionResponseTable> collisionData; //Used to get the exact collision response data

public:
    //std::vector<std::vector<CollisionResponseTable3>> cV;
private:

    //CollisionResponseTable uses external hash table which gives 3.4ms (better)
    //CollisionResponseTable2 uses custom hash table which is 3.7ms

    FrameNumber frameDepth;

    FrameNumber oldestFrame;        //Oldest frame where the input it still saved
    FrameNumber startIndex;         //Index of the oldest frame in the inputs, since it is circular
    FrameNumber frameCount;         //Number of frames currently stored in the buffer
};