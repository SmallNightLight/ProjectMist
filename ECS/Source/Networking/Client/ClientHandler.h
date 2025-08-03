#pragma once

#include "Client.h"
#include "../Shared/Packet.h"

#include "../../Math/Stream.h"
#include "../../Game/Input/InputCollection.h"
#include "../../ECS/ECSSettings.h"
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
#include <limits>

class ClientHandler
{
public:
    ClientHandler(const std::string& serverIP, const std::string& serverPort) : client(serverIP, serverPort), running(false), receivedClientID(0), connected(false), sendGameData(false) { }

    ~ClientHandler()
    {
        Stop();
    }

    void Start()
    {
        running = true;
        receivedClientID = 0;
        AddClient(0, 0);

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
                case AcceptJoin:
                {
                    if (receivedClientID == 0)
                    {
                        receivedClientID = packet.ID;
                        RemoveClient(0);

                        Debug("Received Client ID: ", receivedClientID);
                        connected = true;

                        AddClient(receivedClientID, packet.Frame);

                        //Requesting game data
                        Send(Packet(receivedClientID, RequestGameDataPacket));
                    }
                    else
                    {
                        Warning("Did not yet receive client ID??");
                    }

                    break;
                }
                case NewClientPacket:
                {
                    AddClient(packet.ID, packet.Frame);
                    break;
                }
                case RequestGameDataPacket: //Mark for later serialization
                {
                    sendGameData = true;
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

        if (clientsWaitingToJoin.size() > 0)
        {
            for (auto newClient : clientsWaitingToJoin)
            {
                AddClient(newClient.first, newClient.second);
            }

            clientsWaitingToJoin.clear();
        }
    }

    ///Send back the serialized game state
    void SendGameData(std::shared_ptr<PhysicsWorld> physicsWorld)
    {
        if (!sendGameData) return;

        Stream gameData = Stream();
        physicsWorld->Serialize(gameData);
        Send(Packet(receivedClientID, GameDataPacket, 0, std::move(gameData)));
        sendGameData = false;
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

    void AddClient(ClientID clientID, uint32_t frame)
    {
        if (clientInputs.find(clientID) != clientInputs.end())
        {
            Warning("Can not add a client that already exists");
            return;
        }

        clientIDs.insert(clientID);
        clientInputs.emplace(clientID, InputCollection(playerInputKeys, frame,MaxRollBackFrames * 2));

        //Add input for the first few frames to avoid missing input
        for(int i = frame; i < frame + 5; ++i)
        {
            InputData input = InputData(i, std::vector<bool>(playerInputKeys.size()), std::vector<bool>(playerInputKeys.size()), 0, 0);
            clientInputs.at(clientID).AddInput(input);
        }
    }

    void RemoveClient(ClientID clientID)
    {
        if (clientID == receivedClientID)
        {
            Warning("Cannot remove itself");
            return;
        }

        if (clientInputs.find(clientID) == clientInputs.end())
        {
            Warning("Can not remove a client that does not exists");
            return;
        }

        clientIDs.erase(clientID);
        clientInputs.erase(clientID);
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
            return &inputCollection.GetPredictedInput(frame);
        }

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

    uint32_t GetRollbacks(uint32_t currentFrame) const
    {
        uint32_t lastConfirmedFrame = GetLastConfirmedFrame();

        if (lastConfirmedFrame >= currentFrame) return 0;

        return currentFrame - lastConfirmedFrame;
    }

    uint32_t GetLastConfirmedFrame() const
    {
        uint32_t lastConfirmedFrame = std::numeric_limits<uint32_t>::max();

        for (auto clientInput : clientInputs)
        {
            uint32_t lastCompletedFrame = clientInput.second.GetLastCompletedFrame();

            if (lastCompletedFrame < lastConfirmedFrame)
            {
                lastConfirmedFrame = lastCompletedFrame;
            }
        }

        return lastConfirmedFrame;
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

                    if(packet.Type == AcceptJoin)
                    {
                        //Add packet to queue so it can be evaluated later
                        receivedMessages.push_back(packet.Data.GetBuffer());

                    }
                    else if (packet.Type == NewClientPacket)
                    {
                        clientsWaitingToJoin[packet.ID] = packet.Frame;
                    }
                    else
                    {
                        receivedMessages.push_back(packet.Data.GetBuffer());
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
    std::unordered_map<ClientID, uint32_t> clientsWaitingToJoin;

    std::set<ClientID> clientIDs { };
    std::unordered_map<ClientID, InputCollection> clientInputs;

    bool connected;
    bool sendGameData;
};