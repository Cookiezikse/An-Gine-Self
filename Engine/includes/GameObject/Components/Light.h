#pragma once
#include "Component.h"

/**
 * @brief Classe représentant une lumière.
 */
class Light final : public Component
{
public:
	Light()
	{
		name = "Light";
		type = "Light";
	}

#pragma region Event
	/**
	* @brief Initialise le module.
	*/
	void Init() override;

	/**
	 * @brief Démarre le module.
	 */
	void Start() override;

	/**
	 * @brief Effectue une mise à jour fixe du module.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met à jour le module.
	 */
	void Update() override;

	/**
	 * @brief Fonction pré-rendu du module.
	 */
	void PreRender() override;

	/**
	 * @brief Rendu du module.
	 */
	void Render() override;

	/**
	 * @brief Rendu de l'interface graphique du module.
	 */
	void RenderGui() override;

	/**
	 * @brief Fonction post-rendu du module.
	 */
	void PostRender() override;

	/**
	 * @brief Libère les ressources utilisées par le module.
	 */
	void Release() override;

	/**
	 * @brief Finalise le module.
	 */
	void Finalize() override;

	// Implémentation de la méthode toJson() pour convertir en JSON
	json toJson() const override {
		json j = Component::toJson(); // Appeler la méthode de la classe de base pour obtenir les données communes
		j["lightIntensity"] = lightIntensity;
		return j;
	}

	// Implémentation de la méthode fromJson() pour initialiser à partir de JSON
	void fromJson(const json& j) override {
		Component::fromJson(j); // Appeler la méthode de la classe de base pour initialiser les données communes
		lightIntensity = j["lightIntensity"];
	}
#pragma endregion

	float lightIntensity = 1.0f;
} ;
