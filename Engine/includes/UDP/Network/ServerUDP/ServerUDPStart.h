#pragma once
#include "UDP/SocketsUDP.h"
#include <string>

class ServerUDPStart
{
public:
    ServerUDPStart() {}
    ~ServerUDPStart() {}

    void CreateSocketServer(unsigned short _port);
    void ServerStartUDP(unsigned short _port, std::string ipBuffer);
    void threadServerUDP();

private:
    SOCKET myFirstUdpSocket = INVALID_SOCKET;
    bool alreadySocketCreateServer = false;

    
};