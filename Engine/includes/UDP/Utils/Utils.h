#pragma once

#include "UDP/Types.h"

#include <chrono>

#define UNUSED(x) (void)(x)

namespace Bousk
{
    namespace Utils
    {
        /**
         * @brief Returns the current time in milliseconds since the epoch.
         * @return The current time in milliseconds.
         */
        inline std::chrono::milliseconds Now();

        /**
         * @brief Checks if a sequence number is newer than another.
         * @param _sNew The new sequence number.
         * @param _sLast The last sequence number.
         * @return True if _sNew is newer than _sLast, otherwise false.
         */
        inline bool IsSequenceNewer(uint16 _sNew, uint16 _sLast);

        /**
         * @brief Calculates the difference between two sequence numbers.
         * @param _sNew The new sequence number.
         * @param _sLast The last sequence number.
         * @return The difference between _sNew and _sLast.
         */
        inline uint16 SequenceDiff(uint16 _sNew, uint16 _sLast);

        /**
         * @brief Sets a bit in a bitfield.
         * @param _bitfield The bitfield to modify.
         * @param _n The index of the bit to set.
         */
        inline void SetBit(uint64& _bitfield, uint8 _n);

        /**
         * @brief Unsets a bit in a bitfield.
         * @param _bitfield The bitfield to modify.
         * @param _n The index of the bit to unset.
         */
        inline void UnsetBit(uint64& _bitfield, uint8 _n);

        /**
         * @brief Checks if a bit is set in a bitfield.
         * @param _bitfield The bitfield to check.
         * @param _n The index of the bit to check.
         * @return True if the bit is set, otherwise false.
         */
        inline bool HasBit(uint64 _bitfield, uint8 _n);

        using Bousk::Bit;

        /**
         * @brief Counts the number of bits needed to represent a value.
         * @param _v The value.
         * @return The number of bits needed to represent _v.
         */
        uint8 CountNeededBits(uint64 _v);

        /**
         * @brief Creates a bit mask with specified number of right bits set to 1.
         * @param _rightBits The number of right bits to set to 1.
         * @return The bit mask.
         */
        uint8 CreateRightBitsMask(uint8 _rightBits);

        /**
         * @brief Creates a bit mask with specified number of bits set to 1, skipping a number of right bits.
         * @param _nbBits The number of bits to set to 1.
         * @param _rightBitsToSkip The number of right bits to skip.
         * @return The bit mask.
         */
        uint8 CreateBitsMask(uint8 _nbBits, uint8 _rightBitsToSkip);
    }
}

#include "Utils.inl"