#pragma once

#include "Worlds/PhysicsWorld.h"
#include "../ECS/ECSSettings.h"
#include "../ECS/ECS.h"

class WorldManager
{
public:
    WorldManager() :
        BaseLayer(PhysicsLayer()), ConfirmedLayer(PhysicsLayer()),
        BasePhysicsWorldData(1, 12), ConfirmedPhysicsWorldData(1, 0),
        BasePhysicsWorld(BaseLayer, BasePhysicsWorldData) { }

    void NextFrame(FrameNumber confirmedFrame)
    {
        if (BasePhysicsWorld.GetCurrentFrame() == confirmedFrame)
        {
            //Save confirmed game state
            ConfirmedLayer.Overwrite(BaseLayer);
            ConfirmedPhysicsWorldData = BasePhysicsWorldData;
        }
    }

    //Rollback the amount of specified frames from the current frame
    int32_t Restore()
    {
        FrameNumber currentFrame = BasePhysicsWorld.GetCurrentFrame();

        if (currentFrame <= ConfirmedPhysicsWorldData.CurrentFrame) return currentFrame - ConfirmedPhysicsWorldData.CurrentFrame;

        //Restore last confirmed game state
        BaseLayer.Overwrite(ConfirmedLayer);
        BasePhysicsWorldData = ConfirmedPhysicsWorldData; //todo check if same LastConfirmedFrame
        return currentFrame - ConfirmedPhysicsWorldData.CurrentFrame;
    }

    void Reset()
    {
        ConfirmedLayer.Overwrite(BaseLayer);
        ConfirmedPhysicsWorldData = BasePhysicsWorldData;
    }

    PhysicsWorld& GetPhysicsWorld()
    {
        return BasePhysicsWorld;
    }

private:
    PhysicsLayer BaseLayer;                    //Layer that gets updated
    PhysicsLayer ConfirmedLayer;               //Layer of the last confirmed frame, which is only used to save to and restore from
    PhysicsWorldData BasePhysicsWorldData;
    PhysicsWorldData ConfirmedPhysicsWorldData;
    PhysicsWorld BasePhysicsWorld;  //todo reorder
};