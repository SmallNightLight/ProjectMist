#pragma once

class CameraSystem : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Camera>());
        return signature;
    }

    void Apply()
    {
        auto cameraCollection = EcsManager.GetComponentCollection<Camera>();

        for (const Entity& entity : Entities)
        {
            Camera& camera = cameraCollection->GetComponent(entity);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(camera.Left.ToFloating<float>(), camera.Right.ToFloating<float>(), camera.Bottom.ToFloating<float>(), camera.Top.ToFloating<float>(), -1.0f, 1.0f); //2D, so near/far planes are -1, 1
            glMatrixMode(GL_MODELVIEW);
        }
    }
};