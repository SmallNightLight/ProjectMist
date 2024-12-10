#pragma once

class Server
{
    Server(unsigned short localPort)
    {

    }

    template<typename T>
    void SendToClient(const T& message, uint32_t clientID)
    {
        message.Serialize();
    }

    void SendToAllExcept(const std::string& message, uint32_t clientID);
    void SendToAll(const std::string& message);

    void StartReceive()
    {

    }

    void handle_remote_error(const std::error_code error_code, const udp::endpoint remote_endpoint);
    void handle_receive(const std::error_code& error, std::size_t bytes_transferred);
    void handle_send(std::string /*message*/, const std::error_code& /*error*/, std::size_t /*bytes_transferred*/)	{}
    void run_service();

    // Client management
    int32_t get_or_create_client_id(udp::endpoint endpoint);
    void on_client_disconnected(int32_t id);

    void send(const std::string& message, udp::endpoint target);
};
