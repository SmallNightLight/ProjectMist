#pragma once

#include "Input.h"
#include "../../Math/FixedTypes.h"
#include "../../Components/Camera.h"

#include <map>
#include <vector>
#include <algorithm>
#include <GLFW/glfw3.h>

template <std::size_t KeyCount>
class InputManager
{
public:
    explicit InputManager(const std::array<u_int16_t, KeyCount>& inputKeys)
    {
        u_int8_t keyIndex = 0;
        for (u_int16_t key : inputKeys)
        {
            keyIndexes[keyIndex++] = key;
        }

        currentInput = Input<KeyCount>();
        lastInput = Input<KeyCount>();

        instances.push_back(this);
    }

    static void SetupCallbacks(GLFWwindow& window)
    {
        glfwSetKeyCallback(&window, KeyCallback);
        glfwSetMouseButtonCallback(&window, MouseButtonCallback);
        glfwSetCursorPosCallback(&window, CursorPositionCallback);
    }

    ~InputManager()
    {
        instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
    }

    //Ket input methods
    bool GetKey(u_int16_t glfwKey)
    {
        return currentInput.Keys[keyIndexes[glfwKey]];
    }

    bool GetKeyDown(u_int16_t glfwKey)
    {
        return currentInput.Keys[keyIndexes[glfwKey]] && !lastInput.Keys[keyIndexes[glfwKey]];
    }

    bool GetKeyUp(u_int16_t glfwKey)
    {
        return !currentInput.Keys[keyIndexes[glfwKey]] && lastInput.Keys[keyIndexes[glfwKey]];
    }

    //Mouse position methods
    Vector2 GetMousePosition(Camera* camera) const
    {
        Fixed16_16 normalizedX = Fixed16_16::FromFloat<double>(mouseX) / Fixed16_16::FromFloat<double>(SCREEN_WIDTH);
        Fixed16_16 normalizedY = Fixed16_16::FromFloat<double>(SCREEN_HEIGHT - mouseY) / Fixed16_16::FromFloat<double>(SCREEN_HEIGHT);

        Fixed16_16 worldX = camera->Left + normalizedX * (camera->Right - camera->Left);
        Fixed16_16 worldY = camera->Bottom + normalizedY * (camera->Top - camera->Bottom);

        return Vector2 { worldX, worldY };
    }

    void Update()
    {
        lastInput = currentInput;
    }

    Input<KeyCount> GetCurrentInput()
    {
        return currentInput;
    }

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        for (InputManager* keyInput : instances)
        {
            keyInput->SetKeyState(key, action != GLFW_RELEASE);
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (button < 0 || button > GLFW_KEY_LAST) return;

        for (InputManager* input : instances)
        {
            input->SetKeyState(button, action != GLFW_RELEASE);
        }
    }

    static void CursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        for (InputManager* input : instances)
        {
            input->SetMousePosition(x, y);
        }
    }

    void SetKeyState(u_int16_t glfwKey, bool isDown)
    {
        lastInput.Keys[keyIndexes[glfwKey]] = currentInput.Keys[keyIndexes[glfwKey]];
        currentInput.Keys[keyIndexes[glfwKey]] = isDown;
    }

    void SetMousePosition(double x, double y)
    {
        mouseX = x;
        mouseY = y;
    }

public:
    std::array<u_int8_t, GLFW_KEY_LAST + 1> keyIndexes { };

    Input<KeyCount> currentInput;
    Input<KeyCount> lastInput;

private:
    inline static std::vector<InputManager*> instances;

    double mouseX = 0, mouseY = 0;
};