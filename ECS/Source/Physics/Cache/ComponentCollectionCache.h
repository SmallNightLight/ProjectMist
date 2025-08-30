#pragma once

template<typename T>
class ComponentCollectionCache
{
public:
    inline ComponentCollectionCache() noexcept = default;

    inline void Initialize()
    {
        filled = false;
    }

    inline void Cache(ComponentCollection<T>* collection)
    {
        filled = true;
        data.Overwrite(collection);
    }

    inline bool TryGetTransform(Entity entity, T& result)
    {
        if (!filled) return false;

        result = data.GetComponent(entity);
        return true;
    }

    inline void Reset() noexcept
    {
        filled = false;
    }

private:
    bool filled;
    ComponentCollection<T> data;
};