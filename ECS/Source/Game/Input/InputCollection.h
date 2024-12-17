#pragma once

#include "Input.h"

#include <cstdint>
#include <vector>
#include <algorithm>

//Saves input like a circular buffer for the most recent frames
class InputCollection
{
public:
    InputCollection(const std::vector<uint16_t>& inputKeys, uint32_t size = 15) : baseInput(Input(inputKeys)), saveCount(size), inputs(size), oldestFrame(0), startIndex(0), frameCount(0), lastCompletedFrame(0)
    {
        if (size == 0)
            throw std::invalid_argument("saveCount must be greater than 0");
    }

    void AddInput(const InputPacket& input)
    {
        assert(input.Frame >= oldestFrame && input.Frame < oldestFrame + saveCount * 2 && "Frame is outside the valid range of the buffer");

        if (HasInput(input.Frame)) return;

        inputs[GetIndex(input.Frame)] = input;

        if (frameCount < saveCount)
        {
            frameCount = std::min(input.Frame + 1, saveCount);
        }
        else if (input.Frame >= oldestFrame + saveCount)
        {
            //Update the oldest frame if the frame is added beyond the current range
            //oldestFrame = input.Frame - saveCount + 1;
            //startIndex = GetIndex(oldestFrame);

            //Calculate how many frames the buffer is advancing
            uint32_t framesAdvanced = input.Frame - (oldestFrame + saveCount - 1);

            //Update the oldestFrame and adjust startIndex accordingly
            oldestFrame = input.Frame - saveCount + 1;
            startIndex = (startIndex + framesAdvanced) % saveCount;
        }

        //Update the last complete frame if the new input fills a gap
        if (input.Frame == lastCompletedFrame + 1)
            ++lastCompletedFrame;
    }

    //Retrieves the input for a specific frame
    Input& GetInput(uint32_t frame)
    {
        if (!HasInput(frame))
        {
            return GetPredictedInput();
        }

        baseInput.Overwrite(inputs[GetIndex(frame)]);
        return baseInput;
    }

    Input& GetPredictedInput()
    {
        //Get last registered input packet
        uint32_t index = GetIndex(lastCompletedFrame);
        baseInput.Overwrite(inputs[index]);
        return baseInput;
    }

    bool HasInput(uint32_t frame) const
    {
        //Check if the frame is within the range of the buffer
        return frame >= oldestFrame && frame < oldestFrame + frameCount && frameCount > 0;
    }

private:
    uint32_t GetIndex(uint32_t frame) const
    {
        return (startIndex + (frame - oldestFrame)) % saveCount;
    }

private:
    Input baseInput;

    uint32_t saveCount;                     //Maximum number of frames to save
    std::vector<InputPacket> inputs;        //Stores the inputs
    uint32_t oldestFrame;                   //Oldest frame where the input it still saved
    uint32_t startIndex;                    //Index of the oldest frame in the inputs, since it is circular
    uint32_t frameCount;                    //Number of frames currently stored in the buffer
    uint32_t lastCompletedFrame;            //The highest frame where all previous inputs are known
};