#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>

class Stream
{
public:
    Stream() : currentIndex(0) { }

    Stream(const std::vector<uint8_t>& _buffer) : buffer(_buffer), currentIndex(0) { }

    //Write to stream
    void WriteBool(bool value)
    {
        buffer.push_back(value ? 1 : 0);
    }

    template <typename T>
    void WriteInteger(T value)
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type");

        for (size_t i = 0; i < sizeof(T); ++i)
        {
            buffer.push_back(static_cast<unsigned char>((value >> (i * 8)) & 0xFF));
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

    template <typename T>
    T ReadInteger()
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type");

        if (currentIndex + sizeof(T) > buffer.size())
            throw std::out_of_range("Stream underflow");

        T value = 0;
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            value |= static_cast<T>(static_cast<unsigned char>(buffer[currentIndex + i])) << (i * 8);
        }

        currentIndex += sizeof(T);
        return value;
    }

    std::vector<uint8_t> GetBuffer()
    {
        return buffer;
    }

private:
    std::vector<uint8_t> buffer;
    size_t currentIndex;
};
