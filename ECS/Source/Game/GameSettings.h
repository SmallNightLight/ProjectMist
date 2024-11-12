#pragma once

#include "../ECS/ECS.h"

static constexpr short MaxRollBackFrames = 15; //amount of save states - actually -1

static constexpr short NetworkFPS = 15;
static constexpr short SimulationFPS = 30;
static constexpr short RenderingFPS = 60;