#pragma once

struct PhysicsWorldData
{
    inline PhysicsWorldData() noexcept = default;

    inline constexpr explicit PhysicsWorldData(FrameNumber startFrame, uint32_t seed) : CurrentFrame(startFrame), NumberGenerator(std::mt19937(seed)), Cache()
    {
        Cache.Initialize();
    }

    FrameNumber CurrentFrame;
    std::mt19937 NumberGenerator;
    PhysicsCache Cache;
};