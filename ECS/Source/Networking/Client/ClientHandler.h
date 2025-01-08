#pragma once

#include "Client.h"
#include "../Shared/Packet.h"

#include "../../Math/Stream.h"
#include "../../Game/Input/InputCollection.h"
#include "../../Game/GameSettings.h"
#include "../../Game/Worlds/PhysicsWorld.h"

#include <memory>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>

class ClientHandler
{
public:
    ClientHandler(const std::string& serverIP, const std::string& serverPort) : client(serverIP, serverPort), running(false), receivedClientID(0), connected(false) { }

    ~ClientHandler()
    {
        Stop();
    }

    void Start()
    {
        running = true;
        receivedClientID = 0;
        AddClient(0);

        clientThread = std::thread(&ClientHandler::RunClientThread, this);
    }

    void Stop()
    {
        running = false;
        if (clientThread.joinable())
            clientThread.join();
    }

    void Send(const Packet& packet)
    {
        client.Send(packet.Data.GetBuffer());
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

    void ReadMessages(std::shared_ptr<PhysicsWorld> physicsWorld)
    {
        while(HasNewMessages())
        {
            std::vector<uint8_t> message = PopMessage();
            Packet packet = Packet(message);

            switch (packet.Type)
            {
                case NewClientPacket:
                {
                    AddClient(packet.ID);
                    break;
                }
                case RequestGameDataPacket: //Send back the serialized game state
                {
                    Stream gameData = Stream();
                    physicsWorld->Serialize(gameData);
                    Send(Packet(receivedClientID, GameDataPacket, 0, std::move(gameData)));
                    break;
                }
                case GameDataPacket:  //Deserialize the game data
                {
                    physicsWorld->Deserialize(packet.Data);
                    clientInputs.at(receivedClientID).JumpToFrame(physicsWorld->GetCurrentFrame());
                    Debug("Deserialized the game data");
                    break;
                }
                case RequestInputPacket: //Resend an input packet
                {
                    Warning("RequestInputPacket not implemented");
                    break;
                }
                case InputPacket: //Deserialize the input packet from other clients
                {
                    InputData inputData = InputData(std::move(packet.Data));
                    UpdateInput(packet.ID, inputData);
                    break;
                }
                default:
                {
                    Warning("Received unknown packet type ", packet.Type);
                }
            }
        }
    }

    void UpdateInput(ClientID clientID, const InputData& inputData)
    {
        if (clientInputs.find(clientID) == clientInputs.end()) return;

        clientInputs.at(clientID).AddInput(inputData);
    }

    void SendInput(InputData& inputData)
    {
        if (receivedClientID == 0) return;

        Stream stream = Stream();
        inputData.Serialize(stream);
        Packet packet(receivedClientID, InputPacket, inputData.Frame, std::move(stream));
        Send(packet);
    }

    void AddClient(ClientID clientID)
    {
        if (clientInputs.find(receivedClientID) != clientInputs.end())
        {
            Warning("Can not add a client that already exists");
            return;
        }

        clientIDs.insert(clientID);
        clientInputs.emplace(clientID, InputCollection(playerInputKeys, MaxRollBackFrames * 2));
        //lastInputFrame[clientID] =
    }

    void RemoveClient(ClientID clientID)
    {
        if (clientID == receivedClientID)
        {
            Warning("Cannot remove itself");
            return;
        }

        if (clientInputs.find(receivedClientID) == clientInputs.end())
        {
            Warning("Can not remove a client that does not exists");
            return;
        }

        clientIDs.erase(clientID);
        clientInputs.erase(clientID);
        lastInputFrame.erase(clientID);
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

        lastInputFrame[clientID] = frame;
        return &inputCollection.GetInput(frame);
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
        //Connect to server
        client.Send(std::vector<uint8_t> { });

        while (running)
        {
            //Wait for client ID during initialization
            if (receivedClientID == 0)
            {
                if (client.HasMessages())
                {
                    auto message = client.PopMessage();
                    Packet packet(message);

                    receivedClientID = packet.ID;
                    RemoveClient(0);
                    AddClient(receivedClientID);

                    Debug("Received Client ID: ", receivedClientID);
                    connected = true;

                    //Requesting game data
                    Send(Packet(receivedClientID, RequestGameDataPacket));
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

    bool connected;
};