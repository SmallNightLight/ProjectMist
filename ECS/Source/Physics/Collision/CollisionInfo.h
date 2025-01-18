#pragma once
#include "../../Math/FixedTypes.h"

struct CollisionInfo
{
    Vector2 Normal;
    Entity Entity1;
    Entity Entity2;
    Fixed16_16 Depth;

    Vector2 Contact1;
    Vector2 Contact2;
    int ContactCount;

    bool IsDynamic1;
    bool IsDynamic2;

    CollisionInfo(): Entity1(0), Entity2(0), Depth(0), ContactCount(0), IsDynamic1(false), IsDynamic2(false) { }
    CollisionInfo(Vector2 normal, Entity owner, Entity other, Fixed16_16 depth, Vector2 contact1, Vector2 contact2, int contactCount, bool isDynamic1, bool isDynamic2) :
        Normal(normal), Entity1(owner), Entity2(other), Depth(depth), Contact1(contact1), Contact2(contact2), ContactCount(contactCount), IsDynamic1(isDynamic1), IsDynamic2(isDynamic2) { }

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