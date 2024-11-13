#pragma once

#include "World.h"
#include "GameSettings.h"
#include "../ECS/ECS.h"

#include <array>

template <typename T>
struct SystemCollection
{

};

class WorldManager
{
public:
    WorldManager() : currentFrame(0), lastLayerIndex(0), worldCount(0)
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
    }

    //Rollback the amount of specified frames from the current frame
    /*void RollbackFrames(int frames)
    {
        assert(frames < MaxRollBackFrames);

        RollbackTo(currentFrame - frames);
    }

    //Rollback to the given frame
    void RollbackTo(int frame)
    {

    }*/

private:
    int currentFrame;
    int lastLayerIndex;
    int worldCount;
    std::array<Layer, MaxRollBackFrames> layers;
    std::array<std::array<std::shared_ptr<World>, MaxRollBackFrames>, MAXWORLDS> worlds { };

};