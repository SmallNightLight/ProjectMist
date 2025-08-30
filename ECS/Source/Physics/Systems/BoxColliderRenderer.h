#pragma once

#include "../../ECS/ECS.h"

class BoxColliderRenderer
{
public:
    using RequiredComponents = ComponentList<Transform, BoxCollider, ColliderRenderData>;

    explicit BoxColliderRenderer(PhysicsComponentManager& componentManager)
    {
        transformCollection = componentManager.GetComponentCollection<Transform>();
        transformMetaCollection = componentManager.GetComponentCollection<TransformMeta>();
        boxColliderCollection = componentManager.GetComponentCollection<BoxCollider>();
        colliderRenderDataCollection = componentManager.GetComponentCollection<ColliderRenderData>();

        Entities.Initialize();
    }

    void Render() const
    {
        for (const Entity& entity : Entities)
        {
            Transform& transform = transformCollection->GetComponent(entity);
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            //Draw filled rectangle
            glColor3ub(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);

            Vector2Span vertices = boxCollider.GetTransformedVertices(transform);

            glBegin(GL_QUADS);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();

            //Draw white outline
            glColor3ub(255, 255, 255);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();
        }
    }

    void RenderDebugOverlay() const
    {
        for (const Entity& entity : Entities)
        {
            if (!transformMetaCollection->HasComponent(entity)) continue;

            Transform& transform = transformCollection->GetComponent(entity);
            TransformMeta& transformMeta = transformMetaCollection->GetComponent(entity);
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);

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

            AABB boundingBox = boxCollider.GetAABB(transform, transformMeta);
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
    ComponentCollection<BoxCollider>* boxColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;

public:
    EntitySet<MAXENTITIES> Entities;
};