#include "TCP/Client/Client.h"

namespace Network
{
	namespace TCP
	{
		bool ConnectionHandler::Connect(SOCKET _sckt, const std::string& _address, unsigned short _port)
		{
			assert(_sckt != INVALID_SOCKET);
			mAddress = _address;
			mPort = _port;
			// permet d'ecouter les differents evenements ( voir en dessous le Message::Connection )
			mFd.fd = _sckt;
			// de base POLLOUT - il verifiera si le descripteur de fichier est pret a ecrire.
			mFd.events = POLLOUT;
			inet_pton(AF_INET, mAddress.c_str(), &mConnectedAddress.sin_addr.s_addr);
			mConnectedAddress.sin_family = AF_INET;
			mConnectedAddress.sin_port = htons(mPort);
			// tente de ce connecter au serveur distant
			if (::connect(_sckt, (const sockaddr*)&mConnectedAddress, sizeof(mConnectedAddress)) != 0)
			{
				int err = Errors::Get();
				if (err != Errors::INPROGRESS && err != Errors::WOULDBLOCK)
					return false;
			}
			return true;
		}
		// Nous utilisons "poll()" pour ne pas bloquer le programme quand une operation TCP est en cours
		std::unique_ptr<Messages::Connection> ConnectionHandler::Poll()
		{
			int res = ::poll(&mFd, 1, 0);
			if (res < 0)
				return std::make_unique<Messages::Connection>(Messages::Connection::Result::Failed);
			else if (res > 0)
			{
				// Descripteur de fichier permet d'identifier de maniere unique chaque fichier ouvert
				if (mFd.revents & POLLOUT)
				{
					// Descripteur de fichier est pret e etre ecrit
					return std::make_unique<Messages::Connection>(Messages::Connection::Result::Success);
				}
				else if (mFd.revents & (POLLHUP | POLLNVAL))
				{
					// Fin de connexion ou descripteur de fichier invalide
					return std::make_unique<Messages::Connection>(Messages::Connection::Result::Failed);
				}
				else if (mFd.revents & POLLERR)
				{
					// Erreur sur le descripteur de fichier
					return std::make_unique<Messages::Connection>(Messages::Connection::Result::Failed);
				}
			}
			//!< action non terminee
			return nullptr;
		}

		void ReceptionHandler::Init(SOCKET _sckt)
		{
			assert(_sckt != INVALID_SOCKET);
			mSckt = _sckt;
			StartHeaderReception();
		}
		// init la reception du Header ( l'en-tete ) en prenant compte de sa taille
		void ReceptionHandler::StartHeaderReception()
		{
			StartReception(HeaderSize, State::Header);
		}
		// fonction appellee une fois que qu'elle e recu le Header complet
		void ReceptionHandler::StartDataReception()
		{
			assert(mBuffer.size() == sizeof(HeaderType));
			HeaderType networkExpectedDataLength;
			memcpy(&networkExpectedDataLength, mBuffer.data(), sizeof(networkExpectedDataLength));
			const auto expectedDataLength = ntohs(networkExpectedDataLength);
			StartReception(expectedDataLength, State::Data);
		}
		void ReceptionHandler::StartReception(unsigned int _expectedDataLength, State _newState)
		{
			mReceived = 0;
			mBuffer.clear();
			mBuffer.resize(_expectedDataLength, 0);
			mState = _newState;
		}
		std::unique_ptr<Messages::Base> ReceptionHandler::Recv()
		{
			assert(mSckt != INVALID_SOCKET);
			int ret = ::recv(mSckt, MissingDataStartBuffer(), MissingDataLength(), 0);
			if (ret > 0)
			{
				mReceived += ret;
				if (mReceived == mBuffer.size())
				{
					if (mState == State::Data)
					{
						// si toutes les donnees attendues sont arrivees alors elle renvoie "Messages::UserData"
						std::unique_ptr<Messages::Base> msg = std::make_unique<Messages::UserData>(std::move(mBuffer));
						StartHeaderReception();
						return msg;
					}
					else
					{
						StartDataReception();
						//!< si jamais les donnees sont deja disponibles elles seront ainsi retournees directement
						return Recv();
					}
				}
				return nullptr;
			}
			else if (ret == 0)
			{
				//!< connexion terminee correctement
				return std::make_unique<Messages::Disconnection>(Messages::Disconnection::Reason::Disconnected);
			}
			else // ret < 0
			{
				//!< traitement d'erreur
				int error = Errors::Get();
				if (error == Errors::WOULDBLOCK || error == Errors::AGAIN)
				{
					return nullptr;
				}
				else
				{
					return std::make_unique<Messages::Disconnection>(Messages::Disconnection::Reason::Lost);
				}
			}
		}

		void SendingHandler::Init(SOCKET _sckt)
		{
			mSocket = _sckt;
			if (mState == State::Header || mState == State::Data)
			{
				// nettoie le tampon d'envoie
				mSendingBuffer.clear();
			}
			mState = State::Idle;
		}

		// permet de mettre des donnees dans la file d'attente pour l'envoie
		bool SendingHandler::Send(const unsigned char* _data, unsigned int _datalen)
		{
			if (_datalen > std::numeric_limits<HeaderType>::max())
			{
				return false;
			}
			mQueueingBuffers.emplace_back(_data, _data + _datalen);
			return true;
		}

		// permet de gerer l'envoie de donnees
		void SendingHandler::Update()
		{
			assert(mSocket != INVALID_SOCKET);
			if (mState == State::Idle && !mQueueingBuffers.empty())
			{
				// si il y a des donnees en attente, elle prepare l'envoie du prochain en-tete
				PrepareNextHeader();
			}
			// tant on peut envoyer des donnees
			while (mState != State::Idle && SendPendingBuffer())
			{
				if (mState == State::Header)
				{
					PrepareNextData();
				}
				else
				{
					if (!mQueueingBuffers.empty())
					{
						PrepareNextHeader();
					}
					else
					{
						mState = State::Idle;
					}
				}
			}
		}

		// permet d'envoyer des donnees qui sont en attente sur le socket
		bool SendingHandler::SendPendingBuffer()
		{
			if (mSendingBuffer.empty())
			{
				return true;
			}

			//!< envoi des donnees restantes du dernier envoi
			int sent = ::send(mSocket, reinterpret_cast<char*>(mSendingBuffer.data()), static_cast<int>(mSendingBuffer.size()), 0);
			if (sent > 0)
			{
				if (sent == mSendingBuffer.size())
				{
					//!< toutes les donnees ont ete envoyees
					mSendingBuffer.clear();
					return true;
				}
				else
				{
					//!< envoi partiel
					memmove(mSendingBuffer.data() + sent, mSendingBuffer.data(), sent);
					mSendingBuffer.erase(mSendingBuffer.cbegin() + sent, mSendingBuffer.cend());
				}
			}
			return false;
		}

		// pr�pare l'envoie de la prochaine en-t�te
		void SendingHandler::PrepareNextHeader()
		{
			assert(!mQueueingBuffers.empty());
			const auto header = static_cast<HeaderType>(mQueueingBuffers.front().size());
			const auto networkHeader = htons(header);
			mSendingBuffer.clear();
			mSendingBuffer.resize(HeaderSize);
			memcpy(mSendingBuffer.data(), &networkHeader, sizeof(HeaderType));
			mState = State::Header;
		}

		// pr�pare l'envoie des prochaines donn�es
		void SendingHandler::PrepareNextData()
		{
			assert(!mQueueingBuffers.empty());
			mSendingBuffer.swap(mQueueingBuffers.front());
			mQueueingBuffers.pop_front();
			mState = State::Data;
		}

		// permet de calculer la taille total des donn�es en attente
		size_t SendingHandler::QueueSize() const
		{
			size_t s = std::accumulate(mQueueingBuffers.cbegin(), mQueueingBuffers.cend(), static_cast<size_t>(0), [](size_t n, const std::vector<unsigned char>& queuedItem) {
				return n + queuedItem.size() + HeaderSize;
				});
			if (mState == State::Data)
				s += mSendingBuffer.size();
			return s;
		}

		ClientImpl::~ClientImpl()
		{
			Disconnect();
		}

		bool ClientImpl::Init(SOCKET&& _sckt, const sockaddr_in& _addr)
		{
			assert(_sckt != INVALID_SOCKET);
			if (_sckt == INVALID_SOCKET)
				return false;

			assert(mState == State::Disconnected);
			assert(mSocket == INVALID_SOCKET);
			if (mSocket != INVALID_SOCKET)
				Disconnect();

			mSocket = _sckt;
			if (!Network::SetNonBlocking(mSocket))
			{
				Disconnect();
				return false;
			}
			OnConnected(_addr);
			return true;
		}

		bool ClientImpl::Connect(const std::string& _ipaddress, unsigned short _port)
		{
			assert(mState == State::Disconnected);
			assert(mSocket == INVALID_SOCKET);
			if (mSocket != INVALID_SOCKET)
			{
				Disconnect();
			}
			mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (mSocket == INVALID_SOCKET)
			{
				return false;
			}
			else if (!Network::SetNonBlocking(mSocket))
			{
				Disconnect();
				return false;
			}
			if (mConnectionHandler.Connect(mSocket, _ipaddress, _port))
			{
				mState = State::Connecting;
				return true;
			}
			return false;
		}

		void ClientImpl::Disconnect()
		{
			if (mSocket != INVALID_SOCKET)
			{
				Network::CloseSocket(mSocket);
			}
			mSocket = INVALID_SOCKET;
			mState = State::Disconnected;
		}

		bool ClientImpl::Send(const unsigned char* _data, unsigned int _len)
		{
			return mSendingHandler.Send(_data, _len);
		}

		std::unique_ptr<Messages::Base> ClientImpl::Poll()
		{
			switch (mState)
			{
				// check si le client ce connecte
			case State::Connecting:
			{
				// si il ce connecte alors "mConnectionHandler.poll()" v�rifie l'�tat de la connexion qui est en cours
				auto msg = mConnectionHandler.Poll();
				if (msg)
				{
					// connexion r�ussite
					if (msg->result == Messages::Connection::Result::Success)
					{
						OnConnected(mConnectionHandler.ConnectedAddress());
					}
					else
					{
						// fermer la connexion car il y a eu une erreur
						Disconnect();
					}
				}
				// on return quand m�me le message
				return msg;
			} break;
			// le client est d�j� connect�
			case State::Connected:
			{
				// appelle "update" pour envoyer les donn�es en attente
				mSendingHandler.Update();
				// recv qui sert � recevoir les messages entrant
				auto msg = mReceivingHandler.Recv();
				if (msg)
				{
					if (msg->Is<Messages::Disconnection>())
					{
						Disconnect();
					}
				}
				return msg;
			} break;
			case State::Disconnected:
			{
			} break;
			}
			return nullptr;
		}

		// connexion r�ussie, init de l'adresse du serveur + l'init des gestionnaires d'envoi et de r�ception
		void ClientImpl::OnConnected(const sockaddr_in& _addr)
		{
			mAddress = _addr;
			mSendingHandler.Init(mSocket);
			mReceivingHandler.Init(mSocket);
			mState = State::Connected;
		}

		// constructeur de d�placement & op�rateur d'affectation de d�placement
		// Ils s'occupent de transferer les ressources ( mImpl ) de l'objet source vers l'objet this
		// Utilisation de std::move() pour �viter les copies co�teuses
		Client::Client(Client&& _other)
			: mImpl(std::move(_other.mImpl))
		{}
		Client& Client::operator=(Client&& _other)
		{
			mImpl = std::move(_other.mImpl);
			return *this;
		}

		// init permet d'initialiser une connexion � un serveur distant
		bool Client::Init(SOCKET&& _sckt, const sockaddr_in& _addr)
		{
			if (!mImpl)
				mImpl = std::make_unique<ClientImpl>();
			return mImpl && mImpl->Init(std::move(_sckt), _addr);
		}

		// connect permet d'�tablir la connexion � un serveur distant
		bool Client::Connect(const std::string& _ipaddress, unsigned short _port)
		{
			if (!mImpl)
				mImpl = std::make_unique<ClientImpl>();
			return mImpl && mImpl->Connect(_ipaddress, _port);
		}

		// disconnect permet de fermer la connexion avec le serveur distant
		void Client::Disconnect() { if (mImpl) mImpl->Disconnect(); mImpl = nullptr; }

		// send permet d'envoyer des donn�es au serveur distant gr�ce � la connexion �tablie
		bool Client::Send(const unsigned char* _data, unsigned int _len) { return mImpl && mImpl->Send(_data, _len); }

		// poll permet de v�rifier si les donn�es venant du serveur distant sont disponibles � recevoir
		// Renvoie "Messages::Base" pour d�terminer le messages � transmettre
		std::unique_ptr<Messages::Base> Client::Poll() { return mImpl ? mImpl->Poll() : nullptr; }

		//permet d'obtenir l'id du client et l'adresse de destination associ�e � la connexion
		uint64_t Client::Id() const { return mImpl ? mImpl->Id() : -1; }
		const sockaddr_in& Client::DestinationAddress() const { static sockaddr_in empty{ 0 }; return mImpl ? mImpl->DestinationAddress() : empty; }
	}
}