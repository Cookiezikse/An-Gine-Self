#include "TCP/Client/TCPClientStart.h"
#include <mutex>

void TCPClientStart::ThreadClientUDPConnexionCheck(std::string _data) {
	StatusMessage& statusMsg = StatusMessage::GetInstance();
	while (!GetConnectedClient())
	{
		while (auto msg = client.Poll())
		{
			if (msg->Is<Network::Messages::Connection>())
			{
				auto connection = msg->As<Network::Messages::Connection>();
				if (connection->result == Network::Messages::Connection::Result::Success)
				{
					std::cout << "connexion success" << std::endl;
				} else { std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl; }
			}
			else if (msg->Is<Network::Messages::UserData>())
			{
				auto userdata = msg->As<Network::Messages::UserData>();
				std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
				std::cout << "Reponse du serveur : " << reply << std::endl;
				std::cout << ">";
				if (statusMsg.GetStatus() == StatusMessage::Status::Send)
				{
					SendData(_data);
				}
				statusMsg.AddMessage(reply);
			}
			else if (msg->Is<Network::Messages::Disconnection>())
			{
				auto disconnection = msg->As<Network::Messages::Disconnection>();
				std::cout << "Deconnecte : " << static_cast<int>(disconnection->reason) << std::endl;
			}
		}
	}
}

std::string TCPClientStart::SendData(std::string _data)
{
	StatusMessage& statusMsg = StatusMessage::GetInstance();

	if (!client.Send(reinterpret_cast<const unsigned char*>(_data.c_str()), static_cast<unsigned int>(_data.length())))
	{
		std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
	}
	else { statusMsg.SetStatus(StatusMessage::None); }

	return _data;
}

void TCPClientStart::ConnexionClientUDP(std::string ipAdress, int _port)
{
	if (!Network::Start())
	{
		std::cout << "Erreur initialisation WinSock : " << Network::Errors::Get() << std::endl;
	}
	else { std::cout << "initialisation WinSock" << std::endl; }

	if (!client.Connect(ipAdress, _port))
	{
		std::cout << "Impossible de se connecter au serveur [" << "ipAdress: " << ipAdress << ":" << _port << "] : " << Network::Errors::Get() << std::endl;
	}
	else {
		StartClient();
	}
}

void TCPClientStart::SetConnectedClient(bool connected) {
	client.Disconnect();
	Network::Release();
	isConnected = connected;
	StopClient();
}

void TCPClientStart::StopClient() {
	stopThread = true;
	if (clientThreadConnexionCheck.joinable()) {
		clientThreadConnexionCheck.join();
	}
}

void TCPClientStart::StartClient() {
	clientThreadConnexionCheck = std::thread(&TCPClientStart::ThreadClientUDPConnexionCheck, this, "");
}