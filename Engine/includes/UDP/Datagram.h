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
             * @brief Structure représentant un datagramme UDP.
             */
            struct Datagram
            {
                using ID = uint16_t;
                enum class Type : uint8_t {
                    ConnectedData, /**< Données connectées. */
                    KeepAlive, /**< Keep-alive. */
                    Disconnection, /**< Déconnexion. */
                };

                /**
                 * @brief En-tête du datagramme.
                 */
                struct Header
                {
                    ID id; /**< Identifiant du datagramme. */
                    ID ack; /**< Acquittement. */
                    uint16_t previousAcks; /**< Acquittements précédents. */
                    Type type; /**< Type de datagramme. */
                };

                static constexpr uint16_t BufferMaxSize = 1400; /**< Taille maximale du buffer. */
                static constexpr uint16_t HeaderSize = sizeof(Header); /**< Taille de l'en-tête. */
                static constexpr uint16_t DataMaxSize = BufferMaxSize - HeaderSize; /**< Taille maximale des données. */

                Header header; /**< En-tête du datagramme. */
                std::array<uint8_t, DataMaxSize> data; /**< Données du datagramme. */

                uint16_t datasize{ 0 }; /**< Taille effective des données du datagramme. */

                /**
                 * @brief Calcule la taille totale du datagramme.
                 * @return La taille totale du datagramme.
                 */
                uint16_t size() const { return HeaderSize + datasize; }
            };
        }
    }
}