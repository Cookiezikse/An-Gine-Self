#pragma once

#include "UDP/Types.h"

namespace Bousk
{
    namespace Serialization
    {
        namespace Conversion
        {
            /**
             * @brief Converts a 16-bit unsigned integer from host to network byte order.
             * @param _in The input value in host byte order.
             * @param _out The output value in network byte order.
             */
            void ToNetwork(uint16 _in, uint16& _out);

            /**
             * @brief Converts a 32-bit unsigned integer from host to network byte order.
             * @param _in The input value in host byte order.
             * @param _out The output value in network byte order.
             */
            void ToNetwork(uint32 _in, uint32& _out);

            /**
             * @brief Converts a 64-bit unsigned integer from host to network byte order.
             * @param _in The input value in host byte order.
             * @param _out The output value in network byte order.
             */
            void ToNetwork(uint64 _in, uint64& _out);

            /**
             * @brief Converts a 32-bit floating point value from host to network byte order.
             * @param _in The input value in host byte order.
             * @param _out The output value in network byte order.
             */
            void ToNetwork(float32 _in, uint32& _out);

            /**
             * @brief Converts a 16-bit unsigned integer from host to network byte order and stores it in an array of 2 bytes.
             * @param _in The input value in host byte order.
             * @param _out The output array where the network byte order value will be stored.
             */
            void ToNetwork(uint16 _in, uint8 _out[2]);

            /**
             * @brief Converts a 32-bit unsigned integer from host to network byte order and stores it in an array of 4 bytes.
             * @param _in The input value in host byte order.
             * @param _out The output array where the network byte order value will be stored.
             */
            void ToNetwork(uint32 _in, uint8 _out[4]);

            /**
             * @brief Converts a 64-bit unsigned integer from host to network byte order and stores it in an array of 8 bytes.
             * @param _in The input value in host byte order.
             * @param _out The output array where the network byte order value will be stored.
             */
            void ToNetwork(uint64 _in, uint8 _out[8]);

            /**
             * @brief Converts a 16-bit unsigned integer from network to host byte order.
             * @param _in The input value in network byte order.
             * @param _out The output value in host byte order.
             */
            void ToLocal(uint16 _in, uint16& _out);

            /**
             * @brief Converts a 32-bit unsigned integer from network to host byte order.
             * @param _in The input value in network byte order.
             * @param _out The output value in host byte order.
             */
            void ToLocal(uint32 _in, uint32& _out);

            /**
             * @brief Converts a 64-bit unsigned integer from network to host byte order.
             * @param _in The input value in network byte order.
             * @param _out The output value in host byte order.
             */
            void ToLocal(uint64 _in, uint64& _out);

            /**
             * @brief Converts a 32-bit unsigned integer from network to host byte order and interprets it as a floating point value.
             * @param _in The input value in network byte order.
             * @param _out The output value in host byte order.
             */
            void ToLocal(uint32 _in, float32& _out);

            /**
             * @brief Converts a 16-bit unsigned integer from an array of 2 bytes in network byte order to host byte order.
             * @param _in The input array containing the network byte order value.
             * @param _out The output value in host byte order.
             */
            void ToLocal(const uint8 _in[2], uint16& _out);

            /**
             * @brief Converts a 32-bit unsigned integer from an array of 4 bytes in network byte order to host byte order.
             * @param _in The input array containing the network byte order value.
             * @param _out The output value in host byte order.
             */
            void ToLocal(const uint8 _in[4], uint32& _out);

            /**
             * @brief Converts a 64-bit unsigned integer from an array of 8 bytes in network byte order to host byte order.
             * @param _in The input array containing the network byte order value.
             * @param _out The output value in host byte order.
             */
            void ToLocal(const uint8 _in[8], uint64& _out);
        }
    }
}