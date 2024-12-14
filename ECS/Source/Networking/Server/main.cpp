/*#include "Server.h"
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
*/

#include <iostream>
#include <asio.hpp>
#include <array>
#include <string>
#include <ctime>
#include <memory>
#include <functional>

using asio::ip::udp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

class udp_server
{

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;

    // start to receive asynchronously
    void start_receive()
    {
        std::cout<<"Started to receive : "<<socket_.local_endpoint()<<std::endl;
        socket_.async_receive_from(
            asio::buffer(recv_buffer_),
            remote_endpoint_,
            std::bind(&udp_server::handle_receive, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
    }

    void handle_receive(const asio::error_code &error, std::size_t /*bytes_trasfered*/)
    {

        // if no error has occurred
        if (!error || error == asio::error::message_size)
        {

            std::shared_ptr<std::string> message(new std::string(make_daytime_string()));

            socket_.async_send_to(
                asio::buffer(*message),
                remote_endpoint_,
                std::bind(
                    &udp_server::handle_send, this, message, asio::placeholders::error, asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(std::shared_ptr<std::string> /*message*/,
                     const asio::error_code & /*error*/,
                     std::size_t /*bytes_transferred*/)
    {
    }

public:
    udp_server(asio::io_service &io_service) : socket_(io_service, udp::endpoint(udp::v4(), 50000))
    {
        start_receive();
    }
};

int main()
{
    try
    {

        asio::io_service io_service;
        udp_server server(io_service);
        io_service.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}