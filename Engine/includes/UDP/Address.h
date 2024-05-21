#pragma once

#include "UDP/Types.h"

#include <string>
#include <winSock2.h>

namespace Bousk
{
    namespace Network
    {
        /**
         * @brief Classe représentant une adresse réseau.
         */
        class Address
        {
        public:
            /**
             * @brief Enumération décrivant les types d'adresses possibles.
             */
            enum class Type {
                None, /**< Aucun type défini. */
                IPv4, /**< Adresse IPv4. */
                IPv6, /**< Adresse IPv6. */
            };

        public:
            Address() = default;
            Address(const Address&) noexcept;
            Address(Address&&) noexcept;
            Address& operator=(const Address&) noexcept;
            Address& operator=(Address&&) noexcept;
            ~Address() = default;

            Address(const std::string& _ip, uint16 _port) noexcept;
            Address(const sockaddr_storage& _addr) noexcept;

            /**
             * @brief Crée une adresse de type "Any".
             * @param _type Le type de l'adresse.
             * @param _port Le numéro de port.
             * @return Une adresse de type "Any" avec le numéro de port spécifié.
             */
            static Address Any(Type _type, uint16 _port);

            /**
             * @brief Crée une adresse de type "Loopback".
             * @param _type Le type de l'adresse.
             * @param _port Le numéro de port.
             * @return Une adresse de type "Loopback" avec le numéro de port spécifié.
             */
            static Address Loopback(Type _type, uint16 _port);

            /**
             * @brief Vérifie si l'adresse est valide.
             * @return Vrai si l'adresse est valide, faux sinon.
             */
            inline bool isValid() const { return mType != Type::None; }

            /**
             * @brief Renvoie le type de l'adresse.
             * @return Le type de l'adresse.
             */
            inline Type type() const { return mType; }

            /**
             * @brief Renvoie l'adresse IP sous forme de chaîne de caractères.
             * @return L'adresse IP sous forme de chaîne de caractères.
             */
            std::string address() const;

            /**
             * @brief Renvoie le numéro de port.
             * @return Le numéro de port.
             */
            inline uint16 port() const { return mPort; }

            /**
             * @brief Renvoie une représentation textuelle de l'adresse.
             * @return Une représentation textuelle de l'adresse.
             */
            std::string toString() const;

            bool operator==(const Address& _other) const;
            bool operator!=(const Address& _other) const { return !(*this == _other); }
            bool operator<(const Address& _other) const;

            /**
             * @brief Connecte le socket donné à l'adresse interne.
             * @param _sckt Le socket à connecter.
             * @return Vrai en cas de succès, faux sinon.
             */
            bool connect(SOCKET _sckt) const;

            /**
             * @brief Accepte une connexion entrante sur le socket donné puis met à jour l'adresse interne avec celle de l'expéditeur.
             * @param _sckt Le socket à utiliser pour accepter la connexion.
             * @param _newClient Le nouveau socket client accepté.
             * @return Vrai si un nouveau client a été accepté et que son socket a été attribué à newClient. Faux sinon.
             */
            bool accept(SOCKET _sckt, SOCKET& _newClient);

            /**
             * @brief Lie le socket donné à l'adresse interne.
             * @param _sckt Le socket à lier.
             * @return Vrai en cas de succès, faux sinon.
             */
            bool bind(SOCKET _sckt) const;

            /**
             * @brief Envoie des données du socket donné à l'adresse interne.
             * @param _sckt Le socket à utiliser pour l'envoi.
             * @param _data Les données à envoyer.
             * @param _datalen La longueur des données à envoyer.
             * @return Le nombre d'octets envoyés en cas de succès, ou SOCKET_ERROR en cas d'erreur.
             */
            int sendTo(SOCKET _sckt, const char* _data, size_t _datalen) const;

            /**
             * @brief Reçoit des données du socket donné puis met à jour l'adresse interne avec celle de l'expéditeur.
             * @param _sckt Le socket à utiliser pour la réception.
             * @param _buffer Le tampon dans lequel stocker les données reçues.
             * @param _bufferSize La taille du tampon de réception.
             * @return Le nombre d'octets reçus en cas de succès, ou SOCKET_ERROR en cas d'erreur.
             */
            int recvFrom(SOCKET _sckt, uint8* _buffer, size_t _bufferSize);

        private:
            /**
             * @brief Définit l'adresse interne à partir d'une structure sockaddr_storage.
             * @param _src La structure sockaddr_storage à utiliser.
             */
            void set(const sockaddr_storage& _src);

        private:
            sockaddr_storage mStorage{ 0 }; /**< La structure de stockage de l'adresse. */
            uint16 mPort{ 0 }; /**< Le numéro de port. */
            Type mType{ Type::None }; /**< Le type de l'adresse. */
        };
    }
}