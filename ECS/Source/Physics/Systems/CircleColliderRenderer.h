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

            //Draw line for circle rotation
            Vector2 v1 = Vector2::Zero();
            Vector2 v2 = Vector2(circleCollider.Radius, Fixed16_16(0));

            v1 = transform.Transform(v1);
            v2 = transform.Transform(v2);

            glLineWidth(2.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex2f(v1.X.ToFloating<float>(), v1.Y.ToFloating<float>());
            glVertex2f(v2.X.ToFloating<float>(), v2.Y.ToFloating<float>());
            glEnd();

        }

        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }

    static void RenderContactPoints(std::vector<CollisionInfo>& collisions)
    {
        float size = 0.4f;

        for(CollisionInfo collision : collisions)
        {
            if (collision.ContactCount > 0)
            {
                glLineWidth(2.0f);
                glColor3f(0.5, 0.5f, 0.5f);
                glBegin(GL_LINES);
                glVertex2f(collision.Contact1.X.ToFloating<float>() - size, collision.Contact1.Y.ToFloating<float>() - size);
                glVertex2f(collision.Contact1.X.ToFloating<float>() + size, collision.Contact1.Y.ToFloating<float>() + size);
                glVertex2f(collision.Contact1.X.ToFloating<float>() + size, collision.Contact1.Y.ToFloating<float>() - size);
                glVertex2f(collision.Contact1.X.ToFloating<float>() - size, collision.Contact1.Y.ToFloating<float>() + size);
                glEnd();

                if (collision.ContactCount > 1)
                {
                    glLineWidth(2.0f);
                    glBegin(GL_LINES);
                    glVertex2f(collision.Contact2.X.ToFloating<float>() - size, collision.Contact2.Y.ToFloating<float>() - size);
                    glVertex2f(collision.Contact2.X.ToFloating<float>() + size, collision.Contact2.Y.ToFloating<float>() + size);
                    glVertex2f(collision.Contact2.X.ToFloating<float>() + size, collision.Contact2.Y.ToFloating<float>() - size);
                    glVertex2f(collision.Contact2.X.ToFloating<float>() - size, collision.Contact2.Y.ToFloating<float>() + size);
                    glEnd();
                }
            }
        }
    }

    void RenderAABB()
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            CircleCollider& circleCollider = circleColliderCollection->GetComponent(entity);

            glColor3f(0.5f, 0.5f, 0.5f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);

            AABB boundingBox = transform.GetAABB(circleCollider.Radius);
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());

            glEnd();
        }
    }

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};