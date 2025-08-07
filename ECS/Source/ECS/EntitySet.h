#pragma once

#include "ECSSettings.h"

#include <cstdint>
#include <cassert>
#include <array>
#include <cstring>

template<uint32_t Capacity>
class EntitySet
{
public:
    EntitySet()
    {
        entityToIndex.fill(InvalidEntity);
    }

    EntitySet& operator=(const EntitySet& other)
    {
        if (this == &other) return *this;

        entityCount = other.entityCount;

        std::memcpy(entities.data(), other.entities.data(), entityCount * sizeof(Entity));
        std::memcpy(entityToIndex.data(), other.entityToIndex.data(), entityToIndex.size() * sizeof(uint32_t));

        return *this;
    }

    bool Insert(Entity entity)
    {
        assert(entity < MAXENTITIES && "Entity out of range");

        if (entityToIndex[entity] != InvalidEntity) return false;

        assert(entityCount < Capacity && "EntitySet capacity exceeded");

        uint32_t index = entityCount++;
        entities[index] = entity;
        entityToIndex[entity] = index;

        return true;
    }

    //Removes the entity from the given entity
    void Erase(Entity entity)
    {
        assert(entity < MAXENTITIES && "Entity out of range");

        uint32_t index = entityToIndex[entity];

        if (index == InvalidEntity) return;

        uint32_t lastIndex = entityCount - 1;  //TODO: range exception
        Entity lastEntity = entities[lastIndex];

        //Swap the last entity into the removed spot
        entities[index] = lastEntity;
        entityToIndex[lastEntity] = index;

        //Invalidate the removed entity
        entityToIndex[entity] = InvalidEntity;
        entityCount--;
    }

    inline bool Contains(Entity entity) const
    {
        assert(entity < MAXENTITIES && "Entity out of range");
        return entityToIndex[entity] != InvalidEntity;
    }

    void Clear()
    {
        for (uint32_t i = 0; i < entityCount; ++i)
        {
            entityToIndex[entities[i]] = InvalidEntity;
        }

        entityCount = 0;
    }

    uint32_t Size() const { return entityCount; }

    bool Empty() const { return entityCount == 0; }

    Entity* begin() { return entities.data(); }
    Entity* end() { return entities.data() + entityCount; }

    const Entity* begin() const { return entities.data(); }
    const Entity* end() const { return entities.data() + entityCount; }

private:
    static constexpr uint32_t InvalidEntity = Capacity;

    std::array<Entity, Capacity> entities { };
    std::array<uint32_t, MAXENTITIES> entityToIndex { };

    uint32_t entityCount = 0;
};