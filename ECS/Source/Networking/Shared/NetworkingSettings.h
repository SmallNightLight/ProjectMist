#pragma once

#include <asio/ip/udp.hpp>

static constexpr int NetworkBufferSize = 4096;

typedef uint32_t ClientID;
typedef asio::ip::udp::endpoint ClientEndpoint;

static constexpr bool Serverless = true;