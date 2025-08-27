#pragma once

#include "ContactPair.h"
#include "../../ECS/ECSSettings.h"

#include <vector>

class CollisionCache
{
public:
    explicit CollisionCache(FrameNumber depth, FrameNumber startFrame = 0) : frameDepth(depth), oldestFrame(startFrame), startIndex(0), frameCount(0), currentIndex(0)
    {
        transformCache.resize(frameDepth);
        rigidBodyDataCache.resize(frameDepth);
        collisionPairData.resize(frameDepth);
        collisionData.resize(frameDepth);

        //Initialize caches
        for (FrameNumber i = 0; i < frameDepth; ++i)
        {
            transformCache[i].Initialize();
            rigidBodyDataCache[i].Initialize();
            collisionPairData[i].Initialize();
            collisionData[i].Initialize();
        }
    }

    ///Function updates circular buffer and sets the current frame index.
    ///Call this function before calling the other caching functions
    bool UpdateFrame(FrameNumber frame)
    {
        if (frame < oldestFrame) return false;

        if (frameCount < frameDepth)
        {
            FrameNumber newFrameCount = std::min(frame - oldestFrame + 1, frameDepth); //ToDO: Check if input collection can also have this
            currentIndex = GetIndex(frame);

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
                transformCache[clearIndex].Reset();
                rigidBodyDataCache[clearIndex].Reset();
                collisionPairData[clearIndex].Reset();
                collisionData[clearIndex].Reset();
            }

            //Update the oldestFrame and adjust startIndex accordingly
            oldestFrame = frame - frameDepth + 1;
            startIndex = (startIndex + framesAdvanced) % frameDepth;
            currentIndex = GetIndex(frame);

            assert(frame >= oldestFrame && "Cannot get pair data with frame that is older than the oldest frame");
            assert(frame < oldestFrame + frameDepth && "Cannot get pair data with frame that is in the future");

            return false;
        }

        currentIndex = GetIndex(frame);
        return true;
    }

    //Caching

    inline void CacheTransformCollection(ComponentCollection<Transform>* transformCollection)
    {
        transformCache[currentIndex].Cache(transformCollection);
    }

    inline void CacheRigidBodyDataCollection(ComponentCollection<RigidBodyData>* rigidBodyDataCollection)
    {
        rigidBodyDataCache[currentIndex].Cache(rigidBodyDataCollection);
    }

    inline void CacheCollisionPair(EntityPair entityPair)
    {
        collisionPairData[currentIndex].Cache(entityPair);
    }

    inline void CacheCollision(const ContactPair& contactPair)
    {
        collisionData[currentIndex].Cache(contactPair);
    }

    inline bool TryGetTransform(Entity entity, Transform& transform)
    {
        return transformCache[currentIndex].TryGetTransform(entity, transform);
    }

    inline bool TryGetRigidBodyData(Entity entity, RigidBodyData& result)
    {
        return rigidBodyDataCache[currentIndex].TryGetTransform(entity, result);
    }

    inline constexpr bool AdvancePairCache(EntityPair entityPair) noexcept
    {
        return collisionPairData[currentIndex].AdvanceCache(entityPair);
    }

    inline constexpr bool AdvanceCollisionCache(EntityPair entityPair, ContactPair& result) noexcept
    {
        return collisionData[currentIndex].AdvanceCache(entityPair, result);
    }

    inline constexpr void Flip() noexcept
    {
        collisionPairData[currentIndex].Flip();
        collisionData[currentIndex].Flip();
    }

private:
    constexpr inline uint32_t GetIndex(FrameNumber frame) const
    {
        return (startIndex + (frame - oldestFrame)) % frameDepth;
    }

private:
    //Can use non-deterministic map as it not used in iteration
    std::vector<TransformCache> transformCache;
    std::vector<RigidBodyDataCache> rigidBodyDataCache;
    std::vector<CollisionPairCache> collisionPairData;
    std::vector<CollisionResultCache> collisionData; //Used to get the exact collision response data

private:
    FrameNumber frameDepth;

    FrameNumber oldestFrame;        //Oldest frame where the input it still saved
    FrameNumber startIndex;         //Index of the oldest frame in the inputs, since it is circular
    FrameNumber frameCount;         //Number of frames currently stored in the buffer
    FrameNumber currentIndex;
};