#pragma once

#include "../Base/Settings.h"
#include "Rect.h"

#include <iostream>
#include <array>
#include <vector>
#include <queue>

using Cell = std::uint32_t;
static constexpr std::uint32_t MainBufferSize = 64;
static constexpr std::uint32_t ExtraBufferSize = 32;
static constexpr std::uint32_t ExtraBufferCount = MAXENTITIES / ExtraBufferSize;

static constexpr Rect PartitionArea = Rect(Vector2 (0, 0), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));
static constexpr Fixed16_16 MaxEntitySize = Fixed16_16::FromFixed(50, 0);
static constexpr Fixed16_16 CellSize = MaxEntitySize;

static constexpr Cell CellCountX = static_cast<Cell>(fpm::ceilInt(PartitionArea.Size.X / CellSize));
static constexpr Cell CellCountY = static_cast<Cell>(fpm::ceilInt(PartitionArea.Size.Y / CellSize));
static constexpr Cell CellCount = CellCountX * CellCountY;
static constexpr std::uint32_t IndexNull = MAXENTITIES + 1;
static constexpr std::uint32_t CellNull = CellCount + 1;
static constexpr std::uint32_t SecondaryIndexNull = ExtraBufferCount + 1;
static constexpr Vector2uI CellOffsets[4] =
{
    Vector2uI(0, 0),  //Self
    Vector2uI(1, 0),  //Right
    Vector2uI(0, 1),  //Below
    Vector2uI(1, 1)   //Right-Below
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
        Vector2 cellSize(CellSize, CellSize);

        for (Cell y = 0; y < CellCountY; ++y)
        {
            for (Cell x = 0; x < CellCountX; ++x)
            {
                cellAreas[y * CellCountX + x] = Rect(Vector2(x * CellSize, y * CellSize), cellSize);
            }
        }
    }

    void InsertEntity(Entity entity, Vector2 position) //Does not handle out of bounds position
    {
        InsertEntity(entity, GetCell(position));
    }

    void InsertEntity(Entity entity, Cell cell)
    {
        assert(entity < MAXENTITIES && "Could not insert entity - entity above entity limit");
        assert(cell < CellCount && "Could not insert entity - cell above cell limit");

        if (entityCount[cell] < MainBufferSize)
        {
            uint32_t index = MainBufferSize * cell + entityCount[cell];
            entityIndexes[entity] = index;
            buffer[index] = entity;
            entityCells[entity] = cell;
            entityCount[cell]++;
        }
        else
        {
            uint32_t bufferIndex = SecondaryIndexNull;

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
                        uint32_t nextBufferIndex = extraBufferIndex[bufferIndex];

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
                uint32_t index = MainBufferSize * CellCount + ExtraBufferSize * (bufferIndex - CellCount) + entityCount[bufferIndex];
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

    //Get the cell ID from a position. IMPORTANT: Clamps position to but at least 0, to prevent integer overflow
    [[nodiscard]] static Cell GetCell(Vector2 position)
    {
        //Calculate the cell base on the position
        const Cell cellX = static_cast<Cell>(fpm::floor(fpm::max(position.X, Fixed16_16(0) ) / CellSize));
        const Cell cellY = static_cast<Cell>(fpm::floor(fpm::max(position.Y, Fixed16_16(0)) / CellSize));

        return cellY * CellCountX + cellX;
    }

    [[nodiscard]] std::vector<EntityPair> GetEntityPairs() const
    {
        std::vector<EntityPair> entityPairs;
        entityPairs.reserve(MAXENTITIES * 5);

        for(Cell cellX = 0; cellX < CellCountX; ++cellX)
        {
            for(Cell cellY = 0; cellY < CellCountY; ++cellY)
            {
                Cell mainCell = cellY * CellCountX + cellX;

                for(const Vector2uI offset : CellOffsets)
                {
                    const Cell newX = cellX + offset.X;
                    const Cell newY = cellY + offset.Y;

                    //Check if the new cell is within grid bounds
                    if (newX < CellCountX && newY < CellCountY)
                    {
                        const Cell otherCell = newY * CellCountX + newX;

                        uint32_t currentBufferIndex1 = mainCell;
                        uint8_t totalEntityCount1 = entityCount[mainCell];
                        for (uint32_t i = 0; i < totalEntityCount1; i++)
                        {
                            uint32_t index1 = (currentBufferIndex1 == mainCell)  ? mainCell * MainBufferSize + i : MainBufferSize * CellCount + ExtraBufferSize * (currentBufferIndex1 - CellCount) + i;

                            if (i == entityCount[currentBufferIndex1] - 1 && extraBufferIndex[currentBufferIndex1] != IndexNull)
                            {
                                currentBufferIndex1 = extraBufferIndex[currentBufferIndex1];
                                totalEntityCount1 = entityCount[currentBufferIndex1];
                                i = -1;
                            }

                            uint32_t currentBufferIndex2;
                            uint32_t j = 0;

                            if (mainCell == otherCell)
                            {
                                currentBufferIndex2 = currentBufferIndex1;
                                j = i + 1;

                                if (j == entityCount[currentBufferIndex2] - 1 && extraBufferIndex[currentBufferIndex2] != IndexNull)
                                {
                                    currentBufferIndex2 = extraBufferIndex[currentBufferIndex2];
                                    j = 0;
                                }
                            }
                            else
                            {
                                currentBufferIndex2 = otherCell;
                            }

                            for(uint8_t totalEntityCount2 = entityCount[currentBufferIndex2]; j < totalEntityCount2; j++)
                            {
                                uint32_t index2 = currentBufferIndex2 == otherCell ? otherCell * MainBufferSize + j : MainBufferSize * CellCount + ExtraBufferSize * (currentBufferIndex2 - CellCount) + j;

                                if (j == entityCount[currentBufferIndex2] - 1 && extraBufferIndex[currentBufferIndex2] != IndexNull)
                                {
                                    currentBufferIndex2 = extraBufferIndex[currentBufferIndex2];
                                    totalEntityCount2 = entityCount[currentBufferIndex2];
                                    j = -1;
                                }

                                entityPairs.emplace_back(buffer[index1], buffer[index2]);
                            }
                        }
                    }
                }
            }
        }

        //std::cout << entityPairs.size() << std::endl;
        return entityPairs;
    }

    void MoveEntity(const Entity entity, const Vector2 newPosition)
    {
        const Cell cell = entityCells[entity];
        const Cell newCell = GetCell(newPosition);

        if (cell == newCell) return;

        if (cell != CellNull && entityIndexes[entity] != IndexNull)
        {
            //Adjust bufferIndex
            uint32_t bufferIndex = cell;
            uint32_t nextBufferIndex = extraBufferIndex[cell];
            uint32_t startBufferIndex = bufferIndex * MainBufferSize;

            while(nextBufferIndex != IndexNull)
            {
                bufferIndex = nextBufferIndex;
                nextBufferIndex = extraBufferIndex[bufferIndex];
                startBufferIndex = MainBufferSize * CellCount + ExtraBufferSize * (bufferIndex - CellCount);
            }

            //Replace this entity with the last entity in the buffer to keep the memory continuous
            const Entity lastEntity = buffer[startBufferIndex + (entityCount[bufferIndex] - 1)];
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
    std::array<uint32_t, MAXENTITIES> entityIndexes { };
    std::array<Cell, MAXENTITIES> entityCells { };

    std::array<Entity, CellCount * MainBufferSize + ExtraBufferCount * ExtraBufferSize> buffer { };
    std::array<std::uint8_t, CellCount + ExtraBufferCount> entityCount { };
    std::array<uint32_t, CellCount + ExtraBufferCount> extraBufferIndex { };
    std::array<uint32_t, ExtraBufferCount> extraReverseBufferIndex { };
    std::queue<uint32_t> availableSecondaryBuffers;
};