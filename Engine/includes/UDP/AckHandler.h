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
         * @brief G�re les acquittements des paquets UDP.
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
             * @brief Met � jour les acquittements.
             * @param _newAck Le dernier num�ro de s�quence re�u.
             * @param _previousAcks Le masque des acquittements pr�c�dents.
             * @param _trackLoss Indique s'il faut suivre les pertes de paquets.
             */
            void update(uint16 _newAck, uint64 _previousAcks, bool _trackLoss = false);

            /**
             * @brief V�rifie si un num�ro de s�quence est acquitt�.
             * @param _ack Le num�ro de s�quence � v�rifier.
             * @return Vrai si le num�ro de s�quence est acquitt�, faux sinon.
             */
            bool isAcked(uint16 _ack) const;

            /**
             * @brief V�rifie si un num�ro de s�quence est nouvellement acquitt�.
             * @param _ack Le num�ro de s�quence � v�rifier.
             * @return Vrai si le num�ro de s�quence est nouvellement acquitt�, faux sinon.
             */
            bool isNewlyAcked(uint16 _ack) const;

            /**
             * @brief Renvoie le dernier num�ro de s�quence acquitt�.
             * @return Le dernier num�ro de s�quence acquitt�.
             */
            uint16 lastAck() const;

            /**
             * @brief Renvoie le masque des acquittements pr�c�dents.
             * @return Le masque des acquittements pr�c�dents.
             */
            uint64 previousAcksMask() const;

            /**
             * @brief Renvoie les nouveaux acquittements.
             * @return Un vecteur contenant les nouveaux acquittements.
             */
            std::vector<uint16> getNewAcks() const;

            /**
             * @brief Renvoie les num�ros de s�quence des paquets perdus.
             * @return Un vecteur contenant les num�ros de s�quence des paquets perdus.
             */
            std::vector<uint16>&& loss();

        private:
            uint16 mLastAck = -1; /**< Le dernier num�ro de s�quence acquitt�. */
            uint64 mPreviousAcks = -1; /**< Le masque des acquittements pr�c�dents. */
            uint64 mNewAcks = 0; /**< Les nouveaux acquittements. */
            std::vector<uint16> mLoss; /**< Les num�ros de s�quence des paquets perdus. */
            bool mLastAckIsNew{ false }; /**< Indique si le dernier acquittement est nouveau. */
        };
    }
}