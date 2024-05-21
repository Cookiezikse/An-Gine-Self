#include "UDP/ClientUDP.h"
#include "UDP/DistantClient.h"
#include "UDP/SocketsUDP.h"
#include "UDP/ErrorsUDP.h"

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			void ClientUDP::SetTimeout(std::chrono::milliseconds timeout)
			{
				DistantClient::SetTimeout(timeout);
			}
			std::chrono::milliseconds ClientUDP::GetTimeout()
			{
				return DistantClient::GetTimeout();
			}

			ClientUDP::ClientUDP()
			{}

			ClientUDP::~ClientUDP()
			{
				release();
			}

			// permet de créer un socket qui sera associé au port spécifié par l'utilisateur
			// le socket est config en mode non bloquant ( "SetNonBlocking()" )
			bool ClientUDP::init(uint16_t port)
			{
				release();
				mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (mSocket == INVALID_SOCKET)
					return false;

				sockaddr_in addr;
				addr.sin_addr.s_addr = INADDR_ANY;
				addr.sin_port = htons(port);
				addr.sin_family = AF_INET;
				int res = bind(mSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
				if (res != 0)
					return false;
				if (!Network::SetNonBlocking(mSocket))
					return false;

				return true;
			}

			void ClientUDP::release()
			{

				if (mSocket != INVALID_SOCKET)
					CloseSocket(mSocket);
				mSocket = INVALID_SOCKET;
				{
					OperationsLock lock(mOperationsLock);
					mPendingOperations.clear();
				}
				{
					MessagesLock lock(mMessagesLock);
					mMessages.clear();
				}
				mClients.clear();
				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					mInterruptedClients.clear();
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
			}

			void ClientUDP::connect(const Address& addr)
			{
				assert(addr.isValid());
				OperationsLock lock(mOperationsLock);
				mPendingOperations.push_back(Operation::Connect(addr));
			}

			void ClientUDP::disconnect(const Address& addr)
			{
				assert(addr.isValid());
				OperationsLock lock(mOperationsLock);
				mPendingOperations.push_back(Operation::Disconnect(addr));
			}

			// permet d'envoyer des données à une adresse spécifique
			void ClientUDP::sendTo(const Address& target, std::vector<uint8>&& data, const uint32 channelIndex)
			{
				assert(target.isValid());
				OperationsLock lock(mOperationsLock);
				mPendingOperations.push_back(Operation::SendTo(target, std::move(data), channelIndex));
			}

			void ClientUDP::processSend()
			{
				// Process pending operations
				std::vector<Operation> operations;
				{
					OperationsLock lock(mOperationsLock);
					operations.swap(mPendingOperations);
				}
				for (Operation& op : operations)
				{
					switch (op.mType)
					{
					case Operation::Type::Connect:
					{
						if (auto client = getClient(op.mTarget, true))
							client->connect();
					} break;
					case Operation::Type::SendTo:
					{
						if (auto client = getClient(op.mTarget, true))
							client->send(std::move(op.mData), op.mChannel);
					} break;
					case Operation::Type::Disconnect:
					{
						if (auto client = getClient(op.mTarget))
							client->disconnect();
					} break;
					}
				}

				// Do send data to clients
				for (auto& client : mClients)
					client->processSend();

				// Remove disconnected clients
				const auto clientsToRemove = std::remove_if(mClients.begin(), mClients.end(), [](const std::unique_ptr<DistantClient>& client) { return client->isDisconnected(); });
#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
				// Make sure no interrupted clients have been removed : interrupted clients should resume before disconnecting
				for (auto clientToRemove = clientsToRemove; clientToRemove != mClients.end(); ++clientToRemove)
				{
					const size_t erased = mInterruptedClients.erase(clientToRemove->get());
					assert(erased == 0);
				}
#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
				mClients.erase(clientsToRemove, mClients.end());
			}

#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
			void ClientUDP::onClientInterrupted(const DistantClient* client)
			{
				mInterruptedClients.insert(client);
			}
			void ClientUDP::onClientResumed(const DistantClient* client)
			{
				mInterruptedClients.erase(client);
			}
			bool ClientUDP::isInterruptionCulprit(const DistantClient* client) const
			{
				return mInterruptedClients.size() == 1 && *(mInterruptedClients.begin()) == client;
			}
#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED

			// permet de récupérer les datagrammes sur le socket UDP
			// for(;;) permet de récupérer en permanence les socket s'il y en a
			// Ensuite ("onDatagramReceived()") permet de traiter et envoyer les socket au client
			void ClientUDP::receive()
			{
				for (;;)
				{
					Datagram datagram;
					Address from;
					int ret = from.recvFrom(mSocket, reinterpret_cast<uint8*>(&datagram), Datagram::BufferMaxSize);
					if (ret > 0)
					{
						const uint16 receivedSize = static_cast<uint16>(ret);
						if (receivedSize >= Datagram::HeaderSize)
						{
							datagram.datasize = receivedSize - Datagram::HeaderSize;
#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
							if (mSimulator.isEnabled())
							{
								// Push the datagram into the simulator
								mSimulator.push(datagram, from);
							}
							else
#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
							{
								// Handle the datagram directly
								if (auto client = getClient(from, true))
									client->onDatagramReceived(std::move(datagram));
							}
						}
						else
						{
							//!< Something is wrong, unexpected datagram
						}
					}
					else
					{
						if (ret < 0)
						{
							//!< Error handling
							const auto err = ErrorsUDP::Get();
							if (err != ErrorsUDP::WOULDBLOCK)
							{
								//!< Log that error
							}
						}
						break;
					}
				}
				// Poll pending datagrams from the simulator
#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
				if (mSimulator.isEnabled())
				{
					std::vector<std::pair<Datagram, Address>> datagrams = mSimulator.poll();
					for (auto& [datagram, from] : datagrams)
					{
						if (auto client = getClient(from, true))
							client->onDatagramReceived(std::move(datagram));
					}
				}
#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
			}

			std::vector<std::unique_ptr<Messages::Base>> ClientUDP::poll()
			{
				MessagesLock lock(mMessagesLock);
				return std::move(mMessages);
			}

			// permet d'obtenir le client qui est associé à l'adresse passé en paramètre
			// si aucun client n'est trouvé, alors il en créé un et l'ajoute à la liste
			DistantClient* ClientUDP::getClient(const Address& clientAddr, bool create /*= false*/)
			{
				auto itClient = std::find_if(mClients.begin(), mClients.end(), [&](const std::unique_ptr<DistantClient>& client) { return client->address() == clientAddr; });
				if (itClient != mClients.end())
					return itClient->get();
				else if (create)
				{
					mClients.emplace_back(std::make_unique<DistantClient>(*this, clientAddr, mClientIdsGenerator++));
					setupChannels(*(mClients.back()));
					return mClients.back().get();
				}
				else
					return nullptr;
			}

			void ClientUDP::setupChannels(DistantClient& client)
			{
				for (auto& channel : mRegisteredChannels)
					channel.creator(client);
			}

			void ClientUDP::onMessageReady(std::unique_ptr<Messages::Base>&& msg)
			{
				MessagesLock lock(mMessagesLock);
				mMessages.push_back(std::move(msg));
			}
		}
	}
}