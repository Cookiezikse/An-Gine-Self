#pragma once

#include "Address.h"

#include <cassert>
#include <numeric>
#include <optional>
#include <vector>

namespace Bousk
{
    namespace Network
    {
        namespace Messages
        {
#define DECLARE_MESSAGE(name) friend class Base; static const Base::Type StaticType = Base::Type::name
            /**
             * @brief Base class for network messages.
             */
            class Base
            {
            public:
                /**
                 * @brief Checks if the message is of type M.
                 * @tparam M The type to check against.
                 * @return True if the message is of type M, false otherwise.
                 */
                template<class M>
                bool is() const { return mType == M::StaticType; }

                /**
                 * @brief Casts the message to type M.
                 * @tparam M The type to cast to.
                 * @return Pointer to the message cast to type M.
                 */
                template<class M>
                const M* as() const { assert(is<M>()); return static_cast<const M*>(this); }

                /**
                 * @brief Retrieves the emitter's address.
                 * @return The emitter's address.
                 */
                const Address& emitter() const { return mEmitter; }

                /**
                 * @brief Retrieves the emitter's ID.
                 * @return The emitter's ID.
                 */
                uint64_t emmiterId() const { return mEmitterId; }

            protected:
                enum class Type {
                    IncomingConnection,
                    Connection,
                    ConnectionInterrupted,
                    ConnectionResumed,
                    Disconnection,
                    UserData,
                };

                /**
                 * @brief Constructs a base message.
                 * @param _type The type of the message.
                 * @param _emitter The emitter's address.
                 * @param _emitterid The emitter's ID.
                 */
                Base(Type _type, const Address& _emitter, uint64_t _emitterid)
                    : mType(_type)
                    , mEmitter(_emitter)
                    , mEmitterId(_emitterid)
                {}

            private:
                Address mEmitter; /**< The emitter's address. */
                uint64_t mEmitterId; /**< The emitter's ID. */
                Type mType; /**< The type of the message. */
            };

            /**
             * @brief Represents an incoming connection message.
             */
            class IncomingConnection : public Base
            {
                DECLARE_MESSAGE(IncomingConnection);
            public:
                /**
                 * @brief Constructs an incoming connection message.
                 * @param _emitter The emitter's address.
                 * @param _emitterid The emitter's ID.
                 */
                IncomingConnection(const Address& _emitter, uint64_t _emitterid)
                    : Base(Type::IncomingConnection, _emitter, _emitterid)
                {}
            };

            /**
             * @brief Represents a connection message.
             */
            class Connection : public Base
            {
                DECLARE_MESSAGE(Connection);
            public:
                /**
                 * @brief Represents the result of a connection attempt.
                 */
                enum class Result {
                    Success,
                    Failed,
                    Refused,
                    TimedOut,
                };

                /**
                 * @brief Constructs a connection message.
                 * @param _emitter The emitter's address.
                 * @param _emitterid The emitter's ID.
                 * @param _r The result of the connection attempt.
                 */
                Connection(const Address& _emitter, uint64_t _emitterid, Result _r)
                    : Base(Type::Connection, _emitter, _emitterid)
                    , result(_r)
                {}

                Result result; /**< The result of the connection attempt. */
            };

#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
            /**
             * @brief Represents a message indicating that a connection has been interrupted.
             */
            class ConnectionInterrupted : public Base
            {
                DECLARE_MESSAGE(ConnectionInterrupted);
            public:
                /**
                 * @brief Constructs a new ConnectionInterrupted object.
                 * @param _emitter The address of the emitter.
                 * @param _emitterid The ID of the emitter.
                 * @param _isDirect True if the emitter is directly interrupted to us. False if the emitter forwarded an interruption on his side.
                 */
                ConnectionInterrupted(const Address& _emitter, uint64_t _emitterid, bool _isDirect)
                    : Base(Type::ConnectionInterrupted, _emitter, _emitterid)
                    , isDirectInterruption(_isDirect)
                {}
                bool isDirectInterruption; /**< True if the emitter is directly interrupted to us. False if the emitter forwarded an interruption on his side. */
            };

            /**
             * @brief Represents a message indicating that a connection has been resumed.
             */
            class ConnectionResumed : public Base
            {
                DECLARE_MESSAGE(ConnectionResumed);
            public:
                /**
                 * @brief Constructs a new ConnectionResumed object.
                 * @param _emitter The address of the emitter.
                 * @param _emitterid The ID of the emitter.
                 * @param _networkResume True if the network is now completely resumed. False if network is not yet resumed due to another client being interrupted.
                 */
                ConnectionResumed(const Address& _emitter, uint64_t _emitterid, bool _networkResume)
                    : Base(Type::ConnectionResumed, _emitter, _emitterid)
                    , isNetworkResumed(_networkResume)
                {}
                bool isNetworkResumed; /**< True if the network is now completely resumed. False if network is not yet resumed due to another client being interrupted. */
            };
#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED

            /**
             * @brief Represents a disconnection message.
             */
            class Disconnection : public Base
            {
                DECLARE_MESSAGE(Disconnection);
            public:
                /**
                 * @brief Represents the reason for disconnection.
                 */
                enum class Reason {
                    Disconnected,
                    Lost,
                };

                /**
                 * @brief Constructs a disconnection message.
                 * @param _emitter The emitter's address.
                 * @param _emitterid The emitter's ID.
                 * @param _r The reason for disconnection.
                 */
                Disconnection(const Address& _emitter, uint64_t _emitterid, Reason _r)
                    : Base(Type::Disconnection, _emitter, _emitterid)
                    , reason(_r)
                {}

                Reason reason; /**< The reason for disconnection. */
            };

            /**
             * @brief Represents a user data message.
             */
            class UserData : public Base
            {
                DECLARE_MESSAGE(UserData);
            public:
                /**
                 * @brief Constructs a user data message.
                 * @param _emitter The emitter's address.
                 * @param _emitterid The emitter's ID.
                 * @param _d The user data.
                 * @param _chanId The channel ID.
                 */
                UserData(const Address& _emitter, uint64_t _emitterid, std::vector<unsigned char>&& _d, uint8_t _chanId)
                    : Base(Type::UserData, _emitter, _emitterid)
                    , data(std::move(_d))
                    , channelId(_chanId)
                {}

                std::vector<unsigned char> data; /**< The user data. */
                uint8_t channelId; /**< The channel ID. */
            };
#undef DECLARE_MESSAGE
        }
    }
}

#include <iosfwd>
std::ostream& operator<<(std::ostream& _out, Bousk::Network::Messages::Connection::Result _result);
std::ostream& operator<<(std::ostream& _out, Bousk::Network::Messages::Disconnection::Reason _reason);