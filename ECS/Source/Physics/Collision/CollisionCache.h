#pragma once

#include "CollisionCheckInfo.h"
#include "CollisionResponseInfo.h"
#include "CollisionPairData.h"
#include "CollisionTable.h"
#include "CollisionTable2.h"
#include "CollisionResponseTable.h"
#include "CollisionResponseTable2.h"
#include "../../ECS/ECSSettings.h"

#include <vector>

class CollisionCache
{
public:
    explicit CollisionCache(FrameNumber depth, FrameNumber startFrame = 0) : frameDepth(depth), oldestFrame(startFrame), startIndex(0), frameCount(0)
    {
        collisionPairData.resize(frameDepth);
        collisionData.resize(frameDepth);
    }

    inline void CacheCollision(FrameNumber frame, const CollisionCheckInfo& check, const CollisionResponseInfo& responseInfo)
    {
        collisionData[GetIndex(frame)].CacheCollision(check, responseInfo);
    }

    inline void CacheCollisionPair(FrameNumber frame, const CollisionPairData& collisionPair)
    {
        collisionPairData[GetIndex(frame)].CacheCollisionPair(collisionPair, true);
    }

    inline void CacheNonCollision(FrameNumber frame, const CollisionPairData& collisionPair)
    {
        collisionPairData[GetIndex(frame)].CacheCollisionPair(collisionPair, false);
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

    bool TryGetPairData(FrameNumber frame, const CollisionPairData& pairData, bool& outCollision) const
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldest frame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        return collisionPairData[GetIndex(frame)].TryGetCollisionPair(pairData, outCollision);
    }

    bool TryGetCollisionData(FrameNumber frame, const CollisionCheckInfo& check, CollisionResponseInfo& outResponseInfo) const
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldestFrame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        uint32_t index = GetIndex(frame);

        return collisionData[index].TryGetCollision(check, outResponseInfo);
    }

private:
    constexpr inline uint32_t GetIndex(FrameNumber frame) const
    {
        return (startIndex + (frame - oldestFrame)) % frameDepth;
    }

private:
    //Can use non-deterministic map as it not used in iteration
    std::vector<CollisionTable2> collisionPairData;
    std::vector<CollisionResponseTable> collisionData; //Used to get the exact collision response data

    //CollisionResponseTable uses external hash table which gives 3.4ms (better)
    //CollisionResponseTable2 uses custom hash table which is 3.7ms

    FrameNumber frameDepth;

    FrameNumber oldestFrame;        //Oldest frame where the input it still saved
    FrameNumber startIndex;         //Index of the oldest frame in the inputs, since it is circular
    FrameNumber frameCount;         //Number of frames currently stored in the buffer
};