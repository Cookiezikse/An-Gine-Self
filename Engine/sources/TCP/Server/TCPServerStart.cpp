#include "TCP/Sockets.h"
#include "TCP/Messages.h"
#include "TCP/Server/TCPServerStart.h"
#include "TCP/Client/TCPClientStart.h"

#include "TCP/Errors.h"

#include <iostream>

void TCPServerStart::ServerThreadFunction(Network::TCP::Server& server) {
	while (true)
	{
		server.Update();
		while (auto msg = server.Poll())
		{
			if (msg->Is<Network::Messages::Connection>()) {
				std::cout << "Connexion de [" << Network::GetAddress(msg->from) << ":" << Network::GetPort(msg->from) << "]" << std::endl;
			}
			else if (msg->Is<Network::Messages::Disconnection>()) {
				std::cout << "Deconnexion de [" << Network::GetAddress(msg->from) << ":" << Network::GetPort(msg->from) << "]" << std::endl;
			}
			else if (msg->As<Network::Messages::UserData>()) {
				auto userdata = msg->As<Network::Messages::UserData>();
				StatusMessage& statusMsg = StatusMessage::GetInstance();
				const auto& clients = server.GetClients();

				std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
				statusMsg.AddMessage(reply);

				for (const auto& entities : clients) {
					uint64_t clientId = entities.first;
					const Network::TCP::Client& client = entities.second;

					const sockaddr_in& address = client.DestinationAddress();
					std::string ipAddress = Network::GetAddress(address);
					unsigned short port = Network::GetPort(address);
					server.SendTo(clientId, userdata->data.data(), reply.size());

					std::cout << "Client ID: " << clientId << ", IP Address: " << ipAddress << ", Port: " << port << " Pseudo :" << reply << std::endl;
				}
			}
		}
	}
	server.Stop();
	Network::Release();
}

void TCPServerStart::StartServer(unsigned short _port, std::string ipAdress)
{
	std::cout << "Active le port : " << _port << std::endl;
	if (!Network::Start())
	{
		std::cout << "Erreur initialisation WinSock : " << Network::Errors::Get() << std::endl;
	}
	else { std::cout << "initialisation WinSock" << std::endl; }

	if (!server.Start(_port))
	{
		std::cout << "Erreur initialisation serveur : " << Network::Errors::Get() << std::endl;
	}
	else {
		std::cout << "initialisation du serveur" << std::endl;
		// Démarre le thread
		serverThread = std::thread(&TCPServerStart::ServerThreadFunction, this, std::ref(server));
	}
}