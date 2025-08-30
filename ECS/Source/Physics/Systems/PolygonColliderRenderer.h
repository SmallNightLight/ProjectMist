#pragma once

#include "../../ECS/ECS.h"

class PolygonColliderRenderer
{
public:
    using RequiredComponents = ComponentList<Transform, PolygonCollider, ColliderRenderData>;

    explicit PolygonColliderRenderer(PhysicsComponentManager& componentManager)
    {
        transformCollection = componentManager.GetComponentCollection<Transform>();
        transformMetaCollection = componentManager.GetComponentCollection<TransformMeta>();
        polygonColliderCollection = componentManager.GetComponentCollection<PolygonCollider>();
        colliderRenderDataCollection = componentManager.GetComponentCollection<ColliderRenderData>();

        Entities.Initialize();
    }

    void Render() const
    {
        for (const Entity& entity : Entities)
        {
            Transform& transform = transformCollection->GetComponent(entity);
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            //Get transformed vertices
            Vector2Span vertices = polygonCollider.GetTransformedVertices(transform);

            //Draw filled polygon
            glColor3ub(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);
            glBegin(GL_POLYGON);
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

            //Draw point at the position (center)
            glBegin(GL_POINTS);
            glPointSize(10);
            glColor3ub(255, 255, 255);
            glVertex2f(transform.Base.Position.X.ToFloating<float>(), transform.Base.Position.Y.ToFloating<float>());
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
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);

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

            AABB boundingBox = polygonCollider.GetAABB(transform, transformMeta);
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
    ComponentCollection<PolygonCollider>* polygonColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;

public:
    EntitySet<MAXENTITIES> Entities;
};