#include "UDP/SocketsUDP.h"
#include "UDP/Utils/Utils.h"
#include <iostream>

namespace Bousk
{
	namespace Network
	{
		bool Start()
		{
			// init la bibliothèque Winsock
#ifdef _WIN32
			WSAData wsaData;
			return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
			return true;
#endif
		}
		void Release()
		{
			// libère les ressources
#ifdef _WIN32
			WSACleanup();
#endif
		}
		bool SetNonBlocking(SOCKET socket)
		{
			// permet d'init un socket en mode non bloquant ce qui permet de ne pas bloqué tout le processus et potentiellement renvoyer une erreur
#ifdef _WIN32
			u_long mode = 1;
			return ioctlsocket(socket, FIONBIO, &mode) == 0;
#else
			return fcntl(socket, F_SETFL, O_NONBLOCK) != -1;
#endif
		}
		bool SetReuseAddr(SOCKET socket)
		{
			// permet de config un socket pour ré-utiliser son adresse
#ifdef _WIN32
			UNUSED(socket);
			//int optval = 1;
			//return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&optval), sizeof(optval)) == 0;
			return true;
#else
			int optval = 1;
			return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == 0;
#endif
		}
		void CloseSocket(SOCKET s)
		{
			// permet de fermer le socket
#ifdef _WIN32
			closesocket(s);
#else
			close(s);
#endif
		}
		std::string GetAddress(const sockaddr_in& addr)
		{
			// permet de convertir "sockaddr_in" ( contient une IP ) en chaîne de caractère grâce à "inet_ntop"
			char buff[INET6_ADDRSTRLEN] = { 0 };
			if (auto ret = inet_ntop(addr.sin_family, (void*)&(addr.sin_addr), buff, INET6_ADDRSTRLEN))
			{
				return ret;
			}
			return "";
		}
		unsigned short GetPort(const sockaddr_in& addr)
		{
			// permet d'avoir le port de "sockaddr_in" grâce à "ntohs" ( qui convertit le port  du format réseau au format hôte )
			std::cout << &addr << std::endl;
			return ntohs(addr.sin_port);
		}
	}
}