#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>

#include "../../Math/Stream.h"
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

    void Send(const std::vector<uint8_t>& message)
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
            ClientID clientID = stream.ReadInteger<uint32_t>();

            if (clientInputs.find(receivedClientID) == clientInputs.end())
            {
                //New client detected
                AddClient(clientID);
            }

            InputPacket packet(stream);
            UpdateInput(clientID, packet);
        }
    }

    void UpdateInput(ClientID clientID, const InputPacket& inputPacket)
    {
        if (clientInputs.find(clientID) == clientInputs.end()) return;

        clientInputs.at(clientID).AddInput(inputPacket);
    }

    void SendInput(InputPacket& inputPacket)
    {
        if (receivedClientID == 0) return;

        if (inputPacket.Input[0])
            int i = 0;

        Stream stream = Stream();
        stream.WriteInteger<int32_t>(receivedClientID);

        inputPacket.Serialize(stream);
        Send(stream.GetBuffer());
    }

    void AddClient(ClientID clientID)
    {
        clientIDs.insert(clientID);
        clientInputs.emplace(clientID, InputCollection(playerInputKeys, MaxRollBackFrames * 2));
        //lastInputFrame[clientID] =
    }

    ClientID GetClientID() const
    {
        return receivedClientID;
    }

    const std::set<ClientID>& GetAllClientIDs() const
    {
        return clientIDs;
    }

    Input* GetClientInput(ClientID clientID, uint32_t frame)
    {
        if (clientInputs.find(clientID) == clientInputs.end())
        {
            throw std::invalid_argument("clientID not found");
        }

        InputCollection& inputCollection= clientInputs.at(clientID);

        if (inputCollection.NeedsPrediction(frame))
        {
            return &inputCollection.GetPredictedInput();
        }
        else
        {
            lastInputFrame[clientID] = frame;
            return &inputCollection.GetInput(frame);
        }
    }

    std::vector<Input*> GetAllClientInputs(uint32_t frame)
    {
        std::set<ClientID> clientIDSet = GetAllClientIDs();

        std::vector<Input*> result(clientIDSet.size());

        int i = 0;
        for (ClientID clientID : clientIDSet)
        {
            result[i] = GetClientInput(clientID, frame);
            ++i;
        }

        return result;
    }

    void CheckRollbacks()
    {
        uint32_t rollback = 0;

        for (auto clientInput : clientInputs)
        {
            //clientInput.second.
        }
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
                        receivedClientID = stream.ReadInteger<uint32_t>();
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

    std::set<ClientID> clientIDs { };
    std::unordered_map<ClientID, InputCollection> clientInputs;
    std::unordered_map<ClientID, uint32_t> lastInputFrame;
};