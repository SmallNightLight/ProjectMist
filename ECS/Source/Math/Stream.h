#pragma once

#include <bitset>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <sstream>

#include "FixedTypes.h"

class Stream
{
public:
    Stream() : currentIndex(0) { }

    explicit Stream(const std::vector<uint8_t>& _buffer) : buffer(_buffer), currentIndex(0) { }

    Stream(const Stream& other)
    {
        buffer = other.buffer;
        currentIndex = other.currentIndex;
    }

    void Clear()
    {
        buffer.clear();
        currentIndex = 0;
    }

    void ResetIndex()
    {
        currentIndex = 0;
    }

    std::vector<uint8_t> GetBuffer() const
    {
        return buffer;
    }

    void Join(Stream&& other)
    {
        buffer.insert(buffer.end(), other.buffer.begin(), other.buffer.end());
        other.Clear();
    }

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

    template <std::size_t N>
    void WriteBitset(const std::bitset<N>& value)
    {
        //Round up to nearest byte
        size_t numBytes = (N + 7) / 8;

        for (size_t byteIndex = 0; byteIndex < numBytes; ++byteIndex)
        {
            uint8_t byte = 0;
            for (size_t bitIndex = 0; bitIndex < 8 && byteIndex * 8 + bitIndex < N; ++bitIndex)
            {
                byte |= value[byteIndex * 8 + bitIndex] << bitIndex;
            }
            buffer.push_back(byte);
        }
    }

    void WriteStream(const  std::ostringstream& oss)
    {
        std::string serializedData = oss.str();
        uint32_t streamSize = serializedData.length();

        //Write stream size
        WriteInteger<uint32_t>(streamSize);

        uint32_t i = 0;
        for (char c : serializedData)
        {
            WriteInteger<uint8_t>(static_cast<uint8_t>(c));
            ++i;
        }
        assert(streamSize == i && "Stream size does not match written bytes count");
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

    Fixed16_16 ReadFixed()
    {
        return Fixed16_16::from_raw_value(ReadInteger<int32_t>());
    }

    Vector2 ReadVector2()
    {
        Fixed16_16 x = Fixed16_16::from_raw_value(ReadInteger<int32_t>());
        Fixed16_16 y = Fixed16_16::from_raw_value(ReadInteger<int32_t>());
        return Vector2(x, y);
    }

    //Writes an enum to the buffer with T being an integer type that can hold all possible values of the enum
    template <typename Enum, typename T>
    Enum ReadEnum()
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type");

        return static_cast<Enum>(ReadInteger<T>());
    }

    template <std::size_t N>
    std::bitset<N> ReadBitset()
    {
        size_t numBytes = (N + 7) / 8;

        if (currentIndex + numBytes > buffer.size())
            throw std::out_of_range("Stream underflow");

        std::bitset<N> result;
        for (size_t byteIndex = 0; byteIndex < numBytes; ++byteIndex)
        {
            uint8_t byte = buffer[currentIndex + byteIndex];
            for (size_t bitIndex = 0; bitIndex < 8 && (byteIndex * 8 + bitIndex) < N; ++bitIndex)
            {
                result[byteIndex * 8 + bitIndex] = (byte >> bitIndex) & 1;
            }
        }

        currentIndex += numBytes;
        return result;
    }

    std::istringstream ReadStream()
    {
        //Read stream size
        uint32_t streamSize = ReadInteger<uint32_t>();

        std::ostringstream oss;

        for (int i = 0; i < streamSize; ++i)
        {
            oss.put(static_cast<char>(ReadInteger<uint8_t>()));
        }

        std::istringstream iss(oss.str());
        return iss;
    }

private:
    std::vector<uint8_t> buffer;
    size_t currentIndex;
};