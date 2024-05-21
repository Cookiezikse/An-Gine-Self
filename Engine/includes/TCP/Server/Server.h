#pragma once

#include "TCP/Sockets.h"
#include "TCP/Client/Client.h"
#include "TCP/Messages.h"

#include <memory>
#include <map>
#include <list>
#include <cassert>

namespace Network
{
    namespace Messages {
        class Base;
    }
    namespace TCP
    {
        /**
         * @brief Implémentation privée de la classe Server.
         */
        class ServerImpl;

        /**
         * @brief La classe Server représente un serveur TCP.
         */
        class Server
        {
        public:
            /**
             * @brief Constructeur par défaut de la classe Server.
             */
            Server() {}

            /**
             * @brief Constructeur de copie supprimé.
             */
            Server(const Server&) = delete;

            /**
             * @brief Opérateur d'assignation de copie supprimé.
             */
            Server& operator=(const Server&) = delete;

            /**
             * @brief Constructeur de déplacement de la classe Server.
             */
            Server(Server&&);

            /**
             * @brief Opérateur d'assignation par déplacement de la classe Server.
             */
            Server& operator=(Server&&);

            /**
             * @brief Destructeur de la classe Server.
             */
            ~Server() {}

            /**
             * @brief Démarre le serveur sur le port spécifié.
             * @param _port Port sur lequel démarrer le serveur.
             * @return true si le démarrage réussit, sinon false.
             */
            bool Start(unsigned short _port);

            /**
             * @brief Arrête le serveur.
             */
            void Stop();

            /**
             * @brief Met à jour le serveur pour traiter les événements en attente.
             */
            void Update();

            /**
             * @brief Envoie des données à un client spécifié.
             * @param clientid Identifiant du client.
             * @param data Pointeur vers les données à envoyer.
             * @param len Longueur des données à envoyer.
             * @return true si l'envoi réussit, sinon false.
             */
            bool SendTo(uint64_t _clientid, const unsigned char* _data, unsigned int _len);

            /**
             * @brief Envoie des données à tous les clients connectés.
             * @param data Pointeur vers les données à envoyer.
             * @param len Longueur des données à envoyer.
             * @return true si l'envoi réussit, sinon false.
             */
            bool SendToAll(const unsigned char* _data, unsigned int _len);

            /**
             * @brief Récupère les messages entrants du serveur.
             * @return Un pointeur unique vers le message reçu.
             */
            std::unique_ptr<Messages::Base> Poll();

            /**
             * @brief Obtient la liste des clients connectés au serveur.
             *
             * @return Une référence constante à la map des clients, indexée par leur identifiant.
             */
            const std::map<uint64_t, Client>& GetClients() const;

        private:
            std::unique_ptr<ServerImpl> mImpl; /**< Implémentation privée du serveur. */
        };

        class ServerImpl
        {
        public:
            /**
            * @brief Constructeur par défaut de la classe ServerImpl.
            */
            ServerImpl() = default;

            /**
             * @brief Destructeur de la classe ServerImpl.
             */
            ~ServerImpl();

            /**
             * @brief Démarre le serveur sur le port spécifié.
             *
             * @param _port Port sur lequel démarrer le serveur.
             * @return true si le démarrage réussit, sinon false.
             */
            bool Start(unsigned short _port);

            /**
             * @brief Arrête le serveur.
             */
            void Stop();

            /**
             * @brief Met à jour le serveur.
             */
            void Update();

            /**
             * @brief Envoie des données à un client spécifique.
             *
             * @param _clientid Identifiant du client.
             * @param _data Pointeur vers les données à envoyer.
             * @param _len Longueur des données à envoyer.
             * @return true si l'envoi réussit, sinon false.
             */
            bool SendTo(uint64_t _clientid, const unsigned char* _data, unsigned int _len);

            /**
             * @brief Envoie des données à tous les clients connectés.
             *
             * @param _data Pointeur vers les données à envoyer.
             * @param _len Longueur des données à envoyer.
             * @return true si l'envoi réussit, sinon false.
             */
            bool SendToAll(const unsigned char* _data, unsigned int _len);

            /**
             * @brief Récupère les messages entrants.
             *
             * @return Un pointeur unique vers le message reçu.
             */
            std::unique_ptr<Messages::Base> Poll();

            const std::map<uint64_t, Client>& GetClients() const;

        private:
            std::map<uint64_t, Client> mClients; /**< Map des clients connectés, indexés par leur identifiant. */
            std::list<std::unique_ptr<Messages::Base>> mMessages; /**< Liste des messages entrants. */
            SOCKET mSocket{ INVALID_SOCKET }; /**< Socket du serveur. */
        };
    }
}