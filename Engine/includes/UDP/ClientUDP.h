#pragma once

#include "Address.h"
#include "DistantClient.h"
#include "SocketsUDP.h"
#include "Protocol/Simulator.h"
#include "Types.h"

#include <winSock2.h>
#include <vector>
#include <memory>
#include <unordered_set>
#include <mutex>
#include <functional>

namespace Bousk
{
	namespace Network
	{
		namespace Messages
		{
			class Base;
		}
		namespace UDP
		{
			class ClientUDP
			{
				friend class DistantClient;

			public:
				ClientUDP();
				ClientUDP(const ClientUDP&) = delete;
				ClientUDP(ClientUDP&&) = delete;
				ClientUDP& operator=(const ClientUDP&) = delete;
				ClientUDP& operator=(ClientUDP&&) = delete;
				~ClientUDP();

				template<class T>
				void registerChannel(uint8 _channelId = 0);

				#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
					Simulator& simulator() { return mSimulator; }
					const Simulator& simulator() const { return mSimulator; }
				#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED

				bool init(uint16_t _port);
				void release();

				static void SetTimeout(std::chrono::milliseconds _timeout);
				static std::chrono::milliseconds GetTimeout();

				void connect(const Address& _addr);
				void disconnect(const Address& _addr);
				void sendTo(const Address& _target, std::vector<uint8>&& _data, uint32 _channelIndex);
				void sendTo(const Address& _target, const uint8* _data, size_t _dataSize, uint32 _channelIndex) { sendTo(_target, std::vector<uint8>(_data, _data + _dataSize), _channelIndex); }
				void processSend();
				void receive();
				std::vector<std::unique_ptr<Messages::Base>> poll();

				void onMessageReady(std::unique_ptr<Messages::Base>&& _msg);
				DistantClient* getClient(const Address& _clientAddr, bool _create = false);
				void setupChannels(DistantClient& _client);

				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					inline void enableNetworkInterruption() { setNetworkInterruptionEnabled(true); }
					inline void disableNetworkInterruption() { setNetworkInterruptionEnabled(false); }
					inline void setNetworkInterruptionEnabled(bool _enabled) { mNetworkInterruptionAllowed = _enabled; }
					inline bool isNetworkInterruptionAllowed() const { return mNetworkInterruptionAllowed; }
					inline bool isNetworkInterrupted() const { return !mInterruptedClients.empty(); }
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED

			private:
				SOCKET mSocket{ INVALID_SOCKET };
				std::vector<std::unique_ptr<DistantClient>> mClients;
				std::vector<std::unique_ptr<Messages::Base>> mMessages;
				uint64 mClientIdsGenerator{ 0 };
				std::mutex mMessagesLock;
				using MessagesLock = std::lock_guard<decltype(mMessagesLock)>;

				struct ChannelRegistration
				{
					std::function<void(DistantClient&)> creator;
					uint8 channelId;
				};
				std::vector<ChannelRegistration> mRegisteredChannels;

				struct Operation
				{
				public:
					enum class Type
					{
						Connect,
						SendTo,
						Disconnect,
					};
				public:
					static Operation Connect(const Address& _target) { return Operation(Type::Connect, _target); }
					static Operation SendTo(const Address& _target, std::vector<uint8>&& _data, uint32 _channel) { return Operation(Type::SendTo, _target, std::move(_data), _channel); }
					static Operation Disconnect(const Address& _target) { return Operation(Type::Disconnect, _target); }

					Operation(Type _type, const Address& _target) : mType(_type), mTarget(_target) {}
					Operation(Type _type, const Address& _target, std::vector<uint8>&& _data, uint32 _channel) : mType(_type), mTarget(_target), mData(std::move(_data)), mChannel(_channel) {}
					
					Type mType;
					Address mTarget;
					std::vector<uint8> mData;
					uint32 mChannel{ 0 };
				};
				std::mutex mOperationsLock;
				using OperationsLock = std::lock_guard<decltype(mOperationsLock)>;
				std::vector<Operation> mPendingOperations;

				#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
					Simulator mSimulator;
				#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					bool mNetworkInterruptionAllowed{ true };
					void onClientInterrupted(const DistantClient* _client);
					void onClientResumed(const DistantClient* _client);
					// Returns whether this client is the sole responsible for the network interruption
					bool isInterruptionCulprit(const DistantClient* _client) const;
					std::unordered_set<const DistantClient*> mInterruptedClients;
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
			};

			template<class T>
			void ClientUDP::registerChannel(uint8 channelId /*= 0*/)
			{
				assert(mSocket == INVALID_SOCKET); // Don't add channels after being initialized !!!
				assert(std::find_if(mRegisteredChannels.begin(), mRegisteredChannels.end(), [&](const ChannelRegistration& registration) { return registration.channelId == channelId; }) == mRegisteredChannels.end());
				mRegisteredChannels.push_back({ [channelId](DistantClient& distantClient) { distantClient.registerChannel<T>(channelId); }, channelId });
			}
		}
	}
}

