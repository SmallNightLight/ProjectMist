#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>

class Stream
{
public:
    Stream() : currentIndex(0) { }

    //Write to stream
    void WriteBool(bool value)
    {
        buffer.push_back(value ? 1 : 0);
    }

    void WriteUint32(u_int32_t value)
    {
        for (size_t i = 0; i < sizeof(value); ++i)
        {
            buffer.push_back(static_cast<char>((value >> (i * 8)) & 0xFF));
        }
    }

    //Read from stream
    bool ReadBool()
    {
        if (currentIndex >= buffer.size())
            throw std::out_of_range("Stream underflow");

        bool value = buffer[currentIndex] != 0;
        currentIndex++;
        return value;
    }

    u_int32_t ReadUInt32()
    {
        if (currentIndex + sizeof(u_int32_t) > buffer.size())
            throw std::out_of_range("Stream underflow");

       u_int32_t value = 0;
        for (size_t i = 0; i < sizeof(u_int32_t); ++i)
        {
            value |= (static_cast<u_int32_t>(static_cast<unsigned char>(buffer[currentIndex + i])) << (i * 8));
        }
        currentIndex += sizeof(u_int32_t);
        return value;
    }

    std::vector<char> GetBuffer()
    {
        return buffer;
    }

private:
    std::vector<char> buffer;
    size_t currentIndex;
};
