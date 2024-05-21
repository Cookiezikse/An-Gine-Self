#pragma once

#include "UDP/Settings.h"
#include "UDP/Types.h"

class Serialization_Test;
namespace Bousk
{
	namespace Serialization
	{
		class Serializable;
		class Serializer
		{
			friend class Serialization_Test;
		public:
			Serializer() = default;

			bool write(const Serializable& _serializable);

			bool write(uint8 _data, uint8 _minValue, uint8 _maxValue);
			bool write(uint16 _data, uint16 _minValue, uint16 _maxValue);
			bool write(uint32 _data, uint32 _minValue, uint32 _maxValue);
			bool write(uint64 _data, uint64 _minValue, uint64 _maxValue);
			inline bool write(uint8 _data) { return write(_data, std::numeric_limits<uint8>::min(), std::numeric_limits<uint8>::max()); }
			inline bool write(uint16 _data) { return write(_data, std::numeric_limits<uint16>::min(), std::numeric_limits<uint16>::max()); }
			inline bool write(uint32 _data) { return write(_data, std::numeric_limits<uint32>::min(), std::numeric_limits<uint32>::max()); }
			inline bool write(uint64 _data) { return write(_data, std::numeric_limits<uint64>::min(), std::numeric_limits<uint64>::max()); }

			bool write(int8 _data, int8 _minValue, int8 _maxValue);
			bool write(int16 _data, int16 _minValue, int16 _maxValue);
			bool write(int32 _data, int32 _minValue, int32 _maxValue);
			bool write(int64 _data, int64 _minValue, int64 _maxValue);
			inline bool write(int8 _data) { return write(_data, std::numeric_limits<int8>::min(), std::numeric_limits<int8>::max()); }
			inline bool write(int16 _data) { return write(_data, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max()); }
			inline bool write(int32 _data) { return write(_data, std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max()); }
			inline bool write(int64 _data) { return write(_data, std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max()); }

			inline bool write(bool _data) { return write(_data ? BoolTrue : BoolFalse, static_cast<uint8>(0), static_cast<uint8>(1)); }

			// To use if your enum has Min & Max entries
			template<class E>
			typename std::enable_if<std::is_enum<E>::value, bool>::type write(E _value)
			{
				using T = typename std::underlying_type<E>::type;
				return write(_value, E::Min, E::Max);
			}
			// Use this one to use custom min & max enum values
			template<class E>
			typename std::enable_if<std::is_enum<E>::value, bool>::type write(E _value, E _min, E _max)
			{
				using T = typename std::underlying_type<E>::type;
				return write(static_cast<T>(_value), static_cast<T>(_min), static_cast<T>(_max));
			}

#if BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED
			bool write(float32 _data);
#endif // BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED

			template<class T>
			inline bool write(const std::vector<T>& _data) { return writeContainer(_data); }
			inline bool write(const std::string& _data) { return writeContainer(_data); }

			inline const uint8* buffer() const { return mBuffer.data(); }
			inline size_t bufferSize() const { return mBuffer.size(); }

		private:
			bool writeBits(const uint8* _buffer, const uint8 _buffersize, const uint8 _nbBits);
			template<class CONTAINER>
			bool writeContainer(const CONTAINER& _container);

			// For std::string support
			inline bool write(char _data) { return write(*reinterpret_cast<uint8*>(&_data)); }

		private:
			std::vector<uint8> mBuffer;
			uint8 mUsedBits{ 0 };
		};
	}
}

#include "Serializer.inl"