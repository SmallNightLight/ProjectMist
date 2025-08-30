#pragma once

#include "../../ECS/ECS.h"

class CircleColliderRenderer
{
public:
    using RequiredComponents = ComponentList<Transform, CircleCollider, ColliderRenderData>;

    explicit CircleColliderRenderer(PhysicsComponentManager& componentManager)
    {
        transformCollection = componentManager.GetComponentCollection<Transform>();
        transformMetaCollection = componentManager.GetComponentCollection<TransformMeta>();
        circleColliderCollection = componentManager.GetComponentCollection<CircleCollider>();
        colliderRenderDataCollection = componentManager.GetComponentCollection<ColliderRenderData>();

        Entities.Initialize();
    }

    void Render() const
    {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        for (const Entity& entity : Entities)
        {
            Transform& transform = transformCollection->GetComponent(entity);
            CircleCollider& circleCollider = circleColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            auto x = transform.Base.Position.X.ToFloating<float>();
            auto y = transform.Base.Position.Y.ToFloating<float>();
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

            v1 = transform.TransformVector(v1);
            v2 = transform.TransformVector(v2);

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
            if (!transformMetaCollection->HasComponent(entity)) continue;

            Transform& transform = transformCollection->GetComponent(entity);
            TransformMeta& transformMeta = transformMetaCollection->GetComponent(entity);
            CircleCollider& circleCollider = circleColliderCollection->GetComponent(entity);

            if (transformMeta.Active)
            {
                glColor3ub(128, 128, 128);
            }
            else
            {
                glColor3ub(0, 255, 0);
            }

            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);

            AABB boundingBox = circleCollider.GetAABB(transform, transformMeta);
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());

            glEnd();
        }
    }

private:
    ComponentCollection<Transform>* transformCollection;
    ComponentCollection<TransformMeta>* transformMetaCollection;        //Only for debug todo: remove reference in release build
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;

public:
    EntitySet<MAXENTITIES> Entities;
};