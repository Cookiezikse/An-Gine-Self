#include "TCP/Sockets.h"
#include "TCP/Utils.h"
#include <iostream>

namespace Network
{
	bool Start()
	{
#ifdef _WIN32
		WSAData wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
		return true;
#endif
	}
	void Release()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
	bool SetNonBlocking(SOCKET _socket)
	{
#ifdef _WIN32
		u_long mode = 1;
		return ioctlsocket(_socket, FIONBIO, &mode) == 0;
#else
		return fcntl(socket, F_SETFL, O_NONBLOCK) != -1;
#endif
	}
	bool SetReuseAddr(SOCKET _socket)
	{
#ifdef _WIN32
		UNUSED(_socket);
		//int optval = 1;
		//return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&optval), sizeof(optval)) == 0;
		return true;
#else
		int optval = 1;
		return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == 0;
#endif
	}
	void CloseSocket(SOCKET _s)
	{
#ifdef _WIN32
		closesocket(_s);
#else
		close(_s);
#endif
	}
	std::string GetAddress(const sockaddr_in& _addr)
	{
		char buff[INET6_ADDRSTRLEN] = { 0 };
		if (auto ret = inet_ntop(_addr.sin_family, (void*)&(_addr.sin_addr), buff, INET6_ADDRSTRLEN))
		{
			return ret;
		}
		return "";
	}
	unsigned short GetPort(const sockaddr_in& _addr)
	{
		std::cout << &_addr << std::endl;
		return ntohs(_addr.sin_port);
	}
}