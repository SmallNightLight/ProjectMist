#pragma once

#include "Worlds/PhysicsWorld.h"
#include "../ECS/ECSSettings.h"
#include "../ECS/ECS.h"

class WorldManager
{
public:
    WorldManager() : LastConfirmedFrame(1), BaseLayer(PhysicsLayer()), ConfirmedLayer(PhysicsLayer()), BasePhysicsWorld(BaseLayer)
    {
        PhysicsWorld::RegisterLayer(ConfirmedLayer);
    }

    void NextFrame(FrameNumber confirmedFrame)
    {
        if (BasePhysicsWorld.GetCurrentFrame() == confirmedFrame)
        {
            //Save confirmed game state
            ConfirmedLayer.Overwrite(BaseLayer);
            LastConfirmedFrame = BasePhysicsWorld.GetCurrentFrame();
        }
    }

    //Rollback the amount of specified frames from the current frame
    int32_t Restore()
    {
        FrameNumber currentFrame = BasePhysicsWorld.GetCurrentFrame();

        if (currentFrame <= LastConfirmedFrame) return currentFrame - LastConfirmedFrame;

        //Restore last confirmed game state
        BaseLayer.Overwrite(ConfirmedLayer);
        BasePhysicsWorld.OverwriteFrame(LastConfirmedFrame);
        return currentFrame - LastConfirmedFrame;
    }

    void Reset()
    {
        ConfirmedLayer.Overwrite(BaseLayer);
        LastConfirmedFrame = BasePhysicsWorld.GetCurrentFrame();
    }

    PhysicsWorld& GetPhysicsWorld()
    {
        return BasePhysicsWorld;
    }

private:
    FrameNumber LastConfirmedFrame;
    PhysicsLayer BaseLayer;                    //Layer that gets updated
    PhysicsLayer ConfirmedLayer;               //Layer of the last confirmed frame, which is only used to save to and restore from
    PhysicsWorld BasePhysicsWorld;
};