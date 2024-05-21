#pragma once

#include "Types.h"
#include "Serialization/Serialization.h"
#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"
#include "Utils/Utils.h"

namespace Bousk
{
	template<auto V, bool ForceUnsigned>
	struct ExtractType
	{
		static_assert(std::numeric_limits<decltype(V)>::is_integer, "ExtractType doit être utilisé avec des entiers");
		static_assert(!std::is_same_v<decltype(V), bool>, "ExtractType ne doit pas être utilisé avec bool");
		using Type = std::conditional_t < V < 0
			, std::conditional_t < V < std::numeric_limits<int32>::min(), int64
			, std::conditional_t< V < std::numeric_limits<int16>::min(), int32
			, std::conditional_t< V < std::numeric_limits<int8>::min(), int16
			, int8
			>>>
			// > 0 : doit-on forcer un type non signé ?
			, std::conditional_t< (V > std::numeric_limits<int64>::max()), uint64
			, std::conditional_t< (V > std::numeric_limits<uint32>::max()), std::conditional_t<ForceUnsigned, uint64, int64>
			, std::conditional_t< (V > std::numeric_limits<int32>::max()), std::conditional_t<ForceUnsigned, uint32, int64>
			, std::conditional_t< (V > std::numeric_limits<uint16>::max()), std::conditional_t<ForceUnsigned, uint32, int32>
			, std::conditional_t< (V > std::numeric_limits<int16>::max()), std::conditional_t<ForceUnsigned, uint16, int32>
			, std::conditional_t< (V > std::numeric_limits<uint8>::max()), std::conditional_t<ForceUnsigned, uint16, int16>
			, std::conditional_t< (V > std::numeric_limits<int8>::max()), std::conditional_t<ForceUnsigned, uint8, int16>
			, std::conditional_t<ForceUnsigned, uint8, int8>
			>>>>>>>
			>;
	};

	// Promeut un type vers un type plus large (int32 => int64, uint8 => uint16...)
	template<class T>
	struct Promote { using Type = std::conditional_t<std::is_signed_v<T>, int64, uint64>; };// Par défaut, utiliser le type le plus large possible. Ne devrait pas être utilisé mais nécessaire pour compiler.
	template<> struct Promote<int32> { using Type = int64; };
	template<> struct Promote<uint32> { using Type = uint64; };
	template<> struct Promote<int16> { using Type = int32; };
	template<> struct Promote<uint16> { using Type = uint32; };
	template<> struct Promote<int8> { using Type = int16; };
	template<> struct Promote<uint8> { using Type = uint16; };

	// Retourne le type le plus large
	template<class A, class B>
	struct Biggest
	{
		using Type = std::conditional_t<(sizeof(A) > sizeof(B)), A, B>;
	};
	
	template<class A, class B>
	struct HoldingType
	{
		// Est-ce qu’un tel type existe ?
		// Un cas est impossible : si l’un est uint64 alors que l’autre est signé
		static constexpr bool IsPossible = !((std::is_same_v<A, uint64> && std::is_signed_v<B>) || (std::is_same_v<B, uint64> && std::is_signed_v<A>));

		using Type = typename std::conditional_t<!IsPossible, void
			// Ce sont les mêmes types : utilisons-le
			, std::conditional_t<std::is_same_v<A, B>, A
			// Tous deux signés ou non : utiliser le plus large type
			, std::conditional_t<std::is_signed_v<A> == std::is_signed_v<B>, typename Biggest<A, B>::Type
			// Le plus large est signé, utilisons-le
			, std::conditional_t<std::is_signed_v<typename Biggest<A, B>::Type>, typename Biggest<A, B>::Type
			// Sinon, utiliser le type signé plus large que le plus large des deux
			, std::make_signed_t<typename Promote<typename Biggest<A, B>::Type>::Type>
			>>>>;
	};

	template<auto MIN, auto MAX>
	struct FittingType
	{
		static_assert(MIN < MAX);

		using MinType = typename ExtractType<MIN, (MIN >= 0)>::Type;
		using MaxType = typename ExtractType<MAX, (MIN >= 0)>::Type;
		// Si nous avons une valeur < 0 et l’autre > int64 max, impossible de trouver un type pouvant les accueillir
		static constexpr bool IsPossible = !(MIN < 0 && MAX > std::numeric_limits<int64>::max()) || HoldingType<MinType, MaxType>::IsPossible;

		using Type =
			std::conditional_t<!IsPossible, void,
			// Can MAX be hold by MinType ?
			std::conditional_t<(MAX <= std::numeric_limits<MinType>::max()), MinType,
			// Can MIN be hold by MaxType ?
			std::conditional_t<(MIN >= std::numeric_limits<MaxType>::min()), MaxType,
			// If not, find a type big enough to accomodate both values
			typename HoldingType<MinType, MaxType>::Type
			>>>;
	};

	template<auto MIN, auto MAX>
	constexpr uint64 Range()
	{
		static_assert(MIN < MAX);
		if constexpr (MAX < 0)
		{
			// MIN & MAX < 0
			return static_cast<uint64>(static_cast<int64>(-1) * MIN) - static_cast<uint64>(static_cast<int64>(-1) * MAX);
		}
		else if constexpr (MIN < 0)
		{
			#pragma warning(push)
			#pragma warning(disable: 4307) // '*': signed integral constant overflow
			return static_cast<uint64>(MAX) + static_cast<uint64>(static_cast<int64>(-1) * MIN);
			#pragma warning(pop)
		}
		else
		{
			return static_cast<uint64>(MAX) - static_cast<uint64>(MIN);
		}
	}

	template<auto MIN, auto MAX>
	class RangedInteger : public Serialization::Serializable
	{
		static_assert(MIN < MAX, "MIN et MAX doivent être strictement ordonnées");
		static_assert(FittingType<MIN, MAX>::IsPossible, "Impossible avec ces valeurs de Min & Max");
	public:
		using Type = typename FittingType<MIN, MAX>::Type;
		static constexpr Type Min() { return MIN; }
		static constexpr Type Max() { return MAX; }
		static constexpr uint64 Range = Range<MIN, MAX>();
		static constexpr uint8 NbBits = NbBits<Range>::Value;

		RangedInteger() = default;
		explicit RangedInteger(Type _v) : mValue(_v) { checkValue(); }
		RangedInteger& operator=(Type _v) { CheckValue(_v); mValue = _v; return *this; }
		template<typename OtherType>
		RangedInteger(OtherType _v) { CheckValue(_v); mValue = static_cast<Type>(_v); }
		template<typename OtherType>
		RangedInteger& operator=(OtherType _v) { CheckValue(_v); mValue = static_cast<Type>(_v); return *this; }

		static constexpr bool IsWithinRange(Type _v) { return (_v >= Min() && _v <= Max()); }
		template<typename OtherType>
		static constexpr bool IsWithinRange(OtherType _v)
		{
			if constexpr (!HoldingType<Type, OtherType>::IsPossible)
			{
				return false;
			}
			else
			{
				using CastType = typename HoldingType<Type, OtherType>::Type;
				return (static_cast<CastType>(_v) >= static_cast<CastType>(Min()) && static_cast<CastType>(_v) <= static_cast<CastType>(Max()));
			}
		}

		inline Type get() const { return mValue; }
		inline operator Type() const { return mValue; }

		bool write(Serialization::Serializer& _serializer) const override { return _serializer.write(get(), Min(), Max()); }
		bool read(Serialization::Deserializer& _deserializer) override { return _deserializer.read(mValue, Min(), Max()); }

	private:
		void checkValue() { assert(IsWithinRange(mValue)); }
		static void CheckValue(Type _v) { assert(IsWithinRange(_v)); }
		template<typename OtherType>
		static void CheckValue(OtherType _v) { assert(IsWithinRange(_v)); }

	private:
		Type mValue{ Min() };
	};

	using UInt8 = RangedInteger<std::numeric_limits<uint8>::min(), std::numeric_limits<uint8>::max()>;
	using UInt16 = RangedInteger<std::numeric_limits<uint16>::min(), std::numeric_limits<uint16>::max()>;
	using UInt32 = RangedInteger<std::numeric_limits<uint32>::min(), std::numeric_limits<uint32>::max()>;
	using UInt64 = RangedInteger<std::numeric_limits<uint64>::min(), std::numeric_limits<uint64>::max()>;

	using Int8 = RangedInteger<std::numeric_limits<int8>::min(), std::numeric_limits<int8>::max()>;
	using Int16 = RangedInteger<std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max()>;
	using Int32 = RangedInteger<std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max()>;
	using Int64 = RangedInteger<std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max()>;
}