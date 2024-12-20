#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "FixedTypes.h"

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

    void WriteFixed(Fixed16_16 value)
    {
        WriteInteger<int32_t>(value.raw_value());
    }

    void WriteVector2(Vector2 value)
    {
        WriteInteger<int32_t>(value.X.raw_value());
        WriteInteger<int32_t>(value.Y.raw_value());
    }

    //Writes an enum to the buffer with T being an integer type that can hold all possible values of the enum
    template <typename Enum, typename T>
    void WriteEnum(Enum value)
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type");

        WriteInteger<T>(static_cast<T>(value));
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

    Fixed16_16 ReadFixed()
    {
        return Fixed16_16::from_raw_value(ReadInteger<int32_t>());
    }

    Vector2 ReadVector2()
    {
        return Vector2(Fixed16_16::from_raw_value(ReadInteger<int32_t>()), Fixed16_16::from_raw_value(ReadInteger<int32_t>()));
    }

    //Writes an enum to the buffer with T being an integer type that can hold all possible values of the enum
    template <typename Enum, typename T>
    Enum ReadEnum()
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type");

        return static_cast<Enum>(ReadInteger<T>());
    }

private:
    std::vector<uint8_t> buffer;
    size_t currentIndex;
};