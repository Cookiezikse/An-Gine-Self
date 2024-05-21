#include "UDP/Network/ServerUDP/ServerUDPStart.h"
//#include "UDP/SocketsUDP.h"
#include "UDP/ErrorsUDP.h"

#include <iostream>
#include <string>
#include <thread>

void ServerUDPStart::CreateSocketServer(unsigned short _port)
{
    if (!alreadySocketCreateServer)
    {
        if (!Bousk::Network::Start()) {
            std::cout << "Network lib initialisation error : " << Bousk::Network::ErrorsUDP::Get() << std::endl;
        }

        myFirstUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (myFirstUdpSocket == INVALID_SOCKET) {
            std::cout << "Socket creation error : " << Bousk::Network::ErrorsUDP::Get() << std::endl;
        }

        sockaddr_in addr;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_port);
        addr.sin_family = AF_INET;
        if (bind(myFirstUdpSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            std::cout << "Socket bind error : " << Bousk::Network::ErrorsUDP::Get() << std::endl;
        }
        else
        {
            std::cout << "socket init server" << std::endl;
        }
        alreadySocketCreateServer = true;
        std::cout << "INIT" << std::endl;
    }
}

void ServerUDPStart::threadServerUDP()
{
    while (true)
    {
        char buff[1500] = { 0 };
        sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        int ret = recvfrom(myFirstUdpSocket, buff, 1499, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
        if (ret <= 0) {
            std::cout << "Data reception error : " << Bousk::Network::ErrorsUDP::Get() << ". Closing program." << std::endl;
        }
        std::cout << "Received : " << buff << " from " << Bousk::Network::GetAddress(from) << ":" << Bousk::Network::GetPort(from) << std::endl;
    }
}

void ServerUDPStart::ServerStartUDP(unsigned short _port, std::string ipBuffer)
{
    CreateSocketServer(_port);
    if (myFirstUdpSocket != INVALID_SOCKET)
    {
        if (ipBuffer == "")
        {
            sockaddr_in from;
            socklen_t fromlen = sizeof(from);
            std::cout << "Port : " << Bousk::Network::GetPort(from) << "ouvert" << std::endl;
        }
        else {
            std::thread threadServerUDP(&ServerUDPStart::threadServerUDP, this);
        }
    }
    else
    {
        std::cout << "Erreur lors de la récup du socket Server" << std::endl;
    }

    //Bousk::Network::CloseSocket(myFirstUdpSocket);
    //Bousk::Network::Release();
}