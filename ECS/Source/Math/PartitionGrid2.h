#pragma once

#include "../Base/Settings.h"
#include "Rect.h"

#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <cmath>

constexpr int int_ceil(float f)
{
    const int i = static_cast<int>(f);
    return f > static_cast<float>(i) ? i + 1 : i;
}


using Cell = std::uint32_t;
static constexpr std::int32_t MainBufferSize = 64;
static constexpr std::int32_t ExtraBufferSize = 32;
static constexpr std::int32_t ExtraBufferCount = MAXENTITIES; //?

static constexpr Rect PartitionArea = Rect(glm::vec2(0, 0), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
static constexpr float MaxEntitySize = 50.0f;
static constexpr float CellSize = MaxEntitySize;

static constexpr Cell CellCountX = int_ceil(PartitionArea.Size.x / CellSize);
static constexpr Cell CellCountY = int_ceil(PartitionArea.Size.y / CellSize);
static constexpr Cell CellCount = CellCountX * CellCountY;
static constexpr std::int32_t IndexNull = MAXENTITIES + 1; //?
static constexpr std::int32_t CellNull = CellCount + 1;
static constexpr std::int32_t SecondaryIndexNull = ExtraBufferCount + 1;
static constexpr glm::ivec2 CellOffsets[4] =
{
    glm::ivec2(0, 0),  //Self
    glm::ivec2(1, 0),  //Right
    glm::ivec2(0, 1),  //Below
    glm::ivec2(1, 1)   //Right-Below
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
        extraBufferIndex.fill(IndexNull);
        extraReverseBufferIndex.fill(IndexNull);

        for(int i = CellCount; i < CellCount + ExtraBufferCount; ++i)
        {
            availableSecondaryBuffers.push(i);
        }

        entityCells.fill(CellNull);
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

    void InsertEntity(Entity entity, glm::vec2 position) //Does not handle out of bounds position
    {
        InsertEntity(entity, GetCell(position));
    }

    void InsertEntity(Entity entity, Cell cell)
    {
        assert(entity < MAXENTITIES && "Could not insert entity - entity above entity limit");
        assert(cell < CellCount && "Could not insert entity - cell above cell limit");

        if (entityCount[cell] < MainBufferSize)
        {
            int32_t index = MainBufferSize * cell + entityCount[cell];
            entityIndexes[entity] = index;
            buffer[index] = entity;
            entityCells[entity] = cell;
            entityCount[cell]++;
        }
        else
        {
            int32_t bufferIndex = SecondaryIndexNull;

            if (extraBufferIndex[cell] == IndexNull)
            {
                //No extra buffer initialized
                if (!availableSecondaryBuffers.empty())
                {
                    bufferIndex  = availableSecondaryBuffers.front();
                    availableSecondaryBuffers.pop();

                    extraBufferIndex[cell] = bufferIndex;
                    extraReverseBufferIndex[bufferIndex - CellCount] = cell;
                }
            }
            else
            {
                //Secondary buffer already initialized
                bufferIndex = extraBufferIndex[cell];

                while(true)
                {
                    if (entityCount[bufferIndex] >= ExtraBufferSize)
                    {
                        //This buffer is full - check if next buffer already exists
                        int32_t nextBufferIndex = extraBufferIndex[bufferIndex];

                        if (nextBufferIndex == IndexNull)
                        {
                            //Initialize a new extra buffer
                            if (!availableSecondaryBuffers.empty())
                            {
                                nextBufferIndex = availableSecondaryBuffers.front();
                                availableSecondaryBuffers.pop();

                                extraBufferIndex[bufferIndex] = nextBufferIndex;
                                extraReverseBufferIndex[nextBufferIndex - CellCount] = bufferIndex;

                                bufferIndex = nextBufferIndex;
                            }
                        }
                        else
                        {
                            assert(bufferIndex < CellCount + ExtraBufferCount && "buffer index out of bounds");
                            bufferIndex = extraBufferIndex[bufferIndex];
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (bufferIndex != SecondaryIndexNull)
            {
                int32_t index = MainBufferSize * CellCount + ExtraBufferSize * (bufferIndex - CellCount) + entityCount[bufferIndex];
                entityIndexes[entity] = index;
                buffer[index] = entity;
                entityCount[bufferIndex]++;
                entityCells[entity] = cell;
            }
            else
            {
                entityIndexes[entity] = IndexNull;
                entityCells[entity] = CellNull;
                std::cout << "Could not add entity - Buffer is full" << std::endl;
            }
        }
    }

    [[nodiscard]] static Cell GetCell(glm::vec2 position)
    {
        //Calculate the cell base on the position
        Cell cellX = static_cast<Cell>(std::floor(position.x / CellSize));
        Cell cellY = static_cast<Cell>(std::floor(position.y / CellSize));

        return cellY * CellCountX + cellX;
    }

    std::vector<EntityPair> GetEntityPairs() const
    {
        std::vector<EntityPair> entityPairs;
        entityPairs.reserve(MAXENTITIES * 5);

        for(Cell cellX = 0; cellX < CellCountX; ++cellX)
        {
            for(Cell cellY = 0; cellY < CellCountY; ++cellY)
            {
                for(glm::ivec2 offset : CellOffsets)
                {
                    Cell newX = cellX + offset.x;
                    Cell newY = cellY + offset.y;

                    //Check if the new cell is within grid bounds
                    if (newX < CellCountX && newY < CellCountY)
                    {
                        Cell cell = newY * CellCountX + newX;

                        for (int i = 0; i < entityCount[cell]; i++)
                        {
                            for(int j = i + 1; j < entityCount[cell]; j++)
                            {
                                int32_t index1 = 0;
                                int32_t index2 = 0;

                                if (i < MainBufferSize)
                                {
                                    //Entity i in main buffer
                                    index1 = cell * MainBufferSize + i;

                                }
                                else
                                {
                                    //Entity i in extra buff
                                    int32_t bufferIndex = extraBufferIndex[cell];
                                    int32_t newI = i - MainBufferSize;

                                    while(true)
                                    {
                                        if (newI < ExtraBufferSize)
                                        {
                                            //Entity is in this buffer
                                            index1 = bufferIndex * ExtraBufferSize + newI;
                                            break;
                                        }

                                        //Continue to next buffer
                                        bufferIndex = extraBufferIndex[bufferIndex];
                                        newI -= ExtraBufferSize;
                                    }
                                }

                                if (j < MainBufferSize)
                                {
                                    //Entity j in main buffer
                                    index2 = cell * MainBufferSize + j;
                                }
                                else
                                {
                                    //Entity j in extra buff
                                    int32_t bufferIndex = extraBufferIndex[cell];
                                    int32_t newJ = j - MainBufferSize;

                                    while(true)
                                    {
                                        if (newJ < ExtraBufferSize)
                                        {
                                            //Entity is in this buffer
                                            index2 = bufferIndex * ExtraBufferSize + newJ;
                                            break;
                                        }

                                        //Continue to next buffer
                                        bufferIndex = extraBufferIndex[bufferIndex];
                                        newJ -= ExtraBufferSize;
                                    }
                                }

                                entityPairs.emplace_back(buffer[index1], buffer[index2]);
                            }
                        }
                    }
                }
            }
        }

        return entityPairs;
    }

    void MoveEntity(Entity entity, glm::vec2 newPosition)
    {
        Cell cell = entityCells[entity];
        Cell newCell = GetCell(newPosition);

        if (cell == newCell) return;

        if (cell != CellNull && entityIndexes[entity] != IndexNull)
        {
            //Adjust bufferIndex
            int32_t bufferIndex = cell;
            int32_t nextBufferIndex = extraBufferIndex[cell];
            int32_t startBufferIndex = bufferIndex * MainBufferSize;

            while(nextBufferIndex != IndexNull)
            {
                bufferIndex = nextBufferIndex;
                nextBufferIndex = extraBufferIndex[bufferIndex];
                startBufferIndex = MainBufferSize * CellCount + ExtraBufferSize * (bufferIndex - CellCount);
            }

            //Replace this entity with the last entity in the buffer to keep the memory continuous
            Entity lastEntity = buffer[startBufferIndex + (entityCount[bufferIndex] - 1)];
            buffer[entityIndexes[entity]] = lastEntity;

            entityIndexes[lastEntity] = entityIndexes[entity];
            entityIndexes[entity] = IndexNull;
            entityCount[bufferIndex]--;

            if (bufferIndex != cell && entityCount[bufferIndex] == 0) //ExtraBufferSize % entityCount[bufferIndex] == 0
            {
                assert(extraReverseBufferIndex[bufferIndex - CellCount] != IndexNull && "Reverse extra buffer index not  set");

                //The extra buffer is now empty and needs to be removed
                extraBufferIndex[extraReverseBufferIndex[bufferIndex - CellCount]] = IndexNull;
                extraReverseBufferIndex[bufferIndex - CellCount] = IndexNull;
                availableSecondaryBuffers.push(bufferIndex);
            }
        }

        InsertEntity(entity, newCell);
    }

    [[nodiscard]] const std::array<Rect, CellCount>& GetCellAreas() const
    {
        return cellAreas;
    }

private:
    std::array<Rect, CellCount> cellAreas { };
    std::array<int32_t, MAXENTITIES> entityIndexes { };
    std::array<Cell, MAXENTITIES> entityCells { };

    std::array<Entity, CellCount * MainBufferSize + ExtraBufferCount * ExtraBufferSize> buffer { };
    std::array<std::int8_t, CellCount + ExtraBufferCount> entityCount { };
    std::array<int32_t, CellCount + ExtraBufferCount> extraBufferIndex { };
    std::array<int32_t, ExtraBufferCount> extraReverseBufferIndex { };
    std::queue<int32_t> availableSecondaryBuffers;
};