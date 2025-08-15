#pragma once

#include "../ECS/ECS.h"

//Window
static constexpr int32_t SCREEN_WIDTH = 800;
static constexpr int32_t SCREEN_HEIGHT = 600; //TODO: Bug - needs to be divisble by a specific number

static constexpr short NetworkFPS = 15;
static constexpr short SimulationFPS = 60;
static constexpr short RenderingFPS = 60;

static constexpr bool RollbackDebugMode = false; //Always rollback as much as possible

static const std::vector<uint16_t> playerInputKeys =
{
    GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_MIDDLE
};

static const std::vector<uint16_t> otherInputKeys =
{
    GLFW_KEY_ESCAPE, GLFW_KEY_RIGHT, GLFW_KEY_SPACE, GLFW_KEY_R, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_C, GLFW_KEY_V
};