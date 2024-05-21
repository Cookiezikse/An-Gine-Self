#pragma once

#ifdef _WIN32
#include <winSock2.h>
#else
#include <cerrno>
#define SOCKET int
#define INVALID_SOCKET ((int)-1)
#define SOCKET_ERROR (int(-1))
#endif


namespace Bousk
{
    namespace Network
    {
        namespace ErrorsUDP
        {
            /**
             * @brief Récupère le code d'erreur du dernier appel r�seau.
             * @return Le code d'erreur du dernier appel réseau.
             */
            int Get();

            /**
             * @brief Enumération des codes d'erreur réseau courants.
             */
            enum {
                AGAIN = WSATRY_AGAIN, /**< Ressayer l'opération. */
                WOULDBLOCK = WSAEWOULDBLOCK, /**< L'opération bloquerait le socket. */
                INPROGRESS = WSAEINPROGRESS, /**< L'opération est en cours. */
                INTR = WSAEINTR, /**< L'appel système a été interrompu avant qu'il ne se termine. */
            };
        }
    }
}