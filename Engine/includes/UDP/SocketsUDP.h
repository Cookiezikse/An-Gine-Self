#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <winSock2.h>
#include <ws2tcpip.h>
//using nfds_t = unsigned long;
//inline int poll(pollfd fdarray[], nfds_t nfds, int timeout) { return WSAPoll(fdarray, nfds, timeout); }
#endif

#include <string>

namespace Bousk
{
    namespace Network
    {
        /**
         * @brief Initialise les sockets.
         * @return true si l'initialisation réussit, sinon false.
         */
        bool Start();

        /**
         * @brief Libère les ressources associées aux sockets.
         */
        void Release();

        /**
         * @brief Configure un socket en mode non bloquant.
         * @param socket Socket à configurer.
         * @return true si la configuration réussit, sinon false.
         */
        bool SetNonBlocking(SOCKET _socket);

        /**
         * @brief Configure un socket pour réutiliser l'adresse.
         * @param socket Socket à configurer.
         * @return true si la configuration réussit, sinon false.
         */
        bool SetReuseAddr(SOCKET _socket);

        /**
         * @brief Ferme un socket.
         * @param socket Socket à fermer.
         */
        void CloseSocket(SOCKET _socket);

        /**
         * @brief Récupère l'adresse IP d'une structure sockaddr_in.
         * @param addr Structure sockaddr_in contenant l'adresse.
         * @return L'adresse IP sous forme de chaîne de caractères.
         */
        std::string GetAddress(const sockaddr_in& _addr);

        /**
         * @brief Récupère le port d'une structure sockaddr_in.
         * @param addr Structure sockaddr_in contenant le port.
         * @return Le port sous forme d'entier non signé.
         */
        unsigned short GetPort(const sockaddr_in& _addr);
    }
}