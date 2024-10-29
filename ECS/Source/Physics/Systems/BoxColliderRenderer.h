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

            auto x = transform.Position.X.ToFloating<float>();
            auto y = transform.Position.Y.ToFloating<float>();
            auto width = boxCollider.Width.ToFloating<float>();
            auto height = boxCollider.Height.ToFloating<float>();

            //Draw filled rectangle
            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);
            glBegin(GL_QUADS);
            glVertex2f(x, y);                   //Bottom left corner
            glVertex2f(x + width, y);           //Bottom right corner
            glVertex2f(x + width, y + height);  //Top right corner
            glVertex2f(x, y + height);          //Top left corner
            glEnd();

            //Draw white outline
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(x, y);                   //Bottom left corner
            glVertex2f(x + width, y);           //Bottom right corner
            glVertex2f(x + width, y + height);  //Top right corner
            glVertex2f(x, y + height);          //Top left corner
            glEnd();
        }
    }
};