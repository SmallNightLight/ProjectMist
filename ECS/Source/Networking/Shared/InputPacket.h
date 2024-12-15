#pragma once

#include <bitset>

#include "Stream.h"

struct InputPackage
{
    InputPackage(u_int32_t frame, const std::vector<bool>& input)
    {
        Frame = frame;
        Input = input;
    }

    InputPackage(Stream& stream)
    {
        Frame = stream.ReadUInt32();

        int inputSize = stream.ReadUInt32();

        Input.resize(inputSize, false);

        for(int i = 0; i < inputSize; i++)
        {
            Input[i] = stream.ReadBool();
        }
    }

    void Serialize(Stream& stream)
    {
        stream.WriteUint32(Frame);

        stream.WriteUint32(Input.size());
        for(bool value : Input)
        {
            stream.WriteBool(value);
        }
    }

    u_int32_t Frame;
    std::vector<bool> Input;
};