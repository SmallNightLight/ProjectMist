#pragma once

#include "../Shared/Stream.h"
#include "../Shared/ThreadedQueue.h"
#include "../Shared/NetworkingSettings.h"
#include "../Shared/Log.h"

#include <string>
#include <vector>
#include <map>
#include <asio.hpp>

using asio::ip::udp;

class Server
{
public:
    Server(unsigned short localPort) : socket(io_service, ClientEndpoint(udp::v4(), localPort)), service_thread(&Server::RunService, this), nextClientID(0)
    {
        Info("Starting server on local port ", localPort);
    }

    ~Server()
    {
        io_service.stop();
        service_thread.join();
    }

    void SendToClient(const std::vector<uint8_t>& message, ClientID clientID)
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

    void SendToAllExcept(const std::vector<uint8_t>& message, ClientID clientID)
    {
        for (auto client: Clients)
        {
            if (client.first != clientID)
                SendToClient(message, client.first);
        }
    }

    void SendToAll(const std::vector<uint8_t>& message)
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
                Stream stream = Stream(message);

                if (message.size() == 0)
                {
                    //Accept client to join

                    ClientID clientID = GetClientID(remote_endpoint);
                    std::vector<uint8_t> clientIDBytes(4);
                    std::memcpy(clientIDBytes.data(), &clientID, 4);

                    SendToAll(clientIDBytes);
                }
                else if (ClientID clientID = stream.ReadInteger<uint32_t>(); VerifyClientID(remote_endpoint, clientID))
                {
                    SendToAll(message);
                    //if (!message.empty())
                    //    incomingMessages.push(message);
                }
                else
                {
                    Warning("Received message with invalid client ID: ", clientID);
                    throw std::runtime_error("Message has invalid client ID");
                }
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

    void Send(const std::vector<uint8_t>& message, const ClientEndpoint& client)
    {
        auto data = std::make_shared<std::vector<uint8_t>>(message);

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

    void HandleError(const std::error_code error_code, const ClientEndpoint& client) //TODO: INEFFICIENT, like getID
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
    ClientID GetClientID(const ClientEndpoint& endpoint)
    {
        for (const auto& client : Clients)
            if (client.second == endpoint)
                return client.first;

        Clients[++nextClientID] = endpoint;
        Debug("Accepted new client: ", nextClientID);
        return nextClientID;
    }

    bool VerifyClientID(const ClientEndpoint& client, ClientID expectedID)
    {
        return Clients.find(expectedID) != Clients.end() && Clients[expectedID] == client;
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
    ClientEndpoint server_endpoint;
    ClientEndpoint remote_endpoint;
    std::array<char, NetworkBufferSize> recv_buffer { };
    std::thread service_thread;

    std::map<ClientID, ClientEndpoint> Clients;
    ClientID nextClientID;

    ThreadedQueue<std::vector<uint8_t>> incomingMessages;
};