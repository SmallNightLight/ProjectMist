#pragma once

#include "../../ECS/ECS.h"

class PolygonColliderRenderer : public System
{
public:
    explicit PolygonColliderRenderer(Layer* world) : System(world)
    {
        colliderTransformCollection = layer->GetComponentCollection<ColliderTransform>();
        polygonColliderCollection = layer->GetComponentCollection<PolygonCollider>();
        colliderRenderDataCollection = layer->GetComponentCollection<ColliderRenderData>();
        rigidBodyDataCollection = layer->GetComponentCollection<RigidBodyData>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<ColliderTransform>());
        signature.set(layer->GetComponentType<PolygonCollider>());
        signature.set(layer->GetComponentType<ColliderRenderData>());
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
            std::vector<Vector2> vertices = transform.GetTransformedVertices(polygonCollider.TransformedVertices, polygonCollider.Vertices);

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
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);

            bool active = false;
            if (rigidBodyDataCollection->HasComponent(entity))
            {
                active = rigidBodyDataCollection->GetComponent(entity).Active;
            }

            if (active)
            {
                glColor3ub(128, 128, 128);
            }
            else
            {
                glColor3ub(0, 255, 0);
            }

            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);

            AABB boundingBox = transform.GetAABB(polygonCollider.TransformedVertices, polygonCollider.Vertices);
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());
            glVertex2f(boundingBox.Min.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Max.Y.ToFloating<float>());
            glVertex2f(boundingBox.Max.X.ToFloating<float>(), boundingBox.Min.Y.ToFloating<float>());

            glEnd();
        }
    }

private:
    std::shared_ptr<ComponentCollection<ColliderTransform>> colliderTransformCollection;
    std::shared_ptr<ComponentCollection<PolygonCollider>> polygonColliderCollection;
    std::shared_ptr<ComponentCollection<ColliderRenderData>> colliderRenderDataCollection;
    std::shared_ptr<ComponentCollection<RigidBodyData>> rigidBodyDataCollection;
};