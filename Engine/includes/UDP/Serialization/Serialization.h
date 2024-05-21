#pragma once

namespace Bousk
{
    namespace Serialization
    {
        /**
         * @brief Interface for serializable objects.
         */
        class Serializer;
        class Deserializer;
        class Serializable
        {
        public:
            virtual ~Serializable() = default; /**< Virtual destructor. */

            /**
             * @brief Writes the object to a serializer.
             * @param serializer The serializer to write to.
             * @return True if successful, false otherwise.
             */
            virtual bool write(Serializer&) const = 0;

            /**
             * @brief Reads the object from a deserializer.
             * @param deserializer The deserializer to read from.
             * @return True if successful, false otherwise.
             */
            virtual bool read(Deserializer&) = 0;
        };
    }
}