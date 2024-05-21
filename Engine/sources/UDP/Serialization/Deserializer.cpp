#include "UDP/Serialization/Deserializer.h"
#include "UDP/Serialization/Serialization.h"
#include "UDP/Serialization/Convert.h"
#include "UDP/utils/Utils.h"

#include <algorithm>

namespace Bousk
{
	namespace Serialization
	{
		bool Deserializer::read(Serializable& data)
		{
			return data.read(*this);
		}

		bool Deserializer::readBits(const uint8 nbBits, uint8* const buffer, const uint8 bufferSize)
		{
			static_assert(CHAR_BIT == 8, "");
			assert(nbBits <= bufferSize * 8);
			if (remainingBits() < nbBits)
				return false;

			uint8_t totalReadBits = 0;
			// Le tampon doit être en big-endian (réseau), donc les octets doivent être écrits de droite (buffer + bufferSize - 1) à gauche (buffer)
			for (uint8_t writingBytesOffset = 1; writingBytesOffset <= bufferSize && totalReadBits < nbBits; ++writingBytesOffset)
			{
				uint8_t& dstByte = *(buffer + bufferSize - writingBytesOffset);
				const uint8_t bitsToRead = static_cast<uint8_t>(std::min(8, nbBits - totalReadBits));
				uint8_t bitsRead = 0;
				{
					const uint8_t srcByte = *(mBuffer + mBytesRead);
					// Lire les premiers bits du byte de lecture actuel
					const uint8_t remainingBitsInCurrentByte = 8 - mBitsRead;
					const uint8_t leftBitsToSkip = mBitsRead;
					const uint8_t bitsToReadFromCurrentByte = std::min(bitsToRead, remainingBitsInCurrentByte);
					const uint8_t remainingBitsOnTheRight = 8 - bitsToReadFromCurrentByte - leftBitsToSkip;
					// Extraire les bits de gauche
					const uint8_t readMask = Utils::CreateBitsMask(bitsToReadFromCurrentByte, remainingBitsOnTheRight);
					const uint8_t bits = srcByte & readMask;
					// Aligner ces bits à droite dans le byte de sortie
					const uint8_t bitsAlignedRight = bits >> remainingBitsOnTheRight;
					dstByte |= bitsAlignedRight;

					bitsRead += bitsToReadFromCurrentByte;
					mBitsRead += bitsToReadFromCurrentByte;
					mBytesRead += mBitsRead / 8;
					mBitsRead %= 8;
				}

				if (bitsRead < bitsToRead)
				{
					const uint8_t srcByte = *(mBuffer + mBytesRead);
					// Lire les bits restants du byte de sortie actuel depuis le byte de lecture suivant
					const uint8_t bitsToReadFromCurrentByte = bitsToRead - bitsRead;
					const uint8_t remainingBitsOnTheRight = 8 - bitsToReadFromCurrentByte;
					// Ces bits sont à gauche
					const uint8_t readMask = Utils::CreateBitsMask(bitsToReadFromCurrentByte, remainingBitsOnTheRight);
					const uint8_t bits = srcByte & readMask;
					// Les aligner à droite pour les emballer à la première partie lue
					const uint8_t bitsAlignedRightToPack = bits >> (8 - bitsToReadFromCurrentByte - bitsRead);
					dstByte |= bitsAlignedRightToPack;

					bitsRead += bitsToReadFromCurrentByte;
					mBitsRead += bitsToReadFromCurrentByte;
					mBytesRead += mBitsRead / 8;
					mBitsRead %= 8;
				}

				// Mettre à jour les compteurs
				totalReadBits += bitsRead;
			}

			return totalReadBits == nbBits;
		}

		bool Deserializer::read(uint8& data, const uint8 minValue, const uint8 maxValue)
		{
			assert(minValue < maxValue);
			const uint8 range = maxValue - minValue;
			uint8 bytesRead = 0;
			if (readBits(Utils::CountNeededBits(range), &bytesRead, 1))
			{
				data = bytesRead;
				if (data <= range)
				{
					data += minValue;
					return true;
				}
			}
			return false;
		}
		bool Deserializer::read(uint16& data, const uint16 minValue, const uint16 maxValue)
		{
			assert(minValue < maxValue);
			const uint16 range = maxValue - minValue;
			uint8 bytesRead[2]{ 0 };
			if (readBits(Utils::CountNeededBits(range), bytesRead, 2))
			{
				Conversion::ToLocal(bytesRead, data);
				if (data <= range)
				{
					data += minValue;
					return true;
				}
			}
			return false;
		}
		bool Deserializer::read(uint32& data, const uint32 minValue, const uint32 maxValue)
		{
			assert(minValue < maxValue);
			const uint32 range = maxValue - minValue;
			uint8 bytesRead[4]{ 0 };
			if (readBits(Utils::CountNeededBits(range), bytesRead, 4))
			{
				Conversion::ToLocal(bytesRead, data);
				if (data <= range)
				{
					data += minValue;
					return true;
				}
			}
			return false;
		}

		bool Deserializer::read(uint64& data, uint64 minValue, uint64 maxValue)
		{
			assert(minValue < maxValue);
			const uint64 range = maxValue - minValue;
			uint8 bytesRead[8]{ 0 };
			if (readBits(Utils::CountNeededBits(range), bytesRead, 8))
			{
				Conversion::ToLocal(bytesRead, data);
				if (data <= range)
				{
					data += minValue;
					return true;
				}
			}
			return false;
		}

		bool Deserializer::read(int8& data, const int8 minValue, const int8 maxValue)
		{
			static_assert(sizeof(int8) == sizeof(uint8), "");
			assert(minValue < maxValue);
			const uint8 range = static_cast<uint8>(maxValue - minValue);
			if (read(reinterpret_cast<uint8&>(data), 0, range))
			{
				data += minValue;
				return true;
			}
			return false;
		}
		bool Deserializer::read(int16& data, const int16 minValue, const int16 maxValue)
		{
			static_assert(sizeof(int16) == sizeof(uint16), "");
			assert(minValue < maxValue);
			const uint16 range = static_cast<uint16>(maxValue - minValue);
			if (read(reinterpret_cast<uint16&>(data), 0, range))
			{
				data += minValue;
				return true;
			}
			return false;
		}
		bool Deserializer::read(int32& data, const int32 minValue, const int32 maxValue)
		{
			static_assert(sizeof(int32) == sizeof(uint32), "");
			assert(minValue < maxValue);
			const uint32 range = static_cast<uint32>(maxValue - minValue);
			if (read(reinterpret_cast<uint32&>(data), 0, range))
			{
				data += minValue;
				return true;
			}
			return false;
		}
		bool Deserializer::read(int64& data, int64 minValue, int64 maxValue)
		{
			static_assert(sizeof(int64) == sizeof(uint64), "");
			assert(minValue < maxValue);
			const uint64 range = static_cast<uint64>(maxValue - minValue);
			if (read(reinterpret_cast<uint64&>(data), 0, range))
			{
				data += minValue;
				return true;
			}
			return false;
		}

		bool Deserializer::read(bool& data)
		{
			uint8 byteRead;
			if (read(byteRead, 0, 1))
			{
				data = (byteRead == BoolTrue);
				return true;
			}
			return false;
		}

#if BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED
		bool Deserializer::read(float32& data)
		{
			uint32 conv = 0;
			if (readBits(32, reinterpret_cast<uint8*>(&conv), 4))
			{
				Conversion::ToLocal(conv, data);
				return true;
			}
			return false;
		}
#endif // BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED
	}
}