#pragma once

class PhysicsUtils
{
public:
    static Entity CreateCircle(Layer& layer, const Vector2& position, const Fixed16_16& radius, RigidBodyType shape = Dynamic, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
    {
        Entity entity = layer.CreateEntity();

        layer.AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Circle, shape));
        layer.AddComponent(entity, CircleCollider(radius));
        layer.AddComponent(entity, RigidBodyData::CreateCircleRigidBody(radius, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 8), Fixed16_16(0, 4)));
        layer.AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    static Entity CreateRandomCircleFromPosition(Layer& layer, std::mt19937& numberGenerator, const Vector2& position)
    {
        return CreateCircle(layer, position, Fixed16_16(1), Dynamic, GetRandomColor(numberGenerator), GetRandomColor(numberGenerator), GetRandomColor(numberGenerator));
    }

    static Entity CreateRandomCircle(Layer& layer, std::mt19937& numberGenerator, Fixed16_16 left, Fixed16_16 right, Fixed16_16 bottom, Fixed16_16 top)
    {
        FixedRandom16_16 randomPositionX(left, right);
        FixedRandom16_16 randomPositionY(bottom, top);

        return CreateCircle(layer, Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), Fixed16_16(1), Dynamic, GetRandomColor(numberGenerator), GetRandomColor(numberGenerator), GetRandomColor(numberGenerator));
    }

    static Entity CreateBox(Layer& layer, const Vector2& position, const Fixed16_16& width, const Fixed16_16& height, RigidBodyType shape = Dynamic, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
    {
        Entity entity = layer.CreateEntity();

        layer.AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Box, shape));
        layer.AddComponent(entity, BoxCollider(width, height));
        layer.AddComponent(entity, RigidBodyData::CreateBoxRigidBody(width, height, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 8), Fixed16_16(0, 4)));
        layer.AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    static Entity CreateRandomBoxFromPosition(Layer& layer, std::mt19937& numberGenerator, const Vector2& position)
    {
        return CreateBox(layer, position, Fixed16_16(2), Fixed16_16(2), Dynamic, GetRandomColor(numberGenerator), GetRandomColor(numberGenerator), GetRandomColor(numberGenerator));
    }

    static Entity CreateRandomBox(Layer& layer, std::mt19937& numberGenerator, Fixed16_16 left, Fixed16_16 right, Fixed16_16 bottom, Fixed16_16 top)
    {
        FixedRandom16_16 randomPositionX(left, right);
        FixedRandom16_16 randomPositionY(bottom, top);

        return CreateBox(layer, Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), Fixed16_16(2), Fixed16_16(2), Dynamic, GetRandomColor(numberGenerator), GetRandomColor(numberGenerator), GetRandomColor(numberGenerator));
    }

    static Entity CreatePolygon(Layer& layer, const Vector2& position, const std::vector<Vector2>& vertices, RigidBodyType shape = Dynamic, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
    {
        Entity entity = layer.CreateEntity();

        layer.AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Convex, shape));
        layer.AddComponent(entity, PolygonCollider(vertices));
        layer.AddComponent(entity, RigidBodyData::CreatePolygonRigidBody(vertices, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 8), Fixed16_16(0, 4)));
        layer.AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    static Entity CreateRandomPolygonFromPosition(Layer& layer, std::mt19937& numberGenerator, const Vector2& position)
    {
        return CreatePolygon(layer, Vector2(position), GetRandomVertices(numberGenerator), Dynamic, GetRandomColor(numberGenerator), GetRandomColor(numberGenerator), GetRandomColor(numberGenerator));
    }

    static Entity CreateRandomPolygon(Layer& layer, std::mt19937& numberGenerator, Fixed16_16 left, Fixed16_16 right, Fixed16_16 bottom, Fixed16_16 top)
    {
        FixedRandom16_16 randomPositionX(left, right);
        FixedRandom16_16 randomPositionY(bottom, top);

        return CreatePolygon(layer, Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), GetRandomVertices(numberGenerator), Dynamic, GetRandomColor(numberGenerator), GetRandomColor(numberGenerator), GetRandomColor(numberGenerator));
    }

    //returns the vertices of a random convex shape
    static std::vector<Vector2> GetRandomVertices(std::mt19937& numberGenerator)
    {
        FixedRandom16_16 randomRadius(Fixed16_16(1), Fixed16_16(3));
        Fixed16_16 radius = randomRadius(numberGenerator);

        std::uniform_int_distribution randomSide(3, 6);
        int numSides = randomSide(numberGenerator);

        //Generate vertices around the origin
        std::vector<Vector2> vertices;
        Fixed16_16 angleIncrement = Fixed16_16(2) * Fixed16_16::pi() / Fixed16_16(numSides);

        for (int i = 0; i < numSides; ++i)
        {
            Fixed16_16 angle = angleIncrement * Fixed16_16(i);
            Vector2 vertex(radius * cos(angle), radius * sin(angle));
            vertices.push_back(vertex);
        }

        return vertices;
    }

    static uint8_t GetRandomColor(std::mt19937& numberGenerator)
    {
        std::uniform_int_distribution<unsigned int> randomColor(0, 255);
        return static_cast<uint8_t>(randomColor(numberGenerator));
    }
};