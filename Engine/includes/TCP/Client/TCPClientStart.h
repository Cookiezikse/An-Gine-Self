#pragma once

#include "TCP/Sockets.h" // Including necessary headers for TCP communication
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"
#include "TCP/Errors.h"
#include "TCP/StatusMessage.h"
#include "TCP/Server/TCPServerStart.h"

#include <iostream>
#include <thread> // Including thread for multithreading support

class TCPClientStart
{
public:
    /**
     * @brief Construct a new TCPClientStart object
     *
     */
    TCPClientStart() : stopThread(false) {}

    /**
     * @brief Destroy the TCPClientStart object and stops the client
     *
     */
    ~TCPClientStart() {
        StopClient();
    }

    /**
     * @brief Starts the TCP client
     *
     */
    void StartClient();

    /**
     * @brief Stops the TCP client
     *
     */
    void StopClient();

    /**
     * @brief Establishes UDP connection with the server
     *
     * @param _ipAdress IP address of the server
     * @param _port Port number to connect
     */
    void ConnexionClientUDP(std::string _ipAdress, int _port);

    /**
     * @brief Thread function to check the connection status periodically
     *
     * @param _data Data to be sent for checking connection
     */
    void ThreadClientUDPConnexionCheck(std::string _data = "");

    /**
     * @brief Sends data to the connected server
     *
     * @param _data Data to be sent
     * @return std::string Response received from the server
     */
    std::string SendData(std::string _data = "");

    /**
     * @brief Set the connected client status
     *
     * @param _isConnected Connection status (true if connected, false otherwise)
     */
    void SetConnectedClient(bool _isConnected);

    /**
     * @brief Get the connected client status
     *
     * @return true if client is connected, false otherwise
     */
    bool GetConnectedClient() { return isConnected; }

private:
    Network::TCP::Client client; // TCP client instance
    TCPServerStart server; // TCP server instance for handling connections
    std::thread clientThreadConnexionCheck; // Thread for checking connection status
    bool isConnected = false; // Flag to indicate client connection status
    std::atomic<bool> stopThread; // Atomic flag to stop the connection check thread
};