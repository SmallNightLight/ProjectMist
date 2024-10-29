#pragma once

class RigidBody : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<ColliderTransform>());
        signature.set(EcsManager.GetComponentType<CircleCollider>()); //TODO
        return signature;
    }

    void Update()
    {

    }
};