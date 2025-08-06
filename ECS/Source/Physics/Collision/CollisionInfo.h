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

    inline CollisionInfo() noexcept = default;
    inline constexpr explicit CollisionInfo(Entity owner, Entity other, Vector2 normal, Fixed16_16 depth, Vector2 contact1, Vector2 contact2, uint8_t contactCount, bool isDynamic1, bool isDynamic2) :
        Entity1(owner),
        Entity2(other),
        Normal(normal),
        Depth(depth),
        Contact1(contact1),
        Contact2(contact2),
        ContactCount(contactCount),
        IsDynamic1(isDynamic1),
        IsDynamic2(isDynamic2) { }

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