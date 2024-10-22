#pragma once

class CircleColliderRenderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<ColliderTransform>());
        signature.set(EcsManager.GetComponentType<CircleCollider>());
        signature.set(EcsManager.GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render()
    {
        auto colliderTransformCollection = EcsManager.GetComponentCollection<ColliderTransform>();
        auto circleColliderCollection = EcsManager.GetComponentCollection<CircleCollider>();
        auto colliderRenderDataCollection = EcsManager.GetComponentCollection<ColliderRenderData>();

        glEnable(GL_POINT_SMOOTH);
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            CircleCollider& circleCollider = circleColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);

            auto x = transform.Position.X.ToFloating<float>();
            auto y = transform.Position.Y.ToFloating<float>();
            auto radius = circleCollider.Radius.ToFloating<float>();

            glPointSize(radius);
            glBegin(GL_POINTS);
            glVertex2f(x, y);
            glEnd();
        }
        glDisable(GL_POINT_SMOOTH);
    }
};