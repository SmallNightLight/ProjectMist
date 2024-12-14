#pragma once

#include "../Shared/NetworkingSettings.h"
#include "../Shared/ThreadedQueue.h"
#include "../Shared/Log.h"

#include <string>
#include <array>
#include <vector>
#include <thread>
#include <asio.hpp>

using asio::ip::udp;

class Client
{
public:
    Client(std::string serverIP, std::string serverPort) : socket(io_service, udp::endpoint(udp::v4(), 0)), service_thread(&Client::RunService, this)
    {
        udp::resolver resolver(io_service);
        udp::resolver::query query(udp::v4(), serverIP, serverPort);
        server_endpoint = *resolver.resolve(query);
        Send(std::vector<uint8_t> { });
    }

    ~Client()
    {
        io_service.stop();
        service_thread.join();
    }

    void Send(const std::vector<uint8_t>& data)
    {
        socket.send_to(asio::buffer(data), server_endpoint);
    }

    bool HasMessages()
    {
        return !incomingMessages.empty();
    }

    std::vector<uint8_t> PopMessage()
    {
        if (incomingMessages.empty())
            throw std::logic_error("No messages to pop");

        return incomingMessages.pop();
    }

private:
    void Receive(const std::error_code& error, std::size_t bytes_transferred)
    {
        if (!error)
        {
            std::vector<uint8_t> message(recv_buffer.data(), recv_buffer.data() + bytes_transferred);
            incomingMessages.push(message);
        }
        else
        {
            Error("Client::Receive: ", error);
        }

        StartReceive();
    }

    void StartReceive()
    {
        socket.async_receive_from(asio::buffer(recv_buffer), remote_endpoint, [this](std::error_code ec, std::size_t bytes_recvd)
        {
            this->Receive(ec, bytes_recvd);
        });
    }

    void RunService()
    {
        StartReceive();

        while (!io_service.stopped())
        {
            try
            {
                io_service.run();
            }
            catch (const std::exception& e)
            {
                Warning("Client: network exception: ", e.what());
            }
            catch (...)
            {
                Error("Unknown exception in client network thread");
            }
        }
    }

private:
    asio::io_context io_service;
    udp::socket socket;
    udp::endpoint server_endpoint;
    udp::endpoint remote_endpoint;
    std::array<uint8_t, NetworkBufferSize> recv_buffer;
    std::thread service_thread;

    ThreadedQueue<std::vector<uint8_t>> incomingMessages;
};
