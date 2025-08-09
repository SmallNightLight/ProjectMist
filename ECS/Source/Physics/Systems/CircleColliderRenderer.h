#pragma once

#include "../../ECS/ECS.h"

class CircleColliderRenderer
{
public:
    explicit CircleColliderRenderer(PhysicsComponentManager& componentManager)
    {
        colliderTransformCollection = componentManager.GetComponentCollection<ColliderTransform>();
        circleColliderCollection = componentManager.GetComponentCollection<CircleCollider>();
        colliderRenderDataCollection = componentManager.GetComponentCollection<ColliderRenderData>();

        Entities.Initialize();
    }

    static constexpr PhysicsSignature GetSignature()
    {
        PhysicsSignature signature;
        signature.set(PhysicsComponentManager::GetComponentType<ColliderTransform>());
        signature.set(PhysicsComponentManager::GetComponentType<CircleCollider>());
        signature.set(PhysicsComponentManager::GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render() const
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
            auto radius = circleCollider.GetRadius().ToFloating<float>();
            int numSegments = 100;

            //Draw filled circle
            glColor3ub(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);
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
            glColor3ub(255, 255, 255);
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
            Vector2 v2 = Vector2(circleCollider.GetRadius(), Fixed16_16(0));

            v1 = transform.Transform(v1);
            v2 = transform.Transform(v2);

            glLineWidth(2.0f);
            glColor3ub(255, 255, 255);
            glBegin(GL_LINES);
            glVertex2f(v1.X.ToFloating<float>(), v1.Y.ToFloating<float>());
            glVertex2f(v2.X.ToFloating<float>(), v2.Y.ToFloating<float>());
            glEnd();
        }

        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }

    void RenderDebugOverlay() const
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);
            CircleCollider& circleCollider = circleColliderCollection->GetComponent(entity);

            if (colliderTransform.Active)
            {
                glColor3ub(128, 128, 128);
            }
            else
            {
                glColor3ub(0, 255, 0);
            }

            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);

            AABB boundingBox = colliderTransform.GetAABB(circleCollider.GetRadius());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());

            glEnd();
        }
    }

public:
    EntitySet<MAXENTITIES> Entities;

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};