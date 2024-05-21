#include "TCP/Server/Server.h"
#include <iostream>

namespace Network
{
	namespace TCP
	{
		ServerImpl::~ServerImpl()
		{
			Stop();
		}

		bool ServerImpl::Start(unsigned short _port)
		{
			assert(mSocket == INVALID_SOCKET);
			if (mSocket != INVALID_SOCKET)
				Stop();
			mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (mSocket == INVALID_SOCKET)
				return false;

			if (!Network::SetReuseAddr(mSocket) || !Network::SetNonBlocking(mSocket))
			{
				Stop();
				return false;
			}

			sockaddr_in addr;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port = htons(_port);
			addr.sin_family = AF_INET;
			if (bind(mSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
			{
				Stop();
				return false;
			}
			if (listen(mSocket, SOMAXCONN) != 0)
			{
				Stop();
				return false;
			}
			return true;
		}
		void ServerImpl::Stop()
		{
			for (auto& client : mClients)
				client.second.Disconnect();
			mClients.clear();
			if (mSocket != INVALID_SOCKET)
				Network::CloseSocket(mSocket);
			mSocket = INVALID_SOCKET;
		}
		void ServerImpl::Update()
		{
			if (mSocket == INVALID_SOCKET)
				return;

			//!< accept jusqu'� 10 nouveaux clients
			for (int accepted = 0; accepted < 10; ++accepted)
			{
				sockaddr_in addr = { 0 };
				socklen_t addrlen = sizeof(addr);
				SOCKET newClientSocket = accept(mSocket, reinterpret_cast<sockaddr*>(&addr), &addrlen);
				if (newClientSocket == INVALID_SOCKET)
					break;
				Client newClient;
				if (newClient.Init(std::move(newClientSocket), addr))
				{
					auto message = std::make_unique<Messages::Connection>(Messages::Connection::Result::Success);
					message->idFrom = newClient.Id();
					message->from = newClient.DestinationAddress();
					mMessages.push_back(std::move(message));
					mClients[newClient.Id()] = std::move(newClient);
				}
			}

			//!< mise � jour des clients connect�s
			//!< r�ceptionne au plus 1 message par client
			//!< supprime de la liste les clients d�connect�s
			for (auto itClient = mClients.begin(); itClient != mClients.end(); )
			{
				auto& client = itClient->second;
				auto msg = client.Poll();
				if (msg)
				{
					msg->from = itClient->second.DestinationAddress();
					msg->idFrom = itClient->second.Id();
					if (msg->Is<Messages::Disconnection>())
					{
						itClient = mClients.erase(itClient);
					}
					else
						++itClient;
					mMessages.push_back(std::move(msg));
				}
				else
					++itClient;
			}
		}
		bool ServerImpl::SendTo(uint64_t _clientid, const unsigned char* _data, unsigned int _len)
		{
			auto itClient = mClients.find(_clientid);
			return itClient != mClients.end() && itClient->second.Send(_data, _len);
		}

		bool ServerImpl::SendToAll(const unsigned char* _data, unsigned int _len)
		{
			bool ret = true;
			for (auto& client : mClients)
				ret &= client.second.Send(_data, _len);
			return ret;
		}

		std::unique_ptr<Messages::Base> ServerImpl::Poll()
		{
			if (mMessages.empty())
				return nullptr;

			auto msg = std::move(mMessages.front());
			mMessages.pop_front();
			return msg;
		}

		const std::map<uint64_t, Client>& ServerImpl::GetClients() const
		{
			return mClients;
		}

		// permet de d�placer les ressources de "other" vers l'instance de "Server"
		Server::Server(Server&& _other)
			: mImpl(std::move(_other.mImpl))
		{}

		Server& Server::operator=(Server&& _other)
		{
			mImpl = std::move(_other.mImpl);
			return *this;
		}

		// permet de d�marrer le port du serveur
		bool Server::Start(unsigned short _port)
		{
			if (!mImpl)
				mImpl = std::make_unique<ServerImpl>();
			return mImpl && mImpl->Start(_port);
		}

		// permet de stopper le serveur
		void Server::Stop()
		{
			if (mImpl) mImpl->Stop();
		}

		// permet de mettre � jour le serveur donc par exemple une connexion / deconnection ect...
		void Server::Update()
		{
			if (mImpl) mImpl->Update();
		}

		// permet d'envoyer des donn�es � un client sp�cifique gr�ce � son ID ( clientid )
		bool Server::SendTo(uint64_t _clientid, const unsigned char* _data, unsigned int _len)
		{
			return mImpl && mImpl->SendTo(_clientid, _data, _len);
		}

		// permet d'envoyer des donn�es � tous les clients connect�s
		bool Server::SendToAll(const unsigned char* _data, unsigned int _len)
		{
			return mImpl && mImpl->SendToAll(_data, _len);
		}

		// v�rifie s'il y a des messages en attente
		std::unique_ptr<Messages::Base> Server::Poll()
		{
			return mImpl ? mImpl->Poll() : nullptr;
		}

		const std::map<uint64_t, Client>& Server::GetClients() const
		{
			return mImpl->GetClients();
		}
	}
}