#pragma once

#include "Settings.h"
#include "Datagram.h"

#include <memory>
#include <optional>
#include <tuple>
#include <vector>

namespace Bousk
{
    namespace Network
    {
        namespace UDP
        {
            namespace Protocols
            {
                class IProtocol;
            }

            class ChannelsHandler
            {
            public:
                ChannelsHandler(); /**< Constructor. */
                ~ChannelsHandler(); /**< Destructor. */

                /**
                 * @brief Registers a channel with the specified ID.
                 *
                 * @tparam T Type of the protocol to register.
                 * @param _channelId ID of the channel.
                 */
                template<class T>
                void registerChannel(uint8 _channelId);

                /**
                 * @brief Queues a message data for sending on the specified channel.
                 *
                 * @param _msgData Message data to send.
                 * @param _channelIndex Index of the channel.
                 */
                void queue(std::vector<uint8>&& _msgData, uint32 _channelIndex);

                /**
                 * @brief Serializes the queued messages into a buffer.
                 *
                 * @param _buffer Pointer to the buffer.
                 * @param _buffersize Size of the buffer.
                 * @param _datagramId ID of the datagram.
                 * @param _connectionInterrupted Flag indicating whether the connection is interrupted.
                 * @return uint16 Number of bytes written to the buffer.
                 */
                uint16 serialize(uint8* _buffer, uint16 _buffersize, Datagram::ID _datagramId
#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
                    , bool _connectionInterrupted
#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
                );

                /**
                 * @brief Handles acknowledgment of a datagram.
                 *
                 * @param _datagramId ID of the acknowledged datagram.
                 */
                void onDatagramAcked(Datagram::ID _datagramId);

                /**
                 * @brief Handles loss of a datagram.
                 *
                 * @param _datagramId ID of the lost datagram.
                 */
                void onDatagramLost(Datagram::ID _datagramId);

                /**
                 * @brief Processes the received data.
                 *
                 * @param _data Pointer to the received data.
                 * @param _datasize Size of the received data.
                 */
                void onDataReceived(const uint8* _data, uint16 _datasize);

                /**
                 * @brief Processes the received data and returns a vector of tuples containing channel ID and message data.
                 *
                 * @param _isConnected Flag indicating whether the connection is established.
                 * @return std::vector<std::tuple<uint8, std::vector<uint8>>> Vector of tuples containing channel ID and message data.
                 */
                std::vector<std::tuple<uint8, std::vector<uint8>>> process(bool _isConnected);

			private:
				std::vector<std::unique_ptr<Protocols::IProtocol>> mChannels; /**< Vector of unique pointers to protocol interfaces. */
			};

			template<class T>
			void ChannelsHandler::registerChannel(uint8 _channelId)
			{
				mChannels.push_back(std::make_unique<T>(_channelId));
			}
		}
	}
}
