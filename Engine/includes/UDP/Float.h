#pragma once

#include "Types.h"
#include "RangedInteger.h"

namespace Bousk
{
	/**
  * @brief Classe template représentant un nombre flottant avec une précision et une plage spécifiées.
  * @tparam FLOATTYPE Le type de données flottant (float32 ou float64).
  * @tparam MIN La valeur minimale autorisée.
  * @tparam MAX La valeur maximale autorisée.
  * @tparam NBDECIMALS Le nombre de décimales après la virgule.
  * @tparam STEP Le pas entre chaque valeur autorisée.
  */
	template<class FLOATTYPE, int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	class Float : public Serialization::Serializable
	{
		static_assert(std::is_same_v<FLOATTYPE, float32> || std::is_same_v<FLOATTYPE, float64>, "Float peut seulement être utilisé avec float32 ou float64");
		static_assert(NBDECIMALS > 0, "Au moins 1 décimale est nécessaire");
		static_assert(NBDECIMALS < 10, "Maximum de 10 décimales");
		static_assert(STEP != 0, "Le pas ne peut être nul");
		static_assert(STEP % 10 != 0, "Le pas ne peut être un multiple de 10. Retirez une décimale");
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
         * @brief Constructeur par défaut.
         */
        Float() = default;

        /**
         * @brief Constructeur prenant une valeur flottante en paramètre.
         * @param _value La valeur flottante à assigner.
         */
        Float(FloatType _value)
        {
            mQuantizedValue = Quantize(_value);
        }

        /**
         * @brief Quantifie une valeur flottante en un entier.
         * @param _value La valeur flottante à quantifier.
         * @return La valeur quantifiée.
         */
        static uint32 Quantize(FloatType _value)
        {
            assert(_value >= Min && _value <= Max);
            return static_cast<uint32>(((_value - Min) * Multiple) / Step);
        }

        /**
         * @brief Récupère la valeur flottante correspondante à la valeur quantifiée.
         * @return La valeur flottante correspondante.
         */
        inline FloatType get() const { return static_cast<FloatType>((mQuantizedValue.get() * Step * 1.) / Multiple + Min); }

        /**
         * @brief Conversion implicite vers le type de données flottant.
         * @return La valeur flottante correspondante.
         */
        inline operator FloatType() const { return get(); }

        /**
         * @brief Fonction de sérialisation pour écrire l'objet dans un flux.
         * @param _serializer Le sérialiseur utilisé pour écrire l'objet.
         * @return True si la sérialisation a réussi, sinon False.
         */
        bool write(Serialization::Serializer& _serializer) const override { return mQuantizedValue.write(_serializer); }

        /**
         * @brief Fonction de désérialisation pour lire l'objet depuis un flux.
         * @param _deserializer Le désérialiseur utilisé pour lire l'objet.
         * @return True si la désérialisation a réussi, sinon False.
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