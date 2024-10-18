#pragma once

#include <random>
#include <cassert>

template<typename T>
class FixedRandom
{
public:
    FixedRandom(const T& min, const T& max) : _min(min), _max(max)
    {
        assert(_min <= _max && "Minimum must be less than or equal to maximum.");
    }

    T operator()(std::default_random_engine& gen)
    {
        std::uniform_int_distribution<int64_t> dist(_min.raw_value(), _max.raw_value());
        return T::from_raw_value(dist(gen));
    }

private:
    T _min;
    T _max;
};