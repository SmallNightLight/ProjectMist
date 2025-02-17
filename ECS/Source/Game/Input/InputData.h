#pragma once

#include "../../Math/Stream.h"

#include <cstdint>
#include <cassert>

struct InputData
{
    InputData() : Frame(0), MouseX(0), MouseY(0) { }

    InputData(uint32_t frame, const std::vector<bool>& input, const std::vector<bool>& lastInput, int32_t mouseX, int32_t mouseY)
    {
        Frame = frame;
        Input = input;
        LastInput = lastInput;
        MouseX = mouseX;
        MouseY = mouseY;
    }

    InputData(Stream&& stream)
    {
        //Read frame
        Frame = stream.ReadInteger<uint32_t>();

        //Read mouse position
        MouseX = stream.ReadInteger<int32_t>();
        MouseY = stream.ReadInteger<int32_t>();

        //Read key state
        int inputSize = stream.ReadInteger<uint32_t>();

        Input.resize(inputSize, false);

        for(int i = 0; i < inputSize; i++)
        {
            Input[i] = stream.ReadBool();
        }

        //Read last key state
        LastInput.resize(inputSize, false);

        for(int i = 0; i < inputSize; i++)
        {
            LastInput[i] = stream.ReadBool();
        }
    }

    InputData CopyForNextFrame(uint32_t frame) const
    {
        return InputData(frame, Input, Input, MouseX, MouseY);
    }

    void Serialize(Stream& stream)
    {
        assert(Input.size() == LastInput.size() && "Size of Input and LastInput needs to match");

        //Write frame
        stream.WriteInteger<uint32_t>(Frame);

        //Write mouse position
        stream.WriteInteger<int32_t>(MouseX);
        stream.WriteInteger<int32_t>(MouseY);

        //Write key state
        stream.WriteInteger<uint32_t>(Input.size());

        for(bool value : Input)
        {
            stream.WriteBool(value);
        }

        //Write last key state
        for(bool value : LastInput)
        {
            stream.WriteBool(value);
        }
    }

    uint32_t Frame;
    std::vector<bool> Input;
    std::vector<bool> LastInput;
    std::int32_t MouseX, MouseY;
};