#pragma once

class BoxColliderRenderer : public System
{
public:
    explicit BoxColliderRenderer(Layer* world) : System(world)
    {
        colliderTransformCollection = layer->GetComponentCollection<ColliderTransform>();
        boxColliderCollection = layer->GetComponentCollection<BoxCollider>();
        colliderRenderDataCollection = layer->GetComponentCollection<ColliderRenderData>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<ColliderTransform>());
        signature.set(layer->GetComponentType<BoxCollider>());
        signature.set(layer->GetComponentType<ColliderRenderData>());
        return signature;
    }

    static Signature GetSignature(Layer* layer) override
    {
        Signature signature;
        signature.set(layer->GetComponentType<ColliderTransform>());
        signature.set(layer->GetComponentType<BoxCollider>());
        signature.set(layer->GetComponentType<ColliderRenderData>());
        return signature;
    }

    void Render()
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& transform = colliderTransformCollection->GetComponent(entity);
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);
            ColliderRenderData& colliderRenderData = colliderRenderDataCollection->GetComponent(entity);

            //Draw filled rectangle
            glColor3f(colliderRenderData.R, colliderRenderData.G, colliderRenderData.B);

            std::vector<Vector2> vertices = transform.GetTransformedVertices(boxCollider.TransformedVertices, boxCollider.Vertices);

            glBegin(GL_QUADS);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();

            //Draw white outline
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
            BoxCollider& boxCollider = boxColliderCollection->GetComponent(entity);

            glColor3f(0.5f, 0.5f, 0.5f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);

            AABB boundingBox = transform.GetAABB(boxCollider.TransformedVertices, boxCollider.Vertices);
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