#include "Input.h"
#include "InputManager.h"

void Input::RegisterInput()
{
    InputManager::RegisterInput(this);
}

void Input::RemoveInput()
{
    InputManager::RemoveInput(this);
}