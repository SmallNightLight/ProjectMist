#include "Server.h"
#include "../Shared/Log.h"

#include <iostream>
#include <string>

#include "../Shared/InputPacket.h"

int main()
{
    unsigned short localPort = 50000;
    Server server = Server(localPort);

    //InputPacket p = InputPacket<();

    while (true)
    {

        std::string input;
        std::cin >> input;

        if (input == "exit")
        {
            break;
        }
    }
}