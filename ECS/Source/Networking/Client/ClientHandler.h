#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>
#include <string>

#include "../Shared/Stream.h"
#include "Client.h"

class ClientHandler
{
public:
    ClientHandler(const std::string& serverIP, const std::string& serverPort) : client(serverIP, serverPort), running(false), clientID(0) { }

    ~ClientHandler()
    {
        Stop();
    }

    void Start()
    {
        running = true;
        clientThread = std::thread(&ClientHandler::RunClientThread, this);
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

    uint32_t GetClientID() const
    {
        return clientID;
    }

private:
    void RunClientThread()
    {
        while (running)
        {
            //Wait for client ID during initialization
            if (clientID == 0)
            {
                if (client.HasMessages())
                {
                    auto message = client.PopMessage();
                    if (message.size() == 4)
                    {
                        Stream stream(message);
                        clientID = stream.ReadUInt32();
                        Debug("Received Client ID: ", clientID);
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
    std::thread clientThread;
    std::atomic<bool> running;

    uint32_t clientID;
    std::mutex messageMutex;
    std::vector<std::vector<uint8_t>> receivedMessages;
};
