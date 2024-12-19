#include "Client.h"
#include "../../Math/Stream.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <asio.hpp>

using asio::ip::udp;

int main(int argc, char* argv[])
{
    std::string ip = "127.0.0.1";
    std::string port = "50000";

    if (argc > 1)
        ip = argv[1];

    if (argc > 2)
        port = argv[2];

    std::cout << "Starting client with endpoint " << ip << ": " << port << std::endl;

    Client client(ip, port);

    uint32_t clientID = 0;

    while (clientID == 0)
    {
        if (client.HasMessages())
        {
            std::vector<uint8_t> message = client.PopMessage();

            if (message.size() != 4) continue;

            Stream stream = Stream(message);
            clientID = stream.ReadInteger<uint32_t>();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while(true)
    {
        std::string input;
        std::cin >> input;

        if (input == "exit") break;

        std::vector<uint8_t> clientIDBytes(4);
        std::memcpy(clientIDBytes.data(), &clientID, 4);
        client.Send(clientIDBytes);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (client.HasMessages())
        {
            auto message = client.PopMessage();

            for (auto byte : message)
            {
                std::cout << static_cast<int>(byte) << " ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}