#pragma once

#include "Types.h"
#include "RangedInteger.h"

namespace Bousk
{
	/**
  * @brief Classe template repr�sentant un nombre flottant avec une pr�cision et une plage sp�cifi�es.
  * @tparam FLOATTYPE Le type de donn�es flottant (float32 ou float64).
  * @tparam MIN La valeur minimale autoris�e.
  * @tparam MAX La valeur maximale autoris�e.
  * @tparam NBDECIMALS Le nombre de d�cimales apr�s la virgule.
  * @tparam STEP Le pas entre chaque valeur autoris�e.
  */
	template<class FLOATTYPE, int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	class Float : public Serialization::Serializable
	{
		static_assert(std::is_same_v<FLOATTYPE, float32> || std::is_same_v<FLOATTYPE, float64>, "Float peut seulement �tre utilis� avec float32 ou float64");
		static_assert(NBDECIMALS > 0, "Au moins 1 d�cimale est n�cessaire");
		static_assert(NBDECIMALS < 10, "Maximum de 10 d�cimales");
		static_assert(STEP != 0, "Le pas ne peut �tre nul");
		static_assert(STEP % 10 != 0, "Le pas ne peut �tre un multiple de 10. Retirez une d�cimale");
		using FloatType = FLOATTYPE;
		static constexpr int32 Min = MIN;
		static constexpr int32 Max = MAX;
		static constexpr uint32 Diff = Max - Min;
		static constexpr uint8 NbDecimals = NBDECIMALS;
		static constexpr uint32 Multiple = Pow<10, NbDecimals>::Value;
		static constexpr uint8 Step = STEP;
		static constexpr uint32 Domain = (MAX - MIN) * Multiple / STEP;

    public:
        /**
         * @brief Constructeur par d�faut.
         */
        Float() = default;

        /**
         * @brief Constructeur prenant une valeur flottante en param�tre.
         * @param _value La valeur flottante � assigner.
         */
        Float(FloatType _value)
        {
            mQuantizedValue = Quantize(_value);
        }

        /**
         * @brief Quantifie une valeur flottante en un entier.
         * @param _value La valeur flottante � quantifier.
         * @return La valeur quantifi�e.
         */
        static uint32 Quantize(FloatType _value)
        {
            assert(_value >= Min && _value <= Max);
            return static_cast<uint32>(((_value - Min) * Multiple) / Step);
        }

        /**
         * @brief R�cup�re la valeur flottante correspondante � la valeur quantifi�e.
         * @return La valeur flottante correspondante.
         */
        inline FloatType get() const { return static_cast<FloatType>((mQuantizedValue.get() * Step * 1.) / Multiple + Min); }

        /**
         * @brief Conversion implicite vers le type de donn�es flottant.
         * @return La valeur flottante correspondante.
         */
        inline operator FloatType() const { return get(); }

        /**
         * @brief Fonction de s�rialisation pour �crire l'objet dans un flux.
         * @param _serializer Le s�rialiseur utilis� pour �crire l'objet.
         * @return True si la s�rialisation a r�ussi, sinon False.
         */
        bool write(Serialization::Serializer& _serializer) const override { return mQuantizedValue.write(_serializer); }

        /**
         * @brief Fonction de d�s�rialisation pour lire l'objet depuis un flux.
         * @param _deserializer Le d�s�rialiseur utilis� pour lire l'objet.
         * @return True si la d�s�rialisation a r�ussi, sinon False.
         */
        bool read(Serialization::Deserializer& _deserializer) override { return mQuantizedValue.read(_deserializer); }

	private:
		RangedInteger<0, Domain> mQuantizedValue;
	};

	template<int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	using Float32 = Float<float32, MIN, MAX, NBDECIMALS, STEP>;
	template<int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	using Float64 = Float<float64, MIN, MAX, NBDECIMALS, STEP>;
}