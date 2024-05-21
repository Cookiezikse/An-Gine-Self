#pragma once

#include "UDP/Types.h"

#include <string>
#include <winSock2.h>

namespace Bousk
{
    namespace Network
    {
        /**
         * @brief Classe repr�sentant une adresse r�seau.
         */
        class Address
        {
        public:
            /**
             * @brief Enum�ration d�crivant les types d'adresses possibles.
             */
            enum class Type {
                None, /**< Aucun type d�fini. */
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
             * @brief Cr�e une adresse de type "Any".
             * @param _type Le type de l'adresse.
             * @param _port Le num�ro de port.
             * @return Une adresse de type "Any" avec le num�ro de port sp�cifi�.
             */
            static Address Any(Type _type, uint16 _port);

            /**
             * @brief Cr�e une adresse de type "Loopback".
             * @param _type Le type de l'adresse.
             * @param _port Le num�ro de port.
             * @return Une adresse de type "Loopback" avec le num�ro de port sp�cifi�.
             */
            static Address Loopback(Type _type, uint16 _port);

            /**
             * @brief V�rifie si l'adresse est valide.
             * @return Vrai si l'adresse est valide, faux sinon.
             */
            inline bool isValid() const { return mType != Type::None; }

            /**
             * @brief Renvoie le type de l'adresse.
             * @return Le type de l'adresse.
             */
            inline Type type() const { return mType; }

            /**
             * @brief Renvoie l'adresse IP sous forme de cha�ne de caract�res.
             * @return L'adresse IP sous forme de cha�ne de caract�res.
             */
            std::string address() const;

            /**
             * @brief Renvoie le num�ro de port.
             * @return Le num�ro de port.
             */
            inline uint16 port() const { return mPort; }

            /**
             * @brief Renvoie une repr�sentation textuelle de l'adresse.
             * @return Une repr�sentation textuelle de l'adresse.
             */
            std::string toString() const;

            bool operator==(const Address& _other) const;
            bool operator!=(const Address& _other) const { return !(*this == _other); }
            bool operator<(const Address& _other) const;

            /**
             * @brief Connecte le socket donn� � l'adresse interne.
             * @param _sckt Le socket � connecter.
             * @return Vrai en cas de succ�s, faux sinon.
             */
            bool connect(SOCKET _sckt) const;

            /**
             * @brief Accepte une connexion entrante sur le socket donn� puis met � jour l'adresse interne avec celle de l'exp�diteur.
             * @param _sckt Le socket � utiliser pour accepter la connexion.
             * @param _newClient Le nouveau socket client accept�.
             * @return Vrai si un nouveau client a �t� accept� et que son socket a �t� attribu� � newClient. Faux sinon.
             */
            bool accept(SOCKET _sckt, SOCKET& _newClient);

            /**
             * @brief Lie le socket donn� � l'adresse interne.
             * @param _sckt Le socket � lier.
             * @return Vrai en cas de succ�s, faux sinon.
             */
            bool bind(SOCKET _sckt) const;

            /**
             * @brief Envoie des donn�es du socket donn� � l'adresse interne.
             * @param _sckt Le socket � utiliser pour l'envoi.
             * @param _data Les donn�es � envoyer.
             * @param _datalen La longueur des donn�es � envoyer.
             * @return Le nombre d'octets envoy�s en cas de succ�s, ou SOCKET_ERROR en cas d'erreur.
             */
            int sendTo(SOCKET _sckt, const char* _data, size_t _datalen) const;

            /**
             * @brief Re�oit des donn�es du socket donn� puis met � jour l'adresse interne avec celle de l'exp�diteur.
             * @param _sckt Le socket � utiliser pour la r�ception.
             * @param _buffer Le tampon dans lequel stocker les donn�es re�ues.
             * @param _bufferSize La taille du tampon de r�ception.
             * @return Le nombre d'octets re�us en cas de succ�s, ou SOCKET_ERROR en cas d'erreur.
             */
            int recvFrom(SOCKET _sckt, uint8* _buffer, size_t _bufferSize);

        private:
            /**
             * @brief D�finit l'adresse interne � partir d'une structure sockaddr_storage.
             * @param _src La structure sockaddr_storage � utiliser.
             */
            void set(const sockaddr_storage& _src);

        private:
            sockaddr_storage mStorage{ 0 }; /**< La structure de stockage de l'adresse. */
            uint16 mPort{ 0 }; /**< Le num�ro de port. */
            Type mType{ Type::None }; /**< Le type de l'adresse. */
        };
    }
}