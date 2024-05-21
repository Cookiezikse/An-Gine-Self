#pragma once

#include <functional>

namespace lve
{
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>

	/**
	 * @brief Combine plusieurs valeurs pour générer un hash.
	 *
	 * Cette fonction combine plusieurs valeurs pour générer un hash en utilisant l'algorithme
	 * de mixage de MurmurHash3. Chaque valeur est hashée individuellement et combinée avec
	 * le hash précédent en utilisant une opération XOR et des rotations de bits.
	 *
	 * @tparam T Type de la première valeur à hasher.
	 * @tparam Rest Types des valeurs restantes à hasher.
	 * @param seed Référence vers le seed du hash.
	 * @param v La première valeur à hasher.
	 * @param rest Les valeurs restantes à hasher.
	 */
	void HashCombine(std::size_t& _seed, const T& _v, const Rest&... _rest)
	{
		_seed ^= std::hash<T>{}(_v) + 0x9e3779b9 + (_seed << 6) + (_seed >> 2);
		(HashCombine(_seed, _rest), ...);
	};
} // namespace lve
