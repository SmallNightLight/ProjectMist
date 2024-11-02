#pragma once

class BoxColliderRenderer : public System
{
public:
    explicit BoxColliderRenderer(ECSWorld* world) : System(world)
    {
        colliderTransformCollection = World->GetComponentCollection<ColliderTransform>();
        boxColliderCollection = World->GetComponentCollection<BoxCollider>();
        colliderRenderDataCollection = World->GetComponentCollection<ColliderRenderData>();
    }

    [[nodiscard]] Signature GetSignature() const
    {
        Signature signature;
        signature.set(World->GetComponentType<ColliderTransform>());
        signature.set(World->GetComponentType<BoxCollider>());
        signature.set(World->GetComponentType<ColliderRenderData>());
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

            std::vector<Vector2> vertices = CollisionDetection::GetTransformedVertices(transform, boxCollider);

            glBegin(GL_QUADS);
            for (const auto& vertex : vertices)
            {
                glVertex2f(vertex.X.ToFloating<float>(), vertex.Y.ToFloating<float>());
            }
            glEnd();

            //Draw white outline
            if (colliderRenderData.Outline)
            {
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
    }

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<BoxCollider>* boxColliderCollection;
    ComponentCollection<ColliderRenderData>* colliderRenderDataCollection;
};