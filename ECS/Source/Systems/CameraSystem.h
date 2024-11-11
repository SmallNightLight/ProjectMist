#pragma once

class CameraSystem : public System
{
public:
    explicit  CameraSystem(Layer* world) : System(world)
    {
        cameraCollection = layer->GetComponentCollection<Camera>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<Camera>());
        return signature;
    }

    void Apply()
    {
        for (const Entity& entity : Entities)
        {
            Camera& camera = cameraCollection->GetComponent(entity);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(camera.Left.ToFloating<float>(), camera.Right.ToFloating<float>(), camera.Bottom.ToFloating<float>(), camera.Top.ToFloating<float>(), -1.0f, 1.0f); //2D, so near/far planes are -1, 1
            glMatrixMode(GL_MODELVIEW);
        }
    }

private:
    ComponentCollection<Camera>* cameraCollection;
};