#pragma once

#include <vector>
#include <GLFW/glfw3.h>

#include "Input.h"

class InputManager
{
public:
    InputManager()
    {

    }

    static void RegisterInput(Input* input)
    {
        instances.push_back(input);
    }

    static void RemoveInput(Input* input)
    {
        instances.erase(std::remove(instances.begin(), instances.end(), input), instances.end());
    }

    static void SetupCallbacks(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetCursorPosCallback(window, CursorPositionCallback);
    }

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key < 0 || key > GLFW_KEY_LAST) return;

        for (Input* input : instances)
        {
            input->SetKeyState(key, action != GLFW_RELEASE);
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (button < 0 || button > GLFW_KEY_LAST) return;

        for (Input* input : instances)
        {
            input->SetKeyState(button, action != GLFW_RELEASE);
        }
    }

    static void CursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        for (Input* input : instances)
        {
            input->SetMousePosition(x, y);
        }
    }

    inline static std::vector<Input*> instances;
};