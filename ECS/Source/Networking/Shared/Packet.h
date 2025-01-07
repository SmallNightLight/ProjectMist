#pragma once

#include "../Shared/NetworkingSettings.h"
#include "../../Math/Stream.h"

#include <cstdint>
#include <utility>

struct Packet
{
    //Creates a packet with the bare minimum required data
    Packet(ClientID clientID, PacketType packetType, uint32_t frame = 0) : ID(clientID), Type(packetType), Frame(frame), Data()
    {
        Data.WriteInteger<ClientID>(ID);
        Data.WriteEnum<PacketType, uint8_t>(Type);
        Data.WriteInteger<uint32_t>(Frame);
    }

    //Creates a packet from the provided information and joins the data to the other packet information
    Packet(ClientID clientID, PacketType packetType, uint32_t frame, Stream&& data) : Packet(clientID, packetType, frame)
    {
        Data.Join(std::move(data));
    }

    //Creates a packet from a stream, containing the data
    explicit Packet(const std::vector<uint8_t>& message) : Data(message)
    {
        ID = Data.ReadInteger<ClientID>();
        Type = Data.ReadEnum<PacketType, uint8_t>();
        Frame = Data.ReadInteger<uint32_t>();
    }

    bool VerifyClientID(ClientID clientID)
    {
        return ID == clientID;
    }

    static constexpr uint8_t DefaultSize = 9;

    ClientID ID;
    PacketType Type;
    uint32_t Frame;
    Stream Data;
};