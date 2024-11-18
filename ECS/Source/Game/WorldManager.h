#pragma once

#include "World.h"
#include "GameSettings.h"
#include "../ECS/ECS.h"

#include <array>

class WorldManager
{
public: //TODO: MaxRollBackFrames ḿodf
    WorldManager() : currentFrame(0), lastLayerIndex(0), rollbackCount(MaxRollBackFrames - 1), worldCount(0) //TODO: ACTUALLY - 1? or 2
    {
        for(short layer = 0; layer < MaxRollBackFrames; ++layer)
        {
            layers[layer] = Layer();
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

    Layer& GetCurrentLayer()
    {
        return layers[lastLayerIndex];
    }

    void NextFrame()
    {
        int currentLayer = lastLayerIndex;
        lastLayerIndex++;
        lastLayerIndex %= MaxRollBackFrames;

        //Overwrite the layer that is (MaxRollBackFrames) frames behind
        layers[lastLayerIndex].Overwrite(layers[currentLayer]);

        if (rollbackCount > 0) rollbackCount--;
    }

    //Rollback the amount of specified frames from the current frame
    bool Rollback(unsigned int frames)
    {
        if (frames >= MaxRollBackFrames || rollbackCount + frames >= MaxRollBackFrames)
            return false;

        lastLayerIndex -= frames;
        lastLayerIndex = (lastLayerIndex % MaxRollBackFrames + MaxRollBackFrames) % MaxRollBackFrames;
        rollbackCount += frames;


        if (lastLayerIndex == -1)
        {
            int i = 0;
        }

        return true;
    }

    //Rollback to the given frame
    bool RollbackToFrame(int frame)
    {
        if (frame > currentFrame) return false;

        return Rollback(currentFrame - frame);
    }

private:
    int currentFrame;
    int lastLayerIndex;
    int rollbackCount;
    int worldCount;
    std::array<Layer, MaxRollBackFrames> layers;
    std::array<std::array<std::shared_ptr<World>, MaxRollBackFrames>, MAXWORLDS> worlds { };

};