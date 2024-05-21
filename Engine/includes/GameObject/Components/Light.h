#pragma once
#include "Component.h"

/**
 * @brief Classe repr�sentant une lumi�re.
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
	 * @brief D�marre le module.
	 */
	void Start() override;

	/**
	 * @brief Effectue une mise � jour fixe du module.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met � jour le module.
	 */
	void Update() override;

	/**
	 * @brief Fonction pr�-rendu du module.
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
	 * @brief Lib�re les ressources utilis�es par le module.
	 */
	void Release() override;

	/**
	 * @brief Finalise le module.
	 */
	void Finalize() override;

	// Impl�mentation de la m�thode toJson() pour convertir en JSON
	json toJson() const override {
		json j = Component::toJson(); // Appeler la m�thode de la classe de base pour obtenir les donn�es communes
		j["lightIntensity"] = lightIntensity;
		return j;
	}

	// Impl�mentation de la m�thode fromJson() pour initialiser � partir de JSON
	void fromJson(const json& j) override {
		Component::fromJson(j); // Appeler la m�thode de la classe de base pour initialiser les donn�es communes
		lightIntensity = j["lightIntensity"];
	}
#pragma endregion

	float lightIntensity = 1.0f;
} ;
