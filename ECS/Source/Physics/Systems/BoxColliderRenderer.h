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

            //Draw filled rectangle
            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);

            std::vector<Vector2> vertices = CollisionDetection::GetTransformedVertices(transform, boxCollider);

            glBegin(GL_QUADS);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();

            //Draw white outline
            if (colliderRenderData.Outline)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                glLineWidth(2.0f);
                glBegin(GL_LINE_LOOP);
                for (const auto& vertex : vertices)
                {
                    glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
                }
                glEnd();
            }



            //TEST
            //Draw filled circle
            glColor3f(0.1f, 0.1f, 0.1f);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(colliderRenderData.p.X.ToFloating<float>(), colliderRenderData.p.Y.ToFloating<float>()); // Center point
            for (int i = 0; i <= 20; ++i)
            {
                float theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(20);
                float dx = 0.5f * cosf(theta);
                float dy = 0.5f * sinf(theta);
                glVertex2f(colliderRenderData.p.X.ToFloating<float>() + dx, colliderRenderData.p.Y.ToFloating<float>() + dy);
            }
            glEnd();
        }
    }
};