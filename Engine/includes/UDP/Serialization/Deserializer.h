#pragma once

#include "UDP/Settings.h"
#include "UDP/Types.h"

#include <string>
#include <vector>

class Serialization_Test;
namespace Bousk
{
    namespace Serialization
    {
        /**
         * @brief Class responsible for deserializing data.
         */
        class Serializable;
        class Deserializer
        {
            friend class Serialization_Test;
        public:
            /**
             * @brief Constructs a new Deserializer object.
             * @param _buffer Pointer to the buffer containing the serialized data.
             * @param _bufferSize Size of the buffer.
             */
            Deserializer(const uint8* _buffer, const size_t _bufferSize)
                : mBuffer(_buffer)
                , mBufferSize(_bufferSize)
            {}

            bool read(Serializable& _data); /**< Reads data into a serializable object. */

            bool read(uint8& _data, uint8 _minValue, uint8 _maxValue); /**< Reads a uint8 value with specified minimum and maximum values. */
            bool read(uint16& _data, uint16 _minValue, uint16 _maxValue); /**< Reads a uint16 value with specified minimum and maximum values. */
            bool read(uint32& _data, uint32 _minValue, uint32 _maxValue); /**< Reads a uint32 value with specified minimum and maximum values. */
            bool read(uint64& _data, uint64 _minValue, uint64 _maxValue); /**< Reads a uint64 value with specified minimum and maximum values. */
            inline bool read(uint8& _data) { return read(_data, std::numeric_limits<uint8>::min(), std::numeric_limits<uint8>::max()); } /**< Reads a uint8 value. */
            inline bool read(uint16& _data) { return read(_data, std::numeric_limits<uint16>::min(), std::numeric_limits<uint16>::max()); } /**< Reads a uint16 value. */
            inline bool read(uint32& _data) { return read(_data, std::numeric_limits<uint32>::min(), std::numeric_limits<uint32>::max()); } /**< Reads a uint32 value. */
            inline bool read(uint64& _data) { return read(_data, std::numeric_limits<uint64>::min(), std::numeric_limits<uint64>::max()); } /**< Reads a uint64 value. */

            bool read(int8& _data, int8 _minValue, int8 _maxValue); /**< Reads an int8 value with specified minimum and maximum values. */
            bool read(int16& _data, int16 _minValue, int16 _maxValue); /**< Reads an int16 value with specified minimum and maximum values. */
            bool read(int32& _data, int32 _minValue, int32 _maxValue); /**< Reads an int32 value with specified minimum and maximum values. */
            bool read(int64& _data, int64 _minValue, int64 _maxValue); /**< Reads an int64 value with specified minimum and maximum values. */
            inline bool read(int8& _data) { return read(_data, std::numeric_limits<int8>::min(), std::numeric_limits<int8>::max()); } /**< Reads an int8 value. */
            inline bool read(int16& _data) { return read(_data, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max()); } /**< Reads an int16 value. */
            inline bool read(int32& _data) { return read(_data, std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max()); } /**< Reads an int32 value. */
            inline bool read(int64& _data) { return read(_data, std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max()); } /**< Reads an int64 value. */

            bool read(bool& _data); /**< Reads a boolean value. */

            // To use if your enum has Min & Max entries
            /**
             * @brief Reads an enum value.
             * @tparam E The enum type.
             * @param _data Reference to the variable to store the enum value.
             * @return True if successful, false otherwise.
             */
            template<class E>
            typename std::enable_if<std::is_enum<E>::value, bool>::type read(E& _data)
            {
                return read(_data, E::Min, E::Max);
            }
            /**
             * @brief Reads an enum value with custom minimum and maximum values.
             * @tparam E The enum type.
             * @param _data Reference to the variable to store the enum value.
             * @param _min The minimum value of the enum.
             * @param _max The maximum value of the enum.
             * @return True if successful, false otherwise.
             */
            template<class E>
            typename std::enable_if<std::is_enum<E>::value, bool>::type read(E& _data, E _min, E _max)
            {
                using T = typename std::underlying_type<E>::type;
                T temp;
                if (read(temp, static_cast<T>(_min), static_cast<T>(_max)))
                {
                    _data = static_cast<E>(temp);
                    return true;
                }
                return false;
            }

#if BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED
            bool read(float32& data); /**< Reads a float32 value. */
#endif // BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED

            /**
             * @brief Reads data into a vector.
             * @tparam T The type of data in the vector.
             * @param _data Reference to the vector to store the data.
             * @return True if successful, false otherwise.
             */
            template<class T>
            inline bool read(std::vector<T>& _data) { return readContainer(_data); }
            /**
             * @brief Reads data into a string.
             * @param _data Reference to the string to store the data.
             * @return True if successful, false otherwise.
             */
            inline bool read(std::string& _data) { return readContainer(_data); }

        private:
            bool readBits(uint8 _nbBits, uint8* _buffer, uint8 _bufferSize); /**< Reads a specified number of bits from the buffer. */
            template<class CONTAINER>
            bool readContainer(CONTAINER& _container); /**< Reads data into a container. */

            /**
             * @brief Calculates the number of remaining bytes in the buffer.
             * @return The number of remaining bytes.
             */
            inline size_t remainingBytes() const { return mBufferSize - mBytesRead; }
            /**
             * @brief Calculates the size of the buffer in bits.
             * @return The size of the buffer in bits.
             */
            inline size_t bufferSizeBits() const { return mBufferSize * 8; }
            /**
             * @brief Calculates the number of bits read from the buffer.
             * @return The number of bits read from the buffer.
             */
            inline size_t bufferReadBits() const { return mBytesRead * 8 + mBitsRead; }
            /**
             * @brief Calculates the number of remaining bits in the buffer.
             * @return The number of remaining bits.
             */
            inline size_t remainingBits() const { return bufferSizeBits() - bufferReadBits(); }

            // For std::string support
            /**
             * @brief Reads a character.
             * @param _data Reference to the character to store the data.
             * @return True if successful, false otherwise.
             */
            inline bool read(char& _data) { return read(reinterpret_cast<uint8&>(_data)); }

        private:
            const uint8* mBuffer; /**< Pointer to the buffer containing the serialized data. */
            const size_t mBufferSize; /**< Size of the buffer. */
            size_t mBytesRead{ 0 }; /**< Number of bytes read from the buffer. */
            uint8 mBitsRead{ 0 }; /**< Number of bits read from the buffer. */
        };

    }
}

#include <UDP/Serialization/Deserializer.inl>
