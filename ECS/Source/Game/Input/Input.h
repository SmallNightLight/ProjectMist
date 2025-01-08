#pragma once

#include "InputData.h"
#include "../../Math/FixedTypes.h"
#include "../../Components/Camera.h"
#include "../../ECS/ECSSettings.h"

#include <array>
#include <vector>
#include <cstdint>
#include <GLFW/glfw3.h>

static constexpr uint16_t keyNull = 255;

class InputManager;

struct Input
{
    Input() { }

    Input(const std::vector<uint16_t>& inputKeys)
    {
        RegisterInput();

        CurrentInput.resize(inputKeys.size(), false);
        LastInput.resize(inputKeys.size(), false);

        keyIndexes.fill(keyNull);

        uint8_t keyIndex = 0;
        for (uint16_t key : inputKeys)
        {
            keyIndexes[key] = keyIndex++;
        }
    }

    Input(const std::vector<uint16_t>& inputKeys, InputData inputData) : Input(inputKeys)
    {
        Overwrite(inputData);
    }

    ~Input()
    {
        RemoveInput();
    }

    InputData GetInputData(uint32_t frame) const
    {
        return InputData(frame, CurrentInput, LastInput, mouseX.raw_value(), mouseY.raw_value());
    }

    void Overwrite(const InputData& inputData)
    {
        CurrentInput = inputData.Input;
        LastInput = inputData.LastInput;

        mouseX = Fixed16_16::from_raw_value(inputData.MouseX);
        mouseY = Fixed16_16::from_raw_value(inputData.MouseY);
    }


    //Ket input methods
    bool GetKey(uint16_t glfwKey)
    {
        return CurrentInput[keyIndexes[glfwKey]];
    }

    bool GetKeyDown(uint16_t glfwKey)
    {
        return CurrentInput[keyIndexes[glfwKey]] && !LastInput[keyIndexes[glfwKey]];
    }

    bool GetKeyUp(uint16_t glfwKey)
    {
        return !CurrentInput[keyIndexes[glfwKey]] && LastInput[keyIndexes[glfwKey]];
    }

    //Mouse position methods
    Vector2 GetMousePosition(Camera* camera) const
    {
        Fixed16_16 normalizedX = (mouseX) / Fixed16_16(SCREEN_WIDTH);
        Fixed16_16 normalizedY = (Fixed16_16(SCREEN_HEIGHT) - mouseY) / Fixed16_16(SCREEN_HEIGHT);

        Fixed16_16 worldX = camera->Left + normalizedX * (camera->Right - camera->Left);
        Fixed16_16 worldY = camera->Bottom + normalizedY * (camera->Top - camera->Bottom);

        return Vector2 { worldX, worldY };
    }

    void Update()
    {
        LastInput = CurrentInput;
    }

    void SetKeyState(uint16_t glfwKey, bool isDown)
    {
        if (keyIndexes[glfwKey] == keyNull) return;

        LastInput[keyIndexes[glfwKey]] = CurrentInput[keyIndexes[glfwKey]];
        CurrentInput[keyIndexes[glfwKey]] = isDown;
    }

    void SetMousePosition(double x, double y)
    {
        mouseX = Fixed16_16::FromFloat<double>(x);
        mouseY = Fixed16_16::FromFloat<double>(y);
    }

private:
    void RegisterInput();
    void RemoveInput();

public:
    std::vector<bool> CurrentInput { };
    std::vector<bool> LastInput { };
    Fixed16_16 mouseX = Fixed16_16(0), mouseY = Fixed16_16(0);

    std::array<uint8_t, GLFW_KEY_LAST + 1> keyIndexes { };
};