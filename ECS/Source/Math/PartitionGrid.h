#pragma once

#include "../Base/Settings.h"
#include "Rect.h"

#include <array>
#include <vector>
#include <functional>
#include <unordered_set>
#include <bitset>

#include <iostream>
#include <cmath>
#include <algorithm>

constexpr int int_ceil(float f)
{
    const int i = static_cast<int>(f);
    return f > (float)i ? i + 1 : i;
}


using Cell = std::uint32_t;
static constexpr std::int32_t BufferSize = 64;

static constexpr Rect PartitionArea = Rect(glm::vec2(0, 0), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
static constexpr float MaxEntitySize = 50.0f;
static constexpr float CellSize = MaxEntitySize * 2.0f;

static constexpr Cell CellCountX = int_ceil(PartitionArea.Size.x / CellSize);
static constexpr Cell CellCountY = int_ceil(PartitionArea.Size.y / CellSize);
static constexpr Cell CellCount = CellCountX * CellCountY;
static constexpr std::int32_t IndexNull = MAXENTITIES + 1; //?


struct SecondaryBuffer
{
    std::array<Entity, BufferSize> entities { };
    int32_t otherBufferIndex = IndexNull;
};

struct EntityPair
{
    Entity Entity1;
    Entity Entity2;

    EntityPair(Entity e1, Entity e2) : Entity1(e1), Entity2(e2) { }

    bool operator==(const EntityPair other) const
    {
        return Entity1 == other.Entity1 && Entity2 == other.Entity2;
    }

    bool operator!=(const EntityPair other) const
    {
        return Entity1 != other.Entity1 || Entity2 != other.Entity2;
    }
};

template<> struct std::hash<EntityPair>
{
    std::size_t operator()(const EntityPair& entityPair) const noexcept
    {
        return (BitReverseTable256[entityPair.Entity1 & 0xff] << 24) |
            (BitReverseTable256[(entityPair.Entity1 >> 8) & 0xff] << 16) |
            (BitReverseTable256[(entityPair.Entity1 >> 16) & 0xff] << 8) |
            (BitReverseTable256[(entityPair.Entity1 >> 24) & 0xff])
            | entityPair.Entity2;
    }
};

struct PartitionGrid //assuming that all entities have the same size (or the given size is the max size)
{
    PartitionGrid()
    {
        secondaryIndexes.fill(IndexNull);

        CreateGrid();
    }

    void CreateGrid()
    {
        glm::vec2 cellSize(CellSize, CellSize);

        for (Cell y = 0; y < CellCountY; ++y)
        {
            for (Cell x = 0; x < CellCountX; ++x)
            {
                cellAreas[y * CellCountX + x] = Rect(glm::vec2((float)x * CellSize, (float)y * CellSize), cellSize);
            }
        }
    }

    void InsertEntity(Entity entity, Rect entityArea)
    {
        entityAreas[entity] = entityArea;

        //Calculate the min and max cell indices that the entity rect may overlap
        Cell minCellX = static_cast<Cell>(std::floor(entityArea.Position.x / CellSize));
        Cell maxCellX = static_cast<Cell>(std::floor((entityArea.Position.x + entityArea.Size.x) / CellSize));
        Cell minCellY = static_cast<Cell>(std::floor(entityArea.Position.y / CellSize));
        Cell maxCellY = static_cast<Cell>(std::floor((entityArea.Position.y + entityArea.Size.y) / CellSize));

        //Clamp the indices to be within the grid bounds
        minCellX = std::max(Cell(0), minCellX);
        maxCellX = std::min(CellCountX - 1, maxCellX);
        minCellY = std::max(Cell(0), minCellY);
        maxCellY = std::min(CellCountY - 1, maxCellY);

        int entityCells = 0;

        //Iterate through the relevant cells and add their indices
        for (Cell y = minCellY; y <= maxCellY; ++y)
        {
            for (Cell x = minCellX; x <= maxCellX; ++x)
            {
                //Cell overlaps with entity
                Cell cell = y * CellCountX + x;

                if (entityCount[cell] < BufferSize)
                {
                    entities[cell * BufferSize + entityCount[cell]] = entity;
                    entityCount[cell]++;
                }
                else
                {
                   // std::cout << "Could not add entity buffer overflow" << std::endl;
                }

                entityCells++;
            }
        }

        intersectingEntities.set(entity, entityCells > 1);
    }

    std::vector<EntityPair> GetEntityPairs()
    {
        std::vector<EntityPair> entityPairs;
        entityPairs.reserve(MAXENTITIES);

        //Store here the pairs where both entities are overlapping a grid line
        std::unordered_set<EntityPair> overlappingPairs { };

        for(Cell cell = 0; cell < CellCount; cell++)
        {
            for (int i = 0; i < entityCount[cell] && i < BufferSize; i++)
            {
                for(int j = i + 1; j < entityCount[cell] && j < BufferSize; j++)
                {
                    EntityPair pair {entities[cell * BufferSize + i], entities[cell * BufferSize + j]};

                    if (overlappingPairs.find(pair) == overlappingPairs.end())
                    {
                        entityPairs.push_back(pair);
                    }

                    if (intersectingEntities[pair.Entity1] && intersectingEntities[pair.Entity2])
                    {
                        overlappingPairs.insert(pair);
                    }
                }
            }

            if (entityCount[cell] > BufferSize && secondaryIndexes[cell] != IndexNull) //last statement most likely not needed
            {
                //More entities stored in additional buffers
                //std::int8_t index = secondaryIndexes[]
            }
        }

        return entityPairs;
    }

    [[nodiscard]] const std::array<Rect, CellCount>& GetCellAreas() const
    {
        return cellAreas;
    }

    [[nodiscard]] const std::array<Rect, MAXENTITIES>& GetEntityAreas() const
    {
        return entityAreas;
    }


    void MoveEntity(Entity entity, Rect newArea)
    {
        //TODO: with arrays: save the current cell and check if the cell changed, with bitset * 4 * MAXENTITIES?

        Rect oldArea = entityAreas[entity];

        //Remove the entity from the cells by first checking which cell the entity is still registered in
        Cell minCellX = static_cast<Cell>(std::floor(oldArea.Position.x / CellSize));
        Cell maxCellX = static_cast<Cell>(std::floor((oldArea.Position.x + oldArea.Size.x) / CellSize));
        Cell minCellY = static_cast<Cell>(std::floor(oldArea.Position.y / CellSize));
        Cell maxCellY = static_cast<Cell>(std::floor((oldArea.Position.y + oldArea.Size.y) / CellSize));

        minCellX = std::max(Cell(0), minCellX);
        maxCellX = std::min(CellCountX - 1, maxCellX);
        minCellY = std::max(Cell(0), minCellY);
        maxCellY = std::min(CellCountY - 1, maxCellY);

        //Iterate through the relevant cells and add their indices
        for (Cell y = minCellY; y <= maxCellY; ++y)
        {
            for (Cell x = minCellX; x <= maxCellX; ++x)
            {
                Cell cell = y * CellCountX + x;

                for(int i = 0; i < entityCount[cell]; i++)
                {
                    if (entities[cell * BufferSize + i] == entity)
                    {
                        //Replace this entity with the last entity in the buffer to keep the memory continuous
                        entities[cell * BufferSize + i] = entities[cell * BufferSize + (BufferSize - 1)];
                        entityCount[cell]--;
                    }
                }
            }
        }

        entityAreas[entity] = newArea;
        InsertEntity(entity, newArea);
    }


private:
    std::array<Rect, CellCount> cellAreas;
    std::array<Rect, MAXENTITIES> entityAreas { };

    //Primary buffer
    std::array<Entity, CellCount * BufferSize> entities { };
    std::array<int32_t,  CellCount> secondaryIndexes { };
    std::array<std::int8_t, CellCount> entityCount { };
    std::bitset<MAXENTITIES> intersectingEntities;
};

/*
 with MaxEntities = 1000 and going in *2 steps

1 array:
8 entities per collection
1000 collections
= 8000 total allocated

2 array:
16 entities per collection
111 collections
= 1776 total allocated

3 array:
32 entities per collection
58 collections
= 1856

4 array:
64 entities per collection
16 collections
= 1920 total allocated

5 array:
128 entities per collection
= 1.920 total allocated

6 array:
256 entities per collection
= 1792 total allocated

7 array
512 entities per collection
= 1536 allocated

8 array
1024 entities per collection
= 1024 allocated


= 19.824 entities ALLOCATED


Now with fewer

1 array:
8 entities per collection
1000 collections
= 8000 total allocated

2 array:
32 entities per collection
= 3552 allocated

3 array:
128 entities per collection
= 3840

4 array:
512 entities per collection
= 3584

5 array:
1024 entities per collection
= 1024

=20.000

 now starting with 16 but still +2

1 array:
8 entities per collection
1000 collections
= 8000 total allocated

2 array:
16 entities per collection
111 collections
= 1776 total allocated

3 array:
64 entities per collection
= 3712 allocated

 4 array:
 256 entities per collection
 = 3840

 5 array:
 1024 entities per collection
 = 3072

 = 20.400 TOTAL ALLOCATED
 */