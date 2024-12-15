#pragma once

#include "../Shared/ThreadedQueue.h"
#include "../Shared/NetworkingSettings.h"
#include "../Shared/Log.h"

#include <vector>
#include <map>
#include <asio.hpp>

using asio::ip::udp;
typedef udp::endpoint Client;
typedef uint32_t ClientID;

class Server
{
public:
    Server(unsigned short localPort) : socket(io_service, Client(udp::v4(), localPort)), service_thread(&Server::RunService, this), nextClientID(0)
    {
        Info("Starting server on local port ", localPort);
    }

    ~Server()
    {
        io_service.stop();
        service_thread.join();
    }

    template<typename T>
    void SendToClient(const T& message, ClientID clientID)
    {
        try
        {
            Send(message, Clients.at(clientID));
        }
        catch (const std::out_of_range&)
        {
            Error(std::string(__FUNCTION__) + ": Unknown client ID ", clientID); //TODO;: CHECK FOR __FUNC__
        }
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
            SendToClient(message, client.first);
        }
    }

    std::vector<uint8_t> PopMessage()
    {
        return incomingMessages.pop();
    }

    bool HasMessages()
    {
        return !incomingMessages.empty();
    }

private:
    void StartReceive()
    {
        socket.async_receive_from(asio::buffer(recv_buffer), remote_endpoint, [this](std::error_code ec, std::size_t byteCount)
        {
            this->HandleReceive(ec, byteCount);
        });
    }

    void HandleReceive(const std::error_code& error, std::size_t bytes_transferred)
    {
        if (!error)
        {
            try
            {
                std::vector<uint8_t> message = std::vector<uint8_t>(recv_buffer.data(), recv_buffer.data() + bytes_transferred);

                ClientID clientID = GetClientID(remote_endpoint);

                std::string response = "Received message from some client";
                SendToAll<std::vector<uint8_t>>(std::vector<uint8_t>(response.begin(), response.end()));
                //SendToAll(message);

                //if (!message.empty())
                //    incomingMessages.push(message);
            }
            catch (const std::exception& exception)
            {
                Error("HandleReceive: Error parsing incoming message:", exception.what());
            }
            catch (...)
            {
                Error("HandleReceive: Unknown error while parsing incoming message");
            }
        }
        else
        {
            Error("HandleReceive: error: ", error.message(), " while receiving from address ", remote_endpoint);
            HandleError(error, remote_endpoint);
        }

        StartReceive();
    }

    template<typename T>
    void Send(const T& message, Client client)
    {
        auto data = std::make_shared<std::vector<uint8_t>>(message); //.Serialize()

        socket.async_send_to(asio::buffer(*data), client, [this, data](const std::error_code& error, std::size_t bytes_transferred)
        {
            HandleSend(error, bytes_transferred);
        });

        //socket.send_to(asio::buffer(message), client); - blocking send
    }

    void HandleSend(const std::error_code& error, std::size_t bytes_transferred)
    {
        if (error)
        {
            Error("Send failed with error: ", error.message());
        }
        else
        {
            Debug("Successfully sent ", bytes_transferred, " bytes");
        }
    }

    void HandleError(const std::error_code error_code, const Client& client) //TODO: INEFFICIENT, like getID
    {
        bool found = false;
        ClientID clientID = 0;

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
        OnClientDisconnect(clientID);
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
            catch (const std::exception& exception)
            {
                Warning("Client: network exception: ", exception.what());
            }
            catch (...)
            {
                Error("Unknown exception in client network thread");
            }
        }

        Debug("Server network thread stopped");
    }

    //Returns the client ID, and creates a new one, when it doesn't have an existing ID assigned
    ClientID GetClientID(Client endpoint)
    {
        for (const auto& client : Clients)
            if (client.second == endpoint)
                return client.first;

        Clients[++nextClientID] = endpoint;
        Debug("Accepted new client: ", nextClientID);
        return nextClientID;
    }

    size_t GetClientCount()
    {
        return Clients.size();
    }

    void OnClientDisconnect(ClientID clientID)
    {
        Debug("Client disconnected: " + clientID);

        for (auto& handler : OnDisconnectHandlers)
            if (handler)
                handler(clientID);
    }

public:
    std::vector<std::function<void(ClientID)>> OnDisconnectHandlers;

private:
    asio::io_service io_service;
    udp::socket socket;
    Client server_endpoint;
    Client remote_endpoint;
    std::array<char, NetworkBufferSize> recv_buffer { };
    std::thread service_thread;

    std::map<ClientID, Client> Clients;
    ClientID nextClientID;

    ThreadedQueue<std::vector<uint8_t>> incomingMessages;
};