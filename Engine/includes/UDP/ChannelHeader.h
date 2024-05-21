#pragma once
#include <cstdint>

namespace Bousk
{
    namespace Network
    {
        namespace UDP
        {
            /**
             * @brief Structure repr�sentant l'en-t�te d'un canal UDP.
             */
            struct ChannelHeader
            {
                /**
                 * @brief Taille de l'en-t�te du canal en octets.
                 */
                static constexpr uint8_t Size = sizeof(uint32_t) + sizeof(uint16_t);

                uint32_t channelIndex; /**< Index du canal. */
                uint16_t datasize; /**< Taille des donn�es. */
            };
        }
    }
}