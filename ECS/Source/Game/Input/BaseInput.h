#pragma once

#include <algorithm>

struct BaseInput
{
    virtual ~BaseInput() = default;

    virtual void SetKeyState(u_int16_t glfwKey, bool isDown) = 0;

    virtual void SetMousePosition(double x, double y) = 0;

    virtual void Update() = 0;

    virtual bool GetKey(u_int16_t glfwKey) = 0;

    virtual bool GetKeyDown(u_int16_t glfwKey) = 0;

    virtual bool GetKeyUp(u_int16_t glfwKey) = 0;

    virtual Vector2 GetMousePosition(Camera* camera) const = 0;
};