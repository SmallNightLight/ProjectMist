#pragma once

#include <asio/ip/udp.hpp>

static constexpr int NetworkBufferSize = 4096;

typedef uint32_t ClientID;
typedef asio::ip::udp::endpoint ClientEndpoint;

enum PacketType
{
    RequestJoinPacket, //Also with empty message
    NewClientPacket,
    RequestGameDataPacket,
    GameDataPacket,
    RequestInputPacket, //For resending input packet, input send by default
    InputPacket
};