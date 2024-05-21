#pragma once

#include "TCP/Server/Server.h" // Including necessary headers for TCP server
#include "TCP/StatusMessage.h"

#include <thread> // Including thread for multithreading support

class TCPServerStart
{
public:
    /**
     * @brief Starts the TCP server
     *
     * @param _port Port number to listen on
     * @param _ipAdress IP address to bind the server
     */
    void StartServer(unsigned short _port, std::string _ipAdress);

private:
    /**
     * @brief Thread function for running the server
     *
     * @param _server Reference to the server instance
     */
    void ServerThreadFunction(Network::TCP::Server& _server);

    Network::TCP::Server server; // TCP server instance
    std::thread serverThread; // Thread for running the server
};