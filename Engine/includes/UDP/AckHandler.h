#pragma once

#include "UDP/Utils/Utils.h"
#include "UDP/Utils/Utils.inl"

#include <cstdint>
#include <vector>

namespace Bousk
{
    namespace UDP
    {
        /**
         * @brief Gère les acquittements des paquets UDP.
         */
        class AckHandler
        {
        public:
            AckHandler() = default;
            AckHandler(const AckHandler&) = default;
            AckHandler& operator=(const AckHandler&) = default;
            AckHandler(AckHandler&&) = default;
            AckHandler& operator=(AckHandler&&) = default;
            ~AckHandler() = default;

            /**
             * @brief Met à jour les acquittements.
             * @param _newAck Le dernier numéro de séquence reçu.
             * @param _previousAcks Le masque des acquittements précédents.
             * @param _trackLoss Indique s'il faut suivre les pertes de paquets.
             */
            void update(uint16 _newAck, uint64 _previousAcks, bool _trackLoss = false);

            /**
             * @brief Vérifie si un numéro de séquence est acquitté.
             * @param _ack Le numéro de séquence à vérifier.
             * @return Vrai si le numéro de séquence est acquitté, faux sinon.
             */
            bool isAcked(uint16 _ack) const;

            /**
             * @brief Vérifie si un numéro de séquence est nouvellement acquitté.
             * @param _ack Le numéro de séquence à vérifier.
             * @return Vrai si le numéro de séquence est nouvellement acquitté, faux sinon.
             */
            bool isNewlyAcked(uint16 _ack) const;

            /**
             * @brief Renvoie le dernier numéro de séquence acquitté.
             * @return Le dernier numéro de séquence acquitté.
             */
            uint16 lastAck() const;

            /**
             * @brief Renvoie le masque des acquittements précédents.
             * @return Le masque des acquittements précédents.
             */
            uint64 previousAcksMask() const;

            /**
             * @brief Renvoie les nouveaux acquittements.
             * @return Un vecteur contenant les nouveaux acquittements.
             */
            std::vector<uint16> getNewAcks() const;

            /**
             * @brief Renvoie les numéros de séquence des paquets perdus.
             * @return Un vecteur contenant les numéros de séquence des paquets perdus.
             */
            std::vector<uint16>&& loss();

        private:
            uint16 mLastAck = -1; /**< Le dernier numéro de séquence acquitté. */
            uint64 mPreviousAcks = -1; /**< Le masque des acquittements précédents. */
            uint64 mNewAcks = 0; /**< Les nouveaux acquittements. */
            std::vector<uint16> mLoss; /**< Les numéros de séquence des paquets perdus. */
            bool mLastAckIsNew{ false }; /**< Indique si le dernier acquittement est nouveau. */
        };
    }
}