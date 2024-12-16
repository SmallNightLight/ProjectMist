#pragma once

#include <cstdint>
#include <vector>

#include "Input.h"

typedef std::vector<bool> RawInput;

//Saves input like a circular buffer for the most recent frames
class InputCollection
{
public:
    InputCollection(const std::vector<uint16_t>& inputKeys, uint32_t size = 15) : baseInput(Input(inputKeys)), saveCount(size), inputs(size), oldestFrame(1), startIndex(0)
    {
        if (size == 0)
            throw std::invalid_argument("saveCount must be greater than 0.");
    }

    void AddInput(uint32_t frame, const RawInput& input)
    {
        assert(frame >= oldestFrame && frame < oldestFrame + saveCount && "Frame is outside the valid range of the buffer.");

        uint32_t index = GetIndex(frame);
        inputs[index] = input;

        //Update the oldest frame if we're adding a frame beyond the current range
        if (frame >= oldestFrame + saveCount)
        {
            oldestFrame = frame - saveCount + 1;
            startIndex = GetIndex(oldestFrame);
        }
    }

    //Retrieves the input for a specific frame
    const Input& GetInput(uint32_t frame)
    {
        if (!HasInput(frame))
        {
            throw std::out_of_range("Input for the requested frame not found");
        }

        baseInput.Overwrite(inputs[GetIndex(frame)]);

        return baseInput;
    }

    bool HasInput(uint32_t frame) const
    {
        //Check if the frame is within the range of the buffer
        return frame >= oldestFrame && frame < oldestFrame + saveCount; //TODO: NOT AT BEGINNIGN
    }

private:
    uint32_t GetIndex(uint32_t frame) const
    {
        return (startIndex + (frame - oldestFrame)) % saveCount;
    }

private:
    Input baseInput;

    uint32_t saveCount;                 //Maximum number of frames to save
    std::vector<RawInput> inputs;       //Stores the inputs
    uint32_t oldestFrame;               //oldest frame where the input it still saved
    uint32_t startIndex;                //Index of the oldest frame in the inputs, since it is circular
};