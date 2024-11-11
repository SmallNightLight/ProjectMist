#pragma once

#include "GameSettings.h"
#include "../ECS/ECS.h"

#include <array>

class World
{


public:
    World() : currentFrame(0), lastLayerIndex(0)
    {
        SetupLayers();
    }



    void UpdateLayer()
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

private:
    void SetupLayers()
    {
        for(short frame = 0; frame < MaxRollBackFrames; frame++)
        {
            layers[frame].Setup();
        }
    }


private:
    int currentFrame;
    int lastLayerIndex;
    std::array<Layer, MaxRollBackFrames> layers;
};
