#pragma once

#include "UDP/Settings.h"

#include <optional>
#include <vector>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			namespace Protocols
			{
				class IProtocol
				{
				public:
					IProtocol(uint8_t _channelId) : mChannelId(_channelId) {}
					virtual ~IProtocol() = default;

					uint8_t channelId() const { return mChannelId; }

					virtual void queue(std::vector<uint8_t>&& _msgData) = 0;
					virtual uint16_t serialize(uint8_t* _buffer, uint16_t _buffersize, Datagram::ID _datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
											, bool _connectionInterrupted
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					) = 0;

					virtual void onDatagramAcked(Datagram::ID /*datagramId*/) {}
					virtual void onDatagramLost(Datagram::ID /*datagramId*/) {}

					virtual void onDataReceived(const uint8_t* _data, uint16_t _datasize) = 0;
					virtual std::vector<std::vector<uint8_t>> process() = 0;

					virtual bool isReliable() const = 0;

				private:
					uint8_t mChannelId;
				};
			}
		}
	}
}