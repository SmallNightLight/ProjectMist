#pragma once

class BoxColliderRenderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<ColliderTransform>());
        signature.set(EcsManager.GetComponentType<BoxCollider>());
        signature.set(EcsManager.GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render()
    {
        auto colliderTransformCollection = EcsManager.GetComponentCollection<ColliderTransform>();
        auto boxColliderCollection = EcsManager.GetComponentCollection<BoxCollider>();
        auto colliderRenderDataCollection = EcsManager.GetComponentCollection<ColliderRenderData>();

        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);

            auto x = transform.Position.X.ToFloating<float>();
            auto y = transform.Position.Y.ToFloating<float>();
            auto width = boxCollider.Width.ToFloating<float>();
            auto height = boxCollider.Height.ToFloating<float>();

            glBegin(GL_LINE_LOOP);
            glVertex2f(x, y);                   //Bottom left corner
            glVertex2f(x + width, y);           //Bottom right corner
            glVertex2f(x + width, y + height);  //Top right corner
            glVertex2f(x, y + height);          //Top left corner
            glEnd();
        }
    }
};
