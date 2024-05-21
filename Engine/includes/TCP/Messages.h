#pragma once

#include <vector>
#include <winSock2.h>

namespace Network
{
    namespace Messages
    {
        /**
         * @brief Classe de base pour les messages réseau.
         */
        class Base
        {
        public:
            /**
             * @brief Vérifie si le message est de type M.
             * @tparam M Type de message à vérifier.
             * @return true si le message est de type M, sinon false.
             */
            template<class M>
            bool Is() const { return mType == M::StaticType; }

            /**
             * @brief Convertit le message en type M.
             * @tparam M Type de message à convertir.
             * @return Pointeur vers le message converti en type M.
             */
            template<class M>
            const M* As() const { return static_cast<const M*>(this); }

            sockaddr_in from; /**< Adresse du destinataire du message. */
            uint64_t idFrom; /**< Identifiant du destinataire du message. */

        protected:
            /**
             * @brief Type de message.
             */
            enum class Type {
                Connection, /**< Message de connexion. */
                Disconnection, /**< Message de déconnexion. */
                UserData, /**< Message de données utilisateur. */
            };

            /**
             * @brief Constructeur de la classe Base.
             * @param type Type du message.
             */
            Base(Type _type)
                : mType(_type)
            {}

        private:
            Type mType; /**< Type du message. */
        };

        /**
         * @brief Message de connexion.
         */
        class Connection : public Base
        {
            friend class Base;
            static const Base::Type StaticType = Base::Type::Connection;

        public:
            /**
             * @brief Résultat de la connexion.
             */
            enum class Result {
                Success, /**< Connexion réussie. */
                Failed, /**< Connexion échouée. */
            };

            /**
             * @brief Constructeur de la classe Connection.
             * @param r Résultat de la connexion.
             */
            Connection(Result _r)
                : Base(Type::Connection)
                , result(_r)
            {}

            Result result; /**< Résultat de la connexion. */
        };

        /**
         * @brief Message de déconnexion.
         */
        class Disconnection : public Base
        {
            friend class Base;
            static const Base::Type StaticType = Base::Type::Disconnection;

        public:
            /**
             * @brief Raison de la déconnexion.
             */
            enum class Reason {
                Disconnected, /**< Déconnexion. */
                Lost, /**< Connexion perdue. */
            };

            /**
             * @brief Constructeur de la classe Disconnection.
             * @param r Raison de la déconnexion.
             */
            Disconnection(Reason _r)
                : Base(Type::Disconnection)
                , reason(_r)
            {}

            Reason reason; /**< Raison de la déconnexion. */
        };

        /**
         * @brief Message de données utilisateur.
         */
        class UserData : public Base
        {
            friend class Base;
            static const Base::Type StaticType = Base::Type::UserData;

        public:
            /**
             * @brief Constructeur de la classe UserData.
             * @param d Données utilisateur.
             */
            UserData(std::vector<unsigned char>&& _d)
                : Base(Type::UserData)
                , data(std::move(_d))
            {}

            std::vector<unsigned char> data; /**< Données utilisateur. */
        };
    }
}