#pragma once

#include "CollisionCheckInfo.h"
#include "CollisionResponseInfo.h"
#include "../../ECS/ECSSettings.h"

#include <vector>
#include <unordered_map>

class CollisionCache
{
public:
    CollisionCache(FrameNumber depth, FrameNumber startFrame = 0) : frameDepth(depth), oldestFrame(startFrame), startIndex(0), frameCount(0)
    {
        collisionData.resize(frameDepth);
    }

    void Cache(FrameNumber frame, const CollisionCheckInfo& check, const CollisionResponseInfo& responseInfo) //TODO: make sure that entity 1 < 2 in collision detection
    {
        if (frame < oldestFrame)
        {
            assert(false && "Cannot cache data for an old frame");
            return;
        }

        if (frameCount < frameDepth)
        {
            frameCount = std::min(frame - oldestFrame + 1, frameDepth); //ToDO: Check if input collection can also have this
        }
        else if (frame >= oldestFrame + frameDepth)
        {
            //Calculate how many frames the buffer is advancing
            uint32_t framesAdvanced = frame - (oldestFrame + frameDepth - 1);

            for (int i = 0; i < framesAdvanced; ++i)
            {
                //Clear cache
                uint32_t clearIndex = (startIndex + i) % frameDepth;
                collisionData[clearIndex] = std::unordered_map<CollisionCheckInfo, CollisionResponseInfo, CollisionCheckInfoHash>();
            }

            //Update the oldestFrame and adjust startIndex accordingly
            oldestFrame = frame - frameDepth + 1;
            startIndex = (startIndex + framesAdvanced) % frameDepth;
        }

        collisionData[GetIndex(frame)][check] = responseInfo;
    }

    bool TryGetCollisionData(FrameNumber frame, const CollisionCheckInfo& check, CollisionResponseInfo& outResponseInfo)
    {
        if (frame < oldestFrame || frame >= oldestFrame + frameDepth) return false;

        uint32_t index = GetIndex(frame);
        auto it = collisionData[index].find(check);
        if (it != collisionData[index].end())
        {
            outResponseInfo = it->second;
            return true;
        }
        return false;
    }

private:
    uint32_t GetIndex(FrameNumber frame) const
    {
        return (startIndex + (frame - oldestFrame)) % frameDepth;
    }

private:
    //Can use non-deterministic map as it not used in iteration
    std::vector<std::unordered_map<CollisionCheckInfo, CollisionResponseInfo, CollisionCheckInfoHash>> collisionData;

    FrameNumber frameDepth;

    FrameNumber oldestFrame;        //Oldest frame where the input it still saved
    FrameNumber startIndex;         //Index of the oldest frame in the inputs, since it is circular
    FrameNumber frameCount;         //Number of frames currently stored in the buffer
};