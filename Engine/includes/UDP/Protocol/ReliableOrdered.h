#pragma once

#include "UDP/Packet.h" 
#include "UDP/Datagram.h"
#include "ProtocolInterface.h"

#include <vector>
#include <set>

class Multiplexer_Test;
class Demultiplexer_Test;
namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			namespace Protocols
			{
				class ReliableOrdered : public IProtocol
				{
					friend class Multiplexer_Test;
					friend class Demultiplexer_Test;
				public:
					using IProtocol::IProtocol;
					~ReliableOrdered() override = default;

					void queue(std::vector<uint8>&& _msgData) override { mMultiplexer.queue(std::move(_msgData)); }
					uint16 serialize(uint8* _buffer, const uint16 _buffersize, const Datagram::ID _datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
											, const bool _connectionInterrupted
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
										) override {
											return mMultiplexer.serialize(_buffer, _buffersize, _datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
												, _connectionInterrupted
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					);}

					void onDatagramAcked(const Datagram::ID _datagramId) override { mMultiplexer.onDatagramAcked(_datagramId); }
					void onDatagramLost(const Datagram::ID _datagramId) override { mMultiplexer.onDatagramLost(_datagramId); }

					void onDataReceived(const uint8* _data, const uint16 _datasize) override { mDemultiplexer.onDataReceived(_data, _datasize); }
					std::vector<std::vector<uint8>> process() override { return mDemultiplexer.process(); }

					virtual bool isReliable() const { return true; }
					
				private:
					class Multiplexer
					{
						friend class Multiplexer_Test;
					public:
						Multiplexer() = default;
						~Multiplexer() = default;

						void queue(std::vector<uint8>&& _msgData);
						uint16 serialize(uint8* _buffer, const uint16 _buffersize, const Datagram::ID _datagramId
						#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
													, bool _connectionInterrupted
						#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
						);

						void onDatagramAcked(const Datagram::ID _datagramId);
						void onDatagramLost(const Datagram::ID _datagramId);

					private:
						class ReliablePacket
						{
						public:
							Packet& packet() { return mPacket; }
							const Packet& packet() const { return mPacket; }

							bool shouldSend() const { return mShouldSend; }
							void resend() { mShouldSend = true; }
							void onSent(const Datagram::ID _datagramId) { mDatagramsIncluding.insert(_datagramId); mShouldSend = false; }
							bool isIncludedIn(const Datagram::ID _datagramId) const { return mDatagramsIncluding.find(_datagramId) != mDatagramsIncluding.cend(); }

						private:
							Packet mPacket;
							std::set<Datagram::ID> mDatagramsIncluding;
							bool mShouldSend{ true };
						};
						std::vector<ReliablePacket> mQueue;
						Packet::Id mNextId{ 0 };
						Packet::Id mFirstAllowedPacket{ 0 };
					} mMultiplexer;
					class Demultiplexer
					{
						friend class Demultiplexer_Test;
					public:
						static constexpr size_t QueueSize = 2 * Packet::MaxPacketsPerMessage;
					public:
						Demultiplexer() = default;
						~Demultiplexer() = default;

						void onDataReceived(const uint8* _data, const uint16 _datasize);
						std::vector<std::vector<uint8>> process();

					private:
						void onPacketReceived(const Packet* _pckt);

					private:
						std::array<Packet, QueueSize> mPendingQueue;
						Packet::Id mLastProcessed{ std::numeric_limits<Packet::Id>::max() };
					} mDemultiplexer;
				};
			}
		}
	}
}