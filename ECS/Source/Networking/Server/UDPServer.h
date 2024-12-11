#pragma once

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <thread>

#include <asio.hpp>

#include "ServerSettings.h"

using asio::ip::udp;
using asio::error_code;

class udp_server;

struct udp_session : std::enable_shared_from_this<udp_session> {

    udp_session(udp_server* server) : server_(server) {}

    void handle_request(const error_code& error);

    void handle_sent(const error_code& ec, std::size_t) {
        // here response has been sent
        if (ec) {
            std::cout << "Error sending response to " << remote_endpoint_ << ": " << ec.message() << "\n";
        }
    }

    udp::endpoint remote_endpoint_;
    std::array<char, NetworkBufferSize> recv_buffer_ { };
    std::string message;
    udp_server* server_;
};

class udp_server
{
    typedef std::shared_ptr<udp_session> shared_session;
  public:
    udp_server(asio::io_service& io_service)
        : socket_(io_service, udp::endpoint(udp::v4(), 1313)),
          strand_(io_service)
    {
        receive_session();
    }

  private:
    void receive_session()
    {
        // our session to hold the buffer + endpoint
        auto session = std::make_shared<udp_session>(this);

        socket_.async_receive_from(
                asio::buffer(session->recv_buffer_),
                session->remote_endpoint_,
                strand_.wrap(
                    std::bind(&udp_server::handle_receive, this,
                        session, // keep-alive of buffer/endpoint
                        std::placeholders::_1,
                        std::placeholders::_2)));

    }

    void handle_receive(shared_session session, const error_code& ec, std::size_t /*bytes_transferred*/) {
        // now, handle the current session on any available pool thread
        asio::post(socket_.get_executor(), std::bind(&udp_session::handle_request, session, ec));

        // immediately accept new datagrams
        receive_session();
    }

    void enqueue_response(shared_session const& session) {
        // async_send_to() is not thread-safe, so use a strand.
        asio::post(socket_.get_executor(),
            strand_.wrap(std::bind(&udp_server::enqueue_response_strand, this, session)));
    }

    void enqueue_response_strand(shared_session const& session) {
        socket_.async_send_to(asio::buffer(session->message), session->remote_endpoint_,
                strand_.wrap(bind(&udp_session::handle_sent,
                        session, // keep-alive of buffer/endpoint
                        asio::placeholders::error,
                        asio::placeholders::bytes_transferred)));
    }

    udp::socket socket_;
    asio::io_context::strand strand_;

    friend struct udp_session;
};

inline void udp_session::handle_request(const error_code& error)
{
    if (!error || error == asio::error::message_size)
    {
        message = make_daytime_string(); // let's assume this might be slow
        message += "\n";

        // let the server coordinate actual IO
        server_->enqueue_response(shared_from_this());
    }
}

int main()
{
    try {
        asio::io_service io_service;
        udp_server server(io_service);

        asio::detail::thread_group group;
        for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i)
            group.create_thread(bind(&asio::io_service::run, boost::ref(io_service)));

        group.join_all();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}