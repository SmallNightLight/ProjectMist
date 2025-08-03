#pragma once

#include "CollisionCheckInfo.h"
#include "CollisionResponseInfo.h"
#include "../../ECS/ECSSettings.h"

#include <vector>
#include <unordered_map>

#include "CollisionPairData.h"

class CollisionCache
{
public:
    CollisionCache(FrameNumber depth, FrameNumber startFrame = 0) : frameDepth(depth), oldestFrame(startFrame), startIndex(0), frameCount(0)
    {
        collisionPairData.resize(frameDepth);
        collisionData.resize(frameDepth);
    }

    constexpr inline void CacheCollision(FrameNumber frame, const CollisionCheckInfo& check, const CollisionResponseInfo& responseInfo) //TODO: make sure that entity 1 < 2 in collision detection
    {
        collisionData[GetIndex(frame)][check] = responseInfo;
    }

    constexpr inline void CacheCollisionPair(FrameNumber frame, const CollisionPairData& collisionPair)
    {
        collisionPairData[GetIndex(frame)][collisionPair] = true;
    }

    constexpr inline void CacheNonCollision(FrameNumber frame, const CollisionPairData& collisionPair)
    {
        collisionPairData[GetIndex(frame)][collisionPair] = false;
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
                collisionPairData[clearIndex] = std::unordered_map<CollisionPairData, bool, CollisionPairDataHash>();
                collisionData[clearIndex] = std::unordered_map<CollisionCheckInfo, CollisionResponseInfo, CollisionCheckInfoHash>();
            }

            //Update the oldestFrame and adjust startIndex accordingly
            oldestFrame = frame - frameDepth + 1;
            startIndex = (startIndex + framesAdvanced) % frameDepth;

            return false;
        }

        return true;
    }

    bool TryGetPairData(FrameNumber frame, const CollisionPairData& pairData, bool& outCollision)
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldestFrame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        uint32_t index = GetIndex(frame);
        auto collisionPair = collisionPairData[index].find(pairData);

        if (collisionPair == collisionPairData[index].end()) return false;

        outCollision = collisionPair->second;
        return true;
    }

    bool TryGetCollisionData(FrameNumber frame, const CollisionCheckInfo& check, CollisionResponseInfo& outResponseInfo)
    {
        assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldestFrame");
        assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

        uint32_t index = GetIndex(frame);

        auto collisionResponseData = collisionData[index].find(check);
        if (collisionResponseData != collisionData[index].end())
        {
            outResponseInfo = collisionResponseData->second;
            return true;
        }

        return false;
    }

private:
    constexpr inline uint32_t GetIndex(FrameNumber frame) const
    {
        return (startIndex + (frame - oldestFrame)) % frameDepth;
    }

private:
    //Can use non-deterministic map as it not used in iteration
    std::vector<std::unordered_map<CollisionPairData, bool, CollisionPairDataHash>> collisionPairData; //Used to determine if a collision exist
    std::vector<std::unordered_map<CollisionCheckInfo, CollisionResponseInfo, CollisionCheckInfoHash>> collisionData; //Used to get the exact collision response data

    FrameNumber frameDepth;

    FrameNumber oldestFrame;        //Oldest frame where the input it still saved
    FrameNumber startIndex;         //Index of the oldest frame in the inputs, since it is circular
    FrameNumber frameCount;         //Number of frames currently stored in the buffer
};