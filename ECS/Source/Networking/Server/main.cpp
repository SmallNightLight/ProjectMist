#include "Server.h"
#include "../Shared/Log.h"

#include <iostream>
#include <string>

int main()
{
    unsigned short localPort = 50000;
    Server server(localPort);

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