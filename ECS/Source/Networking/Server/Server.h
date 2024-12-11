#pragma once

#include "ServerSettings.h"
#include "../Shared/Log.h"

#include <asio.hpp>
#include <map>

using asio::ip::udp;
typedef udp::endpoint Client;
typedef uint32_t ClientID;

class Server
{
public:
    Server(unsigned short localPort) : socket(io_service, Client(udp::v4(), localPort)), service_thread(&Server::run_service, this), nextClientID(0)
    {
        Info("Starting server on local port ", localPort);
    }

    ~Server()
    {
        io_service.stop();
        service_thread.join();
    }

    template<typename T>
    void SendToClient(const T& message, Client client)
    {
        auto data = message.Serialize();
        socket.send_to(asio::buffer(message), client);
    }

    template<typename T>
    void SendToAllExcept(const T& message, ClientID clientID)
    {
        for (auto client: Clients)
        {
            if (client.first != clientID)
                SendToClient(message, client.second);
        }
    }

    template<typename T>
    void SendToAll(const T& message)
    {
        for (auto client: Clients)
        {
            SendToClient(message, client.second);
        }
    }

    void StartReceive()
    {
        socket.async_receive_from(asio::buffer(recv_buffer), remote_endpoint,
                    [this](std::error_code ec, std::size_t bytes_recvd){ this->handle_receive(ec, bytes_recvd); });
    }

    void HandleError(const std::error_code error_code, const Client client)
    {
        bool found = false;
        int32_t clientID = 0;
        for (const auto& clientPair : Clients)
        {
            if (clientPair.second == client)
            {
                found = true;
                clientID = clientPair.first;
                break;
            }
        }

        if (found == false) return;

        Clients.erase(clientID);
        OnClientDisconnect(client);
    }

    void handle_receive(const std::error_code& error, std::size_t bytes_transferred)
    {

    }

    void handle_send(std::string /*message*/, const std::error_code& /*error*/, std::size_t /*bytes_transferred*/)	{}
    void run_service();

    // Client management
    int32_t get_or_create_client_id(Client endpoint);

    void OnClientDisconnect(Client client)
    {

    }

    void send(const std::string& message, udp::endpoint target);

private:
    asio::io_service io_service;
    udp::socket socket;
    udp::endpoint server_endpoint;
    udp::endpoint remote_endpoint;
    std::array<char, NetworkBufferSize> recv_buffer;
    std::thread service_thread;

    std::map<ClientID, Client> Clients;
    uint32_t nextClientID;
};
