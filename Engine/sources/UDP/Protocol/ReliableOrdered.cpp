#include "UDP/Protocol/ReliableOrdered.h"

#include "UDP/Utils/Utils.h"
#include <cassert>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			namespace Protocols
			{
				void ReliableOrdered::Multiplexer::queue(std::vector<uint8>&& msgData)
				{
					assert(msgData.size() <= Packet::MaxMessageSize);
					if (msgData.size() > Packet::DataMaxSize)
					{
						uint16 queuedSize = 0;
						while (queuedSize < msgData.size())
						{
							const auto fragmentSize = std::min(Packet::DataMaxSize, static_cast<uint16>(msgData.size() - queuedSize));
							mQueue.resize(mQueue.size() + 1);
							Packet& packet = mQueue.back().packet();
							packet.header.id = mNextId++;
							packet.header.type = ((queuedSize == 0) ? Packet::Type::FirstFragment : Packet::Type::Fragment);
							packet.header.size = fragmentSize;
							memcpy(packet.data(), msgData.data() + queuedSize, fragmentSize);
							queuedSize += fragmentSize;
						}
						mQueue.back().packet().header.type = Packet::Type::LastFragment;
						assert(queuedSize == msgData.size());
					}
					else
					{
						//!< Single packet
						mQueue.resize(mQueue.size() + 1);
						Packet& packet = mQueue.back().packet();
						packet.header.id = mNextId++;
						packet.header.type = Packet::Type::FullMessage;
						packet.header.size = static_cast<uint16>(msgData.size());
						memcpy(packet.data(), msgData.data(), msgData.size());
					}
				}

				uint16 ReliableOrdered::Multiplexer::serialize(uint8* buffer, const uint16 buffersize, Datagram::ID datagramId
				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
									, const bool connectionInterrupted
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
								)
								{
				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
									// Don't send reliable data if the connection is interrupted
									if (connectionInterrupted)
										return 0;
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED

					uint16 serializedSize = 0;
					for (auto& packetHolder : mQueue)
					{
						if (!(Utils::SequenceDiff(packetHolder.packet().id(), mFirstAllowedPacket) < Demultiplexer::QueueSize))
							break;
						if (!packetHolder.shouldSend())
							continue;
						const auto& packet = packetHolder.packet();
						if (serializedSize + packet.size() > buffersize)
							continue; //!< Not enough room, let's skip this one for now

						memcpy(buffer, packet.buffer(), packet.size());
						serializedSize += packet.size();
						buffer += packet.size();

						//!< Once the packet has been serialized into a datagram, save which datagram it's been included into
						packetHolder.onSent(datagramId);
					}
					return serializedSize;
				}

				void ReliableOrdered::Multiplexer::onDatagramAcked(const Datagram::ID datagramId)
				{
					if (mQueue.empty())
						return;

					mQueue.erase(std::remove_if(mQueue.begin(), mQueue.end()
						, [&](const ReliablePacket& packetHolder) { return packetHolder.isIncludedIn(datagramId); })
						, mQueue.cend());
					if (mQueue.empty())
						mFirstAllowedPacket = mNextId;
					else if (Utils::IsSequenceNewer(mQueue.front().packet().id(), mFirstAllowedPacket))
						mFirstAllowedPacket = mQueue.front().packet().id();
				}

				void ReliableOrdered::Multiplexer::onDatagramLost(const Datagram::ID datagramId)
				{
					for (auto& packetHolder : mQueue)
					{
						if (packetHolder.isIncludedIn(datagramId))
							packetHolder.resend();
					}
				}

				void ReliableOrdered::Demultiplexer::onDataReceived(const uint8* data, const uint16 datasize)
				{
					//<! Extraire les paquets du tampon
					uint16 processedDataSize = 0;
					while (processedDataSize < datasize)
					{
						const Packet* pckt = reinterpret_cast<const Packet*>(data);
						if (processedDataSize + pckt->size() > datasize || pckt->datasize() > Packet::DataMaxSize)
						{
							//<! Paquet ou tampon mal formé, on interrompt
							return;
						}
						onPacketReceived(pckt);
						processedDataSize += pckt->size();
						data += pckt->size();
					}
				}

				void ReliableOrdered::Demultiplexer::onPacketReceived(const Packet* pckt)
				{
					if (!Utils::IsSequenceNewer(pckt->id(), mLastProcessed))
						return; //!< Paquet obsolète

					//!< Calcul de l’index dans le tableau
					const size_t index = pckt->id() % mPendingQueue.size();
					Packet& pendingPacket = mPendingQueue[index];
					if (pendingPacket.datasize() == 0)
					{
						// Emplacement disponible, copier simplement les données du réseau dans notre tableau
						pendingPacket = *pckt;
					}
					else
					{
						// Emplacement NON disponible, s’assurer qu’il contient déjà notre paquet, sinon il y a un problème
						assert(pendingPacket.id() == pckt->id() && pendingPacket.datasize() == pckt->datasize());
					}
				}

				std::vector<std::vector<uint8>> ReliableOrdered::Demultiplexer::process()
				{
					//!< Fonction de réinitialisation d’un paquet
					auto ResetPacket = [](Packet& pckt) { pckt.header.size = 0; };
					std::vector<std::vector<uint8>> messagesReady;

					Packet::Id expectedPacketId = mLastProcessed + 1;
					//!< Il faut itérer sur notre tableau en commençant par le paquet attendu, qui peut ne pas être en index 0
					const size_t startIndexOffset = expectedPacketId % mPendingQueue.size();
					for (size_t i = 0; i < mPendingQueue.size(); ++i, ++expectedPacketId)
					{
						//!< On calcule l’index dans notre tableau du prochain paquet à traiter
						const size_t packetIndex = (i + startIndexOffset) % mPendingQueue.size();
						Packet& packet = mPendingQueue[packetIndex];
						if (packet.type() == Packet::Type::FullMessage)
						{
							//!< Message complet
							std::vector<uint8> msg(packet.data(), packet.data() + packet.datasize());
							mLastProcessed = packet.id();
							ResetPacket(packet);
							messagesReady.push_back(std::move(msg));
						}
						else if (packet.type() == Packet::Type::FirstFragment)
						{
							//!< Vérifier que le message est prêt
							const bool isMessageFull = [=]() mutable
								{
									// On saute le premier fragment déjà traité par la boucle sur i
									++i;
									++expectedPacketId;
									// On itère sur les paquets restants pour vérifier que notre message soit complet
									for (size_t j = i; j < mPendingQueue.size(); ++j, ++expectedPacketId)
									{
										const size_t idx = (j + startIndexOffset) % mPendingQueue.size();
										const Packet& pckt = mPendingQueue[idx];
										if (pckt.id() != expectedPacketId || pckt.datasize() == 0)
											break; // Un paquet est manquant
										if (pckt.type() == Packet::Type::LastFragment)
										{
											//!< Nous avons atteint et reçu le dernier fragment, le message est complet
											return true;
										}
										else if (pckt.type() != Packet::Type::Fragment)
										{
											//!< Si nous arrivons ici nous avons probablement reçu un paquet mal formé ou malicieux
											break;
										}
									}
									return false;
								}();
								if (!isMessageFull)
									break; // Protocole ordonné fiable : si le message suivant à extraire est incomplet, nous pouvons arrêter le processus d’extraction

								// Nous avons un message fragmenté complet, nous pouvons maintenant extraire les données et réinitialiser chaque paquet utilisé
								std::vector<uint8> msg(packet.data(), packet.data() + packet.datasize());
								++i;
								++expectedPacketId;
								// Itération sur les paquets restants pour compléter le message
								for (size_t j = i; j < mPendingQueue.size(); ++i, ++j, ++expectedPacketId)
								{
									const size_t idx = (j + startIndexOffset) % mPendingQueue.size();
									Packet& pckt = mPendingQueue[idx];

									if (pckt.type() == Packet::Type::LastFragment)
									{
										//!< Dernier fragment du message maintenant complet
										msg.insert(msg.cend(), pckt.data(), pckt.data() + pckt.datasize());
										mLastProcessed = pckt.id();
										ResetPacket(pckt);
										messagesReady.push_back(std::move(msg));
										break;
									}
									else if (pckt.type() != Packet::Type::Fragment)
									{
										//!< Paquet mal formé ou malicieux
										break;
									}

									msg.insert(msg.cend(), pckt.data(), pckt.data() + pckt.datasize());
									ResetPacket(pckt);
								}
						}
						else
						{
							// Protocole ordonné fiable : si le message suivant à extraire est incomplet, nous pouvons arrêter le processus d’extraction
							break;
						}
					}
					return messagesReady;
				}
			}
		}
	}
}