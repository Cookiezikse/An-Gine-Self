#pragma once
#include "UDP/SocketsUDP.h"
#include <string>

class ClientUDPStart
{
public:
	ClientUDPStart() {};
	~ClientUDPStart() {};

	void CreateSocketClient();
	void ClientStartUDP(unsigned short _port, std::string _ipAddress, std::string _data);
	void threadClientUDP(unsigned short _port, std::string _ipAddress, std::string _data);
private:
	SOCKET myFirstUdpSocket = INVALID_SOCKET;
	bool alreadySocketCreateClient = false;

	
};