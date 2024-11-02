#pragma once

class CircleColliderRenderer : public System
{
public:
    explicit CircleColliderRenderer(ECSWorld* world) : System(world)
    {
        colliderTransformCollection = World->GetComponentCollection<ColliderTransform>();
        circleColliderCollection = World->GetComponentCollection<CircleCollider>();
        colliderRenderDataCollection = World->GetComponentCollection<ColliderRenderData>();
    }

    [[nodiscard]] Signature GetSignature() const
    {
        Signature signature;
        signature.set(World->GetComponentType<ColliderTransform>());
        signature.set(World->GetComponentType<CircleCollider>());
        signature.set(World->GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render()
    {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            CircleCollider& circleCollider = circleColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            auto x = transform.Position.X.ToFloating<float>();
            auto y = transform.Position.Y.ToFloating<float>();
            auto radius = circleCollider.Radius.ToFloating<float>();
            int numSegments = 100;

            //Draw filled circle
            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y); // Center point
            for (int i = 0; i <= numSegments; ++i)
            {
                float theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(numSegments);
                float dx = radius * cosf(theta);
                float dy = radius * sinf(theta);
                glVertex2f(x + dx, y + dy);
            }
            glEnd();

            //Draw white outline
            if (colliderRenderData.Outline)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                glLineWidth(2.0f);
                glBegin(GL_LINE_LOOP);
                for (int i = 0; i < numSegments; ++i)
                {
                    float theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(numSegments);
                    float dx = radius * cosf(theta);
                    float dy = radius * sinf(theta);
                    glVertex2f(x + dx, y + dy);
                }
                glEnd();
            }
        }

        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};