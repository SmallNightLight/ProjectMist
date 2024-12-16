#pragma once

#include <cstdint>
#include <vector>

#include "Input.h"

typedef std::vector<bool> RawInput;

//Saves input like a circular buffer for the most recent frames
class InputCollection
{
public:
    InputCollection(uint32_t size = 15) : saveCount(size), inputs(size), frames(size, 0), startIndex(0), count(0)
    {
        if (size == 0)
            throw std::invalid_argument("saveCount must be greater than 0.");
    }

    void AddInput(uint32_t frame, const Input& input)
    {
        assert(frame != 0 && "Frame can't be 0 - invalid frame");

        //Check if the frame already exists and update it if found
        for (uint32_t i = 0; i < count; ++i)
        {
            uint32_t index = (startIndex + i) % saveCount;
            if (frames[index] == frame)
            {
                //Update input for the frame
                inputs[index] = input;
                return;
            }
        }

        //If the frame does not exist, add it
        uint32_t index = (startIndex + count) % saveCount;

        if (count < saveCount)
        {
            //Increment count if buffer isn't full
            ++count;
        }
        else
        {
            //Overwrite the oldest entry
            startIndex = (startIndex + 1) % saveCount;
        }

        frames[index] = frame;
        inputs[index] = input;
    }

    //Retrieves the input for a specific frame
    Input& GetInput(uint32_t frame)
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            uint32_t index = (startIndex + i) % saveCount;
            if (frames[index] == frame)
                return inputs[index];
        }

        throw std::out_of_range("Input for the requested frame not found.");
    }

    bool HasInput(uint32_t frame) const //TODO: BAD!! optimize
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            uint32_t index = (startIndex + i) % saveCount;
            if (frames[index] == frame)
                return true;
        }
        return false;
    }

private:
    uint32_t saveCount;                 //Maximum number of frames to save
    std::vector<Input> inputs;          //Stores the inputs in a circular manner
    std::vector<uint32_t> frames;       //Stores frame numbers (0 indicates an empty slot)
    uint32_t startIndex;                //Index of the oldest frame
    uint32_t count;                     //Number of frames currently stored
};