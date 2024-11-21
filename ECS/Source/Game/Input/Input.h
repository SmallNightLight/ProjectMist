#pragma once

#include "BaseInput.h"
#include "InputManager.h"
#include "InputPackage.h"
#include "../../Math/FixedTypes.h"
#include "../../Components/Camera.h"


#include <array>
#include <map>
#include <algorithm>

static constexpr uint16_t keyNull = 255;

//class InputManager;


template <std::size_t KeyCount>
struct Input : BaseInput
{
    Input(const std::array<uint16_t, KeyCount>& inputKeys)
    {
        InputManager::RegisterInput(this);

        keyIndexes.fill(keyNull);

        uint8_t keyIndex = 0;
        for (uint16_t key : inputKeys)
        {
            keyIndexes[key] = keyIndex++;
        }
    }

    /*Input(InputPackage<KeyCount> inputPackage)
    {

    }*/

    ~Input() override
    {
        InputManager::RemoveInput(this);
    }

    /*InputPackage<KeyCount> GetPackage()
    {

    }*/

    //Ket input methods
    bool GetKey(uint16_t glfwKey) override
    {
        return CurrentInput[keyIndexes[glfwKey]];
    }

    bool GetKeyDown(uint16_t glfwKey) override
    {
        return CurrentInput[keyIndexes[glfwKey]] && !LastInput[keyIndexes[glfwKey]];
    }

    bool GetKeyUp(uint16_t glfwKey) override
    {
        return !CurrentInput[keyIndexes[glfwKey]] && LastInput[keyIndexes[glfwKey]];
    }

    //Mouse position methods
    Vector2 GetMousePosition(Camera* camera) const override
    {
        Fixed16_16 normalizedX = Fixed16_16::FromFloat<double>(mouseX) / Fixed16_16::FromFloat<double>(SCREEN_WIDTH);
        Fixed16_16 normalizedY = Fixed16_16::FromFloat<double>(SCREEN_HEIGHT - mouseY) / Fixed16_16::FromFloat<double>(SCREEN_HEIGHT);

        Fixed16_16 worldX = camera->Left + normalizedX * (camera->Right - camera->Left);
        Fixed16_16 worldY = camera->Bottom + normalizedY * (camera->Top - camera->Bottom);

        return Vector2 { worldX, worldY };
    }

    void Update() override
    {
        LastInput = CurrentInput;
    }

    void SetKeyState(uint16_t glfwKey, bool isDown) override
    {
        if (keyIndexes[glfwKey] == keyNull) return;

        LastInput[keyIndexes[glfwKey]] = CurrentInput[keyIndexes[glfwKey]];
        CurrentInput[keyIndexes[glfwKey]] = isDown;
    }

    void SetMousePosition(double x, double y) override
    {
        mouseX = x;
        mouseY = y;
    }

    std::array<bool, KeyCount> CurrentInput { };
    std::array<bool, KeyCount> LastInput { };
    double mouseX = 0, mouseY = 0;

    std::array<uint8_t, GLFW_KEY_LAST + 1> keyIndexes { };
};