#pragma once

#include "../../ECS/ECS.h"

class BoxColliderRenderer final : public System
{
public:
    explicit BoxColliderRenderer(PhysicsComponentManager& componentManager) : System()
    {
        colliderTransformCollection = componentManager.GetComponentCollection<ColliderTransform>();
        boxColliderCollection = componentManager.GetComponentCollection<BoxCollider>();
        colliderRenderDataCollection = componentManager.GetComponentCollection<ColliderRenderData>();
    }

    static Signature GetSignature()
    {
        Signature signature;
        signature.set(PhysicsLayer::GetComponentType<ColliderTransform>());
        signature.set(PhysicsLayer::GetComponentType<BoxCollider>());
        signature.set(PhysicsLayer::GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render() const
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            //Draw filled rectangle
            glColor3ub(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);

            Vector2Span vertices = transform.GetTransformedVertices(boxCollider.GetTransformedVertices(), boxCollider.GetVertices());

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
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);

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

            AABB boundingBox = colliderTransform.GetAABB(boxCollider.GetTransformedVertices(), boxCollider.GetVertices());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());

            glEnd();
        }
    }

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<BoxCollider>* boxColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};