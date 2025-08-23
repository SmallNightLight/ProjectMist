#pragma once

#include <map>

#include "ContactPair.h"

class ImpulseCache
{
public:
    inline void Cache(AccumulatedImpulse contactPair)
    {

    }

    inline bool TryGetImpulses(AccumulatedImpulse contactPair)
    {

    }

private:
    std::map<AccumulatedImpulse, MAXENTITIES> contactPairs;
};