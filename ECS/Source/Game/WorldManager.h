#pragma once

#include "World.h"
#include "GameSettings.h"
#include "../ECS/ECSSettings.h"
#include "../ECS/ECS.h"
#include "../ECS/CacheManager.h"

#include <array>

class WorldManager
{
public:
    WorldManager() : lastLayerIndex(0), rollbackCount(MaxRollBackFrames - 1), worldCount(0)
    {
        for(short layer = 0; layer < MaxRollBackFrames; ++layer)
        {
            layers[layer] = Layer(&cacheManager);
        }
    }

    template<typename T>
    WorldType AddWorld()
    {
        assert (worldCount < MAXWORLDS && "Not enough space for a new world - extend the buffer size");

        for(short layer = 0; layer < MaxRollBackFrames; ++layer)
        {
             worlds[worldCount][layer] = std::make_shared<T>(layers[layer]);
        }

        worldCount++;

        return worldCount - 1;
    }

    template<typename T>
    std::shared_ptr<T> GetWorld(WorldType worldType)
    {
        return std::static_pointer_cast<T>(worlds[worldType][lastLayerIndex]);
    }

    template<typename T>
    std::shared_ptr<T> GetPreviousWorld(WorldType worldType, uint32_t steps = 1)
    {
        int targetLayerIndex = (lastLayerIndex - steps + MaxRollBackFrames) % MaxRollBackFrames;
        return std::static_pointer_cast<T>(worlds[worldType][targetLayerIndex]);
    }

    Layer& GetCurrentLayer()
    {
        return layers[lastLayerIndex];
    }

    template<typename T>
    void NextFrame(WorldType worldType)
    {
        std::shared_ptr<T> currentWorld = GetWorld<T>(worldType);
        int currentLayer = lastLayerIndex;

        lastLayerIndex++;
        lastLayerIndex %= MaxRollBackFrames;

        std::shared_ptr<T> nextWorld = GetWorld<T>(worldType);

        //Overwrite the layer that is (MaxRollBackFrames) frames behind
        layers[lastLayerIndex].Overwrite(layers[currentLayer]);
        nextWorld->OverwriteFrame(currentWorld->GetCurrentFrame(), currentWorld->GetNumberGenerator());

        if (rollbackCount > 0) rollbackCount--;
    }

    //Rollback the amount of specified frames from the current frame
    bool Rollback(unsigned int frames)
    {
        if (rollbackCount + frames >= MaxRollBackFrames) return false;

        lastLayerIndex -= frames;
        lastLayerIndex = (lastLayerIndex % MaxRollBackFrames + MaxRollBackFrames) % MaxRollBackFrames; //Todo: lol
        rollbackCount += frames;

        return true;
    }

    void PreventFurtherRollback()
    {
        rollbackCount = MaxRollBackFrames - 1;
    }

    CacheManager* GetCacheManager()
    {
        return &cacheManager;
    }

private:
    int lastLayerIndex;
    int rollbackCount;
    int worldCount;
    std::array<Layer, MaxRollBackFrames> layers;
    std::array<std::array<std::shared_ptr<World>, MaxRollBackFrames>, MAXWORLDS> worlds { };

    CacheManager cacheManager;
};