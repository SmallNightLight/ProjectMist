#pragma once

#include "../ECS/ECS.h"

static constexpr short MaxRollBackFrames = 15; //amount of save states - actually -1

static constexpr short NetworkFPS = 15;
static constexpr short SimulationFPS = 30;
static constexpr short RenderingFPS = 60;

static constexpr std::array<u_int16_t, 3> playerInputKeys =
{
    GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_MIDDLE
};

static constexpr std::array<u_int16_t, 3> otherInputKeys =
{
    GLFW_KEY_ESCAPE, GLFW_KEY_SPACE, GLFW_KEY_R
};