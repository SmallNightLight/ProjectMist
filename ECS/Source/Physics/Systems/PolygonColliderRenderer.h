#pragma once

#include "../../ECS/ECS.h"

class PolygonColliderRenderer final : public System
{
public:
    explicit PolygonColliderRenderer(PhysicsComponentManager& componentManager) : System()
    {
        colliderTransformCollection = componentManager.GetComponentCollection<ColliderTransform>();
        polygonColliderCollection = componentManager.GetComponentCollection<PolygonCollider>();
        colliderRenderDataCollection = componentManager.GetComponentCollection<ColliderRenderData>();
    }

    static Signature GetSignature()
    {
        Signature signature;
        signature.set(PhysicsComponentManager::GetComponentType<ColliderTransform>());
        signature.set(PhysicsComponentManager::GetComponentType<PolygonCollider>());
        signature.set(PhysicsComponentManager::GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render() const
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            //Get transformed vertices
            Vector2Span vertices = transform.GetTransformedVertices(polygonCollider.GetTransformedVertices(), polygonCollider.GetVertices());

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
            glVertex2f(transform.Position.X.ToFloating<float>(), transform.Position.Y.ToFloating<float>());
            glEnd();
        }
    }

    void RenderDebugOverlay() const
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);

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

            AABB boundingBox = colliderTransform.GetAABB(polygonCollider.GetTransformedVertices(), polygonCollider.GetVertices());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());

            glEnd();
        }
    }

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<PolygonCollider>* polygonColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};