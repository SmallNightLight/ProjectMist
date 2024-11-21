#pragma once

#include <algorithm>

struct BaseInput
{
    virtual ~BaseInput() = default;

    virtual void SetKeyState(uint16_t glfwKey, bool isDown) = 0;

    virtual void SetMousePosition(double x, double y) = 0;

    virtual void Update() = 0;

    virtual bool GetKey(uint16_t glfwKey) = 0;

    virtual bool GetKeyDown(uint16_t glfwKey) = 0;

    virtual bool GetKeyUp(uint16_t glfwKey) = 0;

    virtual Vector2 GetMousePosition(Camera* camera) const = 0;
};