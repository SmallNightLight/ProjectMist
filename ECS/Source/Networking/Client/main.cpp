#include "Client.h"

#include <iostream>
#include <array>
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
    while(!client.HasMessages()) { }

    auto message = client.PopMessage();
    std::cout << std::string(message.begin(), message.end()) << std::endl;

    return 0;
}