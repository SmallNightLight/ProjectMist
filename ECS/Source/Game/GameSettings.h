#pragma once

#include "../ECS/ECS.h"

static constexpr short MaxRollBackFrames = 15; //amount of save states - actually -1

static constexpr short NetworkFPS = 15;
static constexpr short SimulationFPS = 60;
static constexpr short RenderingFPS = 60;

static constexpr bool RollbackDebugMode = true; //always rollback as much as possible

static const std::vector<uint16_t> playerInputKeys =
{
    GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_MIDDLE
};

static const std::vector<uint16_t> otherInputKeys =
{
    GLFW_KEY_ESCAPE, GLFW_KEY_SPACE, GLFW_KEY_R, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_C, GLFW_KEY_V
};