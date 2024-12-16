#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>

#include "../Shared/Stream.h"
#include "Client.h"
#include "../../Game/Input/InputCollection.h"
#include "../Shared/InputPacket.h"

class ClientHandler
{
public:
    ClientHandler(const std::string& serverIP, const std::string& serverPort) : client(serverIP, serverPort), running(false), receivedClientID(0) { }

    ~ClientHandler()
    {
        Stop();
    }

    void Start()
    {
        running = true;
        clientThread = std::thread(&ClientHandler::RunClientThread, this);

        if (Serverless)
        {
            AddClient(0);
        }
    }

    void Stop()
    {
        running = false;
        if (clientThread.joinable())
            clientThread.join();
    }

    void SendMessage(const std::vector<uint8_t>& message)
    {
        client.Send(message);
    }

    bool HasNewMessages()
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        return !receivedMessages.empty();
    }

    std::vector<uint8_t> PopMessage()
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        if (receivedMessages.empty())
            return {};

        auto message = std::move(receivedMessages.front());
        receivedMessages.erase(receivedMessages.begin());
        return message;
    }

    void UpdateInputCollections()
    {
        while(HasNewMessages())
        {
            std::vector<uint8_t> message = PopMessage();

            Stream stream = Stream(message);
            ClientID clientID = stream.ReadUInt32();

            if (clientInputs.find(receivedClientID) == clientInputs.end())
            {
                //New client detected
                AddClient(clientID);
            }

            InputPacket packet(stream);


        }
    }

    void UpdateInput(ClientID clientID, const Input& input)
    {

    }

    void AddClient(ClientID clientID)
    {
        clientInputs[clientID] = InputCollection(MaxRollBackFrames * 2);
    }

    uint32_t GetClientID() const
    {
        return receivedClientID;
    }

private:
    void RunClientThread()
    {
        while (running)
        {
            //Wait for client ID during initialization
            if (receivedClientID == 0)
            {
                if (client.HasMessages())
                {
                    auto message = client.PopMessage();
                    if (message.size() == 4)
                    {
                        Stream stream(message);
                        receivedClientID = stream.ReadUInt32();
                        AddClient(receivedClientID);

                        Debug("Received Client ID: ", receivedClientID);
                    }
                }
            }
            else
            {
                //Process messages and push them to the queue
                if (client.HasMessages())
                {
                    std::vector<uint8_t> message = client.PopMessage();
                    std::lock_guard<std::mutex> lock(messageMutex);
                    receivedMessages.push_back(std::move(message));
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    Client client;
    uint32_t receivedClientID;

    std::thread clientThread;
    std::atomic<bool> running;

    std::mutex messageMutex;
    std::vector<std::vector<uint8_t>> receivedMessages;

    std::unordered_map<ClientID, InputCollection> clientInputs;
};