#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/HashUtils.h"

#include <sstream>
#include <fstream>

struct CollisionInfo
{
    Entity Entity1;
    Entity Entity2;

    Vector2 Normal;
    Fixed16_16 Depth;

    Vector2 Contact1;
    Vector2 Contact2;
    uint8_t ContactCount;

    bool IsDynamic1;
    bool IsDynamic2;

    constexpr inline CollisionInfo(): Entity1(0), Entity2(0), Depth(0), ContactCount(0), IsDynamic1(false), IsDynamic2(false) { }
    constexpr inline CollisionInfo(Entity owner, Entity other, Vector2 normal, Fixed16_16 depth, Vector2 contact1, Vector2 contact2, uint8_t contactCount, bool isDynamic1, bool isDynamic2) :
        Entity1(owner),
        Entity2(other),
        Normal(normal),
        Depth(depth),
        Contact1(contact1),
        Contact2(contact2),
        ContactCount(contactCount),
        IsDynamic1(isDynamic1),
        IsDynamic2(isDynamic2) { }

    bool operator==(const CollisionInfo& otherCollisionInfo) const
    {
        return
            Entity1 == otherCollisionInfo.Entity1 &&
            Entity2 == otherCollisionInfo.Entity2 &&
            Normal == otherCollisionInfo.Normal &&
            Depth == otherCollisionInfo.Depth &&
            ContactCount == otherCollisionInfo.ContactCount;
            //Contact1 == otherCollisionInfo.Contact1 &&
            //Contact2 == otherCollisionInfo.Contact2 &&

            //IsDynamic1 == otherCollisionInfo.IsDynamic1 &&
            //IsDynamic2 == otherCollisionInfo.IsDynamic2;
    }

    bool operator!=(const CollisionInfo& otherCollisionInfo) const
    {
        return !(*this == otherCollisionInfo);
    }

    static void LogCollisions(const std::vector<CollisionInfo>& collisions, int frame, uint32_t id, const std::string& baseFileName = "collision_logs")
    {
        std::ostringstream oss;
        oss << baseFileName << "_" << id << ".txt";
        std::string fileName = oss.str();

        std::ofstream logFile(fileName, std::ios::app); // Open in append mode
        if (!logFile.is_open())
        {
            std::cerr << "Error: Could not open log file: " << fileName << std::endl;
            return;
        }

        logFile << "Frame " << frame << "/\n";

        for (size_t i = 0; i < collisions.size(); ++i)
        {
            const CollisionInfo& col = collisions[i];
            //logFile << "Collision " << i + 1 << ":\n";
            //logFile << "  Normal: (" << col.Normal.X << ", " << col.Normal.Y << ")\n";
            logFile << col.Entity1 << " ";;
            logFile << col.Entity2;
            //logFile << "  Depth: " << col.Depth << "\n";
            //logFile << "  Contact1: (" << col.Contact1.X << ", " << col.Contact1.Y << ")\n";
            //logFile << "  Contact2: (" << col.Contact2.X << ", " << col.Contact2.Y << ")\n";
            //logFile << "  ContactCount: " << col.ContactCount << "\n";
            //logFile << "  IsDynamic1: " << (col.IsDynamic1 ? "true" : "false") << "\n";
            //logFile << "  IsDynamic2: " << (col.IsDynamic2 ? "true" : "false") << "\n";
            logFile << "\n";
        }

        logFile << "----------------------------------------\n\n";
        logFile.close();
    }
};

struct CollisionInfoHash
{
    std::size_t operator()( const CollisionInfo& collisionInfo ) const
    {
        uint32_t hash = CombineHash(static_cast<uint32_t>(collisionInfo.Entity1), static_cast<uint32_t>(collisionInfo.Entity2));
        hash = CombineHash(hash, static_cast<uint32_t>(collisionInfo.Normal.X.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(collisionInfo.Normal.Y.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(collisionInfo.Depth.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(collisionInfo.ContactCount));

        return static_cast<std::size_t>(hash);
    }
};