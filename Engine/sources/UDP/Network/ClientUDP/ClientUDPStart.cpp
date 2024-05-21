#include "UDP/Network/ClientUDP/ClientUDPStart.h"
//#include "UDP/SocketsUDP.h"
#include "UDP/ErrorsUDP.h"

#include <iostream>
#include <string>
#include <thread>

void ClientUDPStart::CreateSocketClient()
{
    if (!alreadySocketCreateClient)
    {
        if (!Bousk::Network::Start())
        {
            std::cout << "Network lib initialisation error : " << Bousk::Network::ErrorsUDP::Get() << std::endl;
        }

        myFirstUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (myFirstUdpSocket == SOCKET_ERROR) {
            std::cout << "Socket creation error : " << Bousk::Network::ErrorsUDP::Get() << std::endl;
        }
        alreadySocketCreateClient = true;
    }
}

void ClientUDPStart::threadClientUDP(unsigned short _port, std::string _ipAddress, std::string _data)
{
    sockaddr_in to = { 0 };
    inet_pton(AF_INET, _ipAddress.c_str(), &to.sin_addr.s_addr);
    to.sin_family = AF_INET;
    to.sin_port = htons(_port);
    while (true)
    {
        std::cout << "Init du message : " << std::endl;
        if (_data == "")
        {
            std::cout << "Connexion au server" << std::endl;
        }
        else {
            std::cout << "SA DOIT EVOYER" << std::endl;
            int ret = sendto(myFirstUdpSocket, _data.data(), static_cast<int>(_data.length()), 0, reinterpret_cast<const sockaddr*>(&to), sizeof(to));
            if (ret <= 0) {
                std::cout << "Data send error : " << Bousk::Network::ErrorsUDP::Get() << ". Closing program." << std::endl;
            }
        }
    }
}

void ClientUDPStart::ClientStartUDP(unsigned short _port, std::string _ipAddress, std::string _data)
{
    CreateSocketClient();
    std::cout << "Port : " << _port << " - Address IP : " << _ipAddress.c_str() << std::endl;
    if (myFirstUdpSocket != INVALID_SOCKET)
    {
        std::thread threadClientUDP(&ClientUDPStart::threadClientUDP, this, std::ref(_port), std::ref(_ipAddress), std::ref(_data));
    }
    else
    {
        std::cout << "erreur lors de la récup du socket" << std::endl;
    }

    //Bousk::Network::CloseSocket(myFirstUdpSocket);
    //Bousk::Network::Release();
}