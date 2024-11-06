#pragma once

class PolygonColliderRenderer : public System
{
public:
    explicit PolygonColliderRenderer(ECSWorld* world) : System(world)
    {
            colliderTransformCollection = World->GetComponentCollection<ColliderTransform>();
            polygonColliderCollection = World->GetComponentCollection<PolygonCollider>();
            colliderRenderDataCollection = World->GetComponentCollection<ColliderRenderData>();
    }

    [[nodiscard]] Signature GetSignature() const
    {
        Signature signature;
        signature.set(World->GetComponentType<ColliderTransform>());
        signature.set(World->GetComponentType<PolygonCollider>());
        signature.set(World->GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render()
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            // Get transformed vertices
            std::vector<Vector2> vertices = transform.GetTransformedVertices(polygonCollider.TransformedVertices, polygonCollider.Vertices);

            // Draw filled polygon
            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);
            glBegin(GL_POLYGON);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();

            // Draw white outline
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();
        }
    }

    void RenderAABB()
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            PolygonCollider& polygonCollider = polygonColliderCollection->GetComponent(entity);

            glColor3f(0.5f, 0.5f, 0.5f);
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
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<PolygonCollider>* polygonColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};