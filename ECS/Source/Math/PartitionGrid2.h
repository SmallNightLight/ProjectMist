#pragma once

#include "../Base/Settings.h"
#include "Rect.h"

#include <iostream>
#include <array>
#include <vector>
#include <cmath>

constexpr int int_ceil(float f)
{
    const int i = static_cast<int>(f);
    return f > (float)i ? i + 1 : i;
}


using Cell = std::uint32_t;
static constexpr std::int32_t BufferSize = 32;

static constexpr Rect PartitionArea = Rect(glm::vec2(0, 0), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
static constexpr float MaxEntitySize = 50.0f;
static constexpr float CellSize = MaxEntitySize;

static constexpr Cell CellCountX = int_ceil(PartitionArea.Size.x / CellSize);
static constexpr Cell CellCountY = int_ceil(PartitionArea.Size.y / CellSize);
static constexpr Cell CellCount = CellCountX * CellCountY;
static constexpr std::int32_t IndexNull = MAXENTITIES + 1; //?
static constexpr std::int32_t CellNull = CellCount + 1;
static constexpr glm::ivec2 CellOffsets[4] =
{
    glm::ivec2(0, 0),  //Self
    glm::ivec2(1, 0),  //Right
    glm::ivec2(0, 1),  //Below
    glm::ivec2(1, 1)   //Right-Below
};


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

struct PartitionGrid2 //assuming that all entities have the same size (or the given size is the max size) and no position is out of bounds
{
    PartitionGrid2()
    {
        //secondaryIndexes.fill(IndexNull);

        //CreateGrid();
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

    void InsertEntity(Entity entity, glm::vec2 position) //Does not handle out of bounds position
    {
        //Calculate the cell base on the position
        Cell cellX = static_cast<Cell>(std::floor(position.x / CellSize));
        Cell cellY = static_cast<Cell>(std::floor(position.y / CellSize));

        Cell cell = cellY * CellCountX + cellX;

        if (entityCount[cell] < BufferSize)
        {
            int32_t index = cell * BufferSize + entityCount[cell];

            if (entity > MAXENTITIES || cell > CellCount)
                return;


            entityIndexes[entity] = index;
            entities[index] = entity;
            entityCells[entity] = cell;
            entityCount[cell]++;
        }
        else
        {
            entityIndexes[entity] = IndexNull;
            entityCells[entity] = CellCount + 1;;

            //std::cout << "Could not add entity buffer overflow" << std::endl; //TODO: CHECK HOW MUCH OVERFLOW THERE EIS
        }
    }

    std::vector<EntityPair> GetEntityPairs()
    {
        std::vector<EntityPair> entityPairs;
        entityPairs.reserve(MAXENTITIES);

        for(Cell cellX = 0; cellX < CellCountX; ++cellX)
        {
            for(Cell cellY = 0; cellY < CellCountY; ++cellY)
            {
                for(glm::ivec2 offset : CellOffsets)
                {
                    Cell newX = cellX + offset.x;
                    Cell newY = cellY + offset.y;

                    // heck if the new cell is within grid bounds
                    if (newX >= 0 && newX < CellCountX && newY >= 0 && newY < CellCountY)
                    {
                        Cell cell = newY * CellCountX + newX;

                        for (int i = 0; i < entityCount[cell] && i < BufferSize; i++)
                        {
                            for(int j = i + 1; j < entityCount[cell] && j < BufferSize; j++)
                            {
                                if (entities[cell * BufferSize + i] > MAXENTITIES) {
                                    int a = 0;
                                }

                                entityPairs.emplace_back(entities[cell * BufferSize + i], entities[cell * BufferSize + j]);
                            }
                        }
                    }



                    /*if (entityCount[cell] > BufferSize && secondaryIndexes[cell] != IndexNull) //last statement most likely not needed
                    {
                        //More entities stored in additional buffers
                        //std::int8_t index = secondaryIndexes[]
                    }*/
                }
            }
        }

        return entityPairs;
    }

    void MoveEntity(Entity entity, glm::vec2 newPosition)
    {
        //Replace this entity with the last entity in the buffer to keep the memory continuous
        Cell cell = entityCells[entity];
        entities[entityIndexes[entity]] = entities[cell * BufferSize + (BufferSize - 1)];
        entityCount[cell]--;

        InsertEntity(entity, newPosition);
    }

    [[nodiscard]] const std::array<Rect, CellCount>& GetCellAreas() const
    {
        return cellAreas;
    }

private:
    std::array<Rect, CellCount> cellAreas;
    std::array<int32_t, MAXENTITIES> entityIndexes { };
    std::array<int32_t, MAXENTITIES> entityCells { };

    //Primary buffer
    std::array<Entity, CellCount * BufferSize> entities { };
    //std::array<int32_t,  CellCount> secondaryIndexes { };
    std::array<std::int8_t, CellCount> entityCount { };
};