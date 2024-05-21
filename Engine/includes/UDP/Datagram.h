#pragma once

#include <array>

#include "UDP/Types.h"

namespace Bousk
{
    namespace Network
    {
        namespace UDP
        {
            /**
             * @brief Structure repr�sentant un datagramme UDP.
             */
            struct Datagram
            {
                using ID = uint16_t;
                enum class Type : uint8_t {
                    ConnectedData, /**< Donn�es connect�es. */
                    KeepAlive, /**< Keep-alive. */
                    Disconnection, /**< D�connexion. */
                };

                /**
                 * @brief En-t�te du datagramme.
                 */
                struct Header
                {
                    ID id; /**< Identifiant du datagramme. */
                    ID ack; /**< Acquittement. */
                    uint16_t previousAcks; /**< Acquittements pr�c�dents. */
                    Type type; /**< Type de datagramme. */
                };

                static constexpr uint16_t BufferMaxSize = 1400; /**< Taille maximale du buffer. */
                static constexpr uint16_t HeaderSize = sizeof(Header); /**< Taille de l'en-t�te. */
                static constexpr uint16_t DataMaxSize = BufferMaxSize - HeaderSize; /**< Taille maximale des donn�es. */

                Header header; /**< En-t�te du datagramme. */
                std::array<uint8_t, DataMaxSize> data; /**< Donn�es du datagramme. */

                uint16_t datasize{ 0 }; /**< Taille effective des donn�es du datagramme. */

                /**
                 * @brief Calcule la taille totale du datagramme.
                 * @return La taille totale du datagramme.
                 */
                uint16_t size() const { return HeaderSize + datasize; }
            };
        }
    }
}