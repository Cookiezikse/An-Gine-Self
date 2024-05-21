#pragma once

#include "UDP/Packet.h"
#include "ProtocolInterface.h"

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
                /**
                 * @brief Implementation of an unreliable ordered protocol.
                 */
                class UnreliableOrdered : IProtocol
                {
                    friend class Multiplexer_Test;
                    friend class Demultiplexer_Test;

                public:
                    using IProtocol::IProtocol; /**< Inherit constructors from IProtocol. */
                    ~UnreliableOrdered() override = default; /**< Destructor. */

                    /**
                     * @brief Queues a message for transmission.
                     * @param _msgData The data of the message to queue.
                     */
                    void queue(std::vector<uint8>&& _msgData) override { mMultiplexer.queue(std::move(_msgData)); }

                    /**
                     * @brief Serializes the protocol data into a buffer.
                     * @param _buffer Pointer to the buffer to serialize into.
                     * @param _buffersize Size of the buffer.
                     * @param _datagramId ID of the datagram.
                     * @return The number of bytes written into the buffer.
                     */
                    uint16 serialize(uint8* _buffer, uint16 _buffersize, Datagram::ID _datagramId
#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
                        , bool /*connectionInterrupted*/
#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
                    ) override {
                        return mMultiplexer.serialize(_buffer, _buffersize, _datagramId);
                    }

                    /**
                     * @brief Called when data is received.
                     * @param _data Pointer to the received data.
                     * @param _datasize Size of the received data.
                     */
                    void onDataReceived(const uint8* _data, uint16 _datasize) override { mDemultiplexer.onDataReceived(_data, _datasize); }

                    /**
                     * @brief Processes received data.
                     * @return A vector of processed messages.
                     */
                    std::vector<std::vector<uint8>> process() override { return mDemultiplexer.process(); }

                    /**
                     * @brief Checks if the protocol is reliable.
                     * @return True if the protocol is reliable, false otherwise.
                     */
                    virtual bool isReliable() const { return false; }

                private:
                    /**
                     * @brief Class for multiplexing data.
                     */
                    class Multiplexer
                    {
                        friend class Multiplexer_Test;

                    public:
                        Multiplexer() = default; /**< Default constructor. */
                        ~Multiplexer() = default; /**< Destructor. */

                        /**
                         * @brief Queues a message for transmission.
                         * @param _msgData The data of the message to queue.
                         */
                        void queue(std::vector<uint8>&& _msgData);

                        /**
                         * @brief Serializes queued data into a buffer.
                         * @param _buffer Pointer to the buffer to serialize into.
                         * @param _buffersize Size of the buffer.
                         * @param _datagramId ID of the datagram.
                         * @return The number of bytes written into the buffer.
                         */
                        uint16 serialize(uint8* _buffer, uint16 _buffersize, Datagram::ID);

					private:
						std::vector<Packet> mQueue; /**< Queue of packets to be transmitted. */
						Packet::Id mNextId{ 0 }; /**< ID of the next packet to be transmitted.*/
					} mMultiplexer; /**< Multiplexer instance. */
                    /**
                     * @brief Class for demultiplexing data.
                     */
                    class Demultiplexer
                    {
                        friend class Demultiplexer_Test;

                    public:
                        Demultiplexer() = default; /**< Default constructor. */
                        ~Demultiplexer() = default; /**< Destructor. */

                        /**
                         * @brief Called when data is received.
                         * @param _data Pointer to the received data.
                         * @param _datasize Size of the received data.
                         */
                        void onDataReceived(const uint8* _data, uint16 _datasize);

                        /**
                         * @brief Processes received data.
                         * @return A vector of processed messages.
                         */
                        std::vector<std::vector<uint8>> process();

                    private:
                        /**
                         * @brief Called when a packet is received.
                         * @param _pckt Pointer to the received packet.
                         */
                        void onPacketReceived(const Packet* _pckt);

					private:
						std::vector<Packet> mPendingQueue; /**< Queue of packets awaiting processing.*/
						Packet::Id mLastProcessed{ std::numeric_limits<Packet::Id>::max() }; /**< ID of the last processed packet. */
					} mDemultiplexer; /**< Demultiplexer instance. */
				};
			}
		}
	}
}

