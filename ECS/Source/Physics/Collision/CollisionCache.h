#pragma once

#include "CollisionInfo.h"
#include "CollisionCheckInfo.h"
#include "CollisionResponseInfo.h"
#include "../../ECS/ECSSettings.h"

#include <array>
#include <unordered_map>

template<unsigned int FrameDepth>
class CollisionCache
{
public:
    CollisionCache(FrameNumber startFrame = 0) : oldestFrame(startFrame), startIndex(0), frameCount(0) { }

    void Cache(FrameNumber frame, const CollisionInfo& collisionInfo, const CollisionResponseInfo& responseInfo) //TODO: make sure that entity 1 < 2 in collision detection
    {
        if (frame < oldestFrame)
        {
            assert(false && "Cannot cache data for an old frame");
            return;
        }

        if (frameCount < FrameDepth)
        {
            frameCount = std::min(frame - oldestFrame + 1, FrameDepth); //ToDO: Check if input collection can also have this
        }
        else if (frame >= oldestFrame + FrameDepth)
        {
            //Calculate how many frames the buffer is advancing
            uint32_t framesAdvanced = frame - (oldestFrame + FrameDepth - 1);

            for (int i = 0; i < framesAdvanced; ++i)
            {
                //Clear cache
                uint32_t clearIndex = (startIndex + i) % FrameDepth;
                collisionData[clearIndex].clear();
            }

            //Update the oldestFrame and adjust startIndex accordingly
            oldestFrame = frame - FrameDepth + 1;
            startIndex = (startIndex + framesAdvanced) % FrameDepth;
        }

        collisionData[GetIndex(frame)][collisionInfo] = responseInfo;
    }

    bool TryGetCollisionData(FrameNumber frame, const CollisionInfo& collisionInfo, CollisionResponseInfo& outResponse)
    {
        if (frame < oldestFrame || frame >= oldestFrame + FrameDepth) return false;

        auto it = collisionData[GetIndex(frame)].find(collisionInfo);
        if (it != collisionData[GetIndex(frame)].end())
        {
            outResponse = it->second;
            return true;
        }
        return false;
    }

private:
    uint32_t GetIndex(FrameNumber frame) const
    {
        return (startIndex + (frame - oldestFrame)) % FrameDepth;
    }

private:
    //Can use non-deterministic map as it not used in iteration
    std::array<std::unordered_map<CollisionInfo, CollisionResponseInfo, CollisionInfoHash>, FrameDepth> collisionData;

    FrameNumber oldestFrame;        //Oldest frame where the input it still saved
    FrameNumber startIndex;         //Index of the oldest frame in the inputs, since it is circular
    FrameNumber frameCount;         //Number of frames currently stored in the buffer
};