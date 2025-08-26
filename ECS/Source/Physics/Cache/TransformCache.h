#pragma once

class TransformCache
{
public:
    inline TransformCache() noexcept = default;

    inline void Initialize()
    {
        filled = false;
    }

    inline void Cache(ComponentCollection<Transform>* transformCollection)
    {
        filled = true;
        data.Overwrite(transformCollection);
    }

    inline bool TryGetTransform(Entity entity, Transform& transform)
    {
        if (!filled) return false;

        transform = data.GetComponent(entity);
        return true;
    }

    inline void Reset() noexcept
    {
        filled = false;
    }

private:
    bool filled;
    ComponentCollection<Transform> data;
};