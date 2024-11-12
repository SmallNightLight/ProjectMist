#pragma once

#include "GameSettings.h"
#include "../ECS/ECS.h"

#include <array>

class World
{


public:
    World() : currentFrame(0), lastLayerIndex(0)
    {

    }

    template<typename T>
    void RegisterComponent()
    {
        for(short frame = 0; frame < MaxRollBackFrames; frame++)
        {
            layers[frame].RegisterComponent<T>();
        }
    }

    template<typename T>
    std::array<T, MaxRollBackFrames> RegisterSystem()
    {
        std::array<T, MaxRollBackFrames> systems;

        for(short frame = 0; frame < MaxRollBackFrames; frame++)
        {
            systems[frame] = layers[frame].RegisterSystem<T>(this);
        }

        return systems;
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
    std::array<Layer, MaxRollBackFrames> layers;
};
