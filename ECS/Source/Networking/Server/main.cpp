#include "Server.h"
#include "../Shared/Log.h"

#include <iostream>
#include <string>

#include "../Shared/InputPacket.h"

int main()
{
    Server server = Server();

    //InputPackage p = InputPackage<();

    while (true)
    {

        std::string input;
        std::cin >> input;
        Debug(input);
    }
}
