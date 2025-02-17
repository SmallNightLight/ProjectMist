#pragma once

#include "../Shared/Packet.h"
#include "../Shared/ThreadedQueue.h"
#include "../Shared/NetworkingSettings.h"
#include "../Shared/Log.h"
#include "../../Math/Stream.h"

#include <vector>
#include <set>
#include <unordered_map>
#include <asio.hpp>
#include <thread>

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
        socket.cancel();
        io_service.stop();
        service_thread.join();
    }

    void SendToClient(const Packet& packet, ClientID clientID, uint32_t delayMilliseconds = 0)
    {
        try
        {
            if (delayMilliseconds == 0)
            {
                Debug("Sending packet: (Bytes: ", packet.Data.GetBuffer().size(), ", Receiver: ", clientID, ", Frame: ", packet.Frame, ")");
                Send(packet.Data.GetBuffer(), Clients.at(clientID));
            }
           else
           {
               //Capture the necessary data for sending in a lambda
               auto sendFunction = [this, packet, clientID]()
               {
                   try
                   {
                       Send(packet.Data.GetBuffer(), Clients.at(clientID));
                       Debug("Sent packet: (Bytes: ", packet.Data.GetBuffer().size(), ", Receiver: ", clientID, ", Frame: ", packet.Frame, ")");
                   }
                   catch (const std::out_of_range&)
                   {
                       Error("Cannot send packet to client, unknown client ID ", clientID);
                   }
               };

               //Use a thread to introduce a delay before sending
               std::thread([sendFunction, delayMilliseconds]()
               {
                   std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
                   sendFunction();
               }).detach();
           }
        }
        catch (const std::out_of_range&)
        {
            Error("Cannot send packet to client, unknown client ID ", clientID);
        }
    }

    void SendToAllExcept(const Packet& packet, ClientID clientID, uint32_t delayMilliseconds = 0)
    {
        for (auto client: Clients)
        {
            if (client.first != clientID)
                SendToClient(packet, client.first, delayMilliseconds);
        }
    }

    void SendToAll(const Packet& packet, uint32_t delayMilliseconds = 0)
    {
        for (auto client: Clients)
        {
            SendToClient(packet, client.first, delayMilliseconds);
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
                ClientID clientID;

                if (message.size() == 0)
                {
                    //Accept client to join
                    GetClientID(remote_endpoint, clientID, true);

                    //Send joining of client to other clients
                    for (auto client: Clients)
                    {
                        SendToClient(Packet(clientID, clientID == client.first ? AcceptJoin : NewClientPacket, ClientCurrentFrames[clientID]), client.first);
                    }

                    //Send other existing clients to client
                    for (auto client: Clients)
                    {
                        if (client.first == clientID) continue;

                        SendToClient(Packet(client.first, NewClientPacket, ClientCurrentFrames[client.first]), clientID);
                    }
                }
                else if (message.size() >= Packet::DefaultSize)
                {
                    Packet packet = Packet(message);

                    if (GetClientID(remote_endpoint, clientID) && packet.VerifyClientID(clientID))
                    {
                        switch (packet.Type)
                        {
                            case RequestJoinPacket: //Accept client to join
                            {
                                Warning("Not implemented, join with empty packet");
                            }
                            case RequestGameDataPacket: //Continue request to all other clients
                            {
                                if (Clients.size() > 1) //ToDO: when reconnecting on the same ip it might register even when in practice no clients are active (no heartbeats)
                                {
                                    clientsWaitingForGameData.insert(clientID);
                                    SendToAllExcept(packet, clientID);
                                }
                                break;
                            }
                            case GameDataPacket:  //Continue game data to the client requesting it
                            {
                                for(ClientID receivingClient : clientsWaitingForGameData)
                                {
                                    SendToClient(packet, receivingClient);
                                }
                                clientsWaitingForGameData.clear();
                                break;
                            }
                            case RequestInputPacket: //Resend an input packet
                            {
                                Warning("RequestInputPacket not implemented");
                                break;
                            }
                            case InputPacket: //Continue the input packet to other clients
                            {
                                //Update information on the frame
                                if (ClientCurrentFrames.find(clientID) != ClientCurrentFrames.end() &&  ClientCurrentFrames.at(clientID) < packet.Frame)
                                    ClientCurrentFrames[clientID] = packet.Frame;

                                SendToAllExcept(packet, clientID, InputPacketDelay);
                                break;
                            }
                            default:
                            {
                                Warning("Received unknown packet type ", packet.Type);
                            }
                        }
                    }
                    else
                    {
                        //ToDO: Let the client know that it could not process the packet
                        Warning("Received message with invalid client ID: ", clientID);
                    }
                }
                else
                {
                    //ToDO: Let the client know that it could not process the packet
                    Warning("Received message with invalid size");
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
    }

    void HandleError(const std::error_code error_code, const ClientEndpoint& client)
    {
        Warning("Error while receiving a packet", error_code.message());

        if (!ClientExists(client)) return;

        ClientID clientID;
        GetClientID(client, clientID);

        Clients.erase(clientID);
        Endpoints.erase(client);
        ClientCurrentFrames.erase(clientID);
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

    bool ClientExists(const ClientEndpoint& endpoint)
    {
        return Endpoints.find(endpoint) != Endpoints.end();
    }

    //Returns the client ID, and creates a new one, when it doesn't have an existing ID assigned
    bool GetClientID(const ClientEndpoint& endpoint, ClientID& clientID, bool addClient = false)
    {
        if (ClientExists(endpoint))
        {
            clientID = Endpoints.at(endpoint);
            return true;
        }

        if (addClient)
        {
            clientID = AddClient(endpoint);
            return true;
        }

        return false;
    }

    ClientID AddClient(const ClientEndpoint& endpoint)
    {
        uint32_t frame = GetGameFrame();

        Clients[++nextClientID] = endpoint;
        Endpoints[endpoint] = nextClientID;
        ClientCurrentFrames[nextClientID] = frame;

        Debug("Accepted new client: ", nextClientID);
        return nextClientID;
    }

    uint32_t GetGameFrame() const
    {
        uint32_t frame = 0;

        for (auto clientFrame : ClientCurrentFrames)
        {
            if (clientFrame.second > frame)
                frame = clientFrame.second;
        }

        return frame;
    }

    size_t GetClientCount() const
    {
        return Clients.size();
    }

    void OnClientDisconnect(ClientID clientID)
    {
        Debug("Client disconnected: ", clientID);

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

    std::unordered_map<ClientID, ClientEndpoint> Clients;
    std::unordered_map<ClientEndpoint, ClientID> Endpoints;
    std::unordered_map<ClientID, uint32_t> ClientCurrentFrames;
    ClientID nextClientID;

    std::set<ClientID> clientsWaitingForGameData;

    ThreadedQueue<std::vector<uint8_t>> incomingMessages;
};