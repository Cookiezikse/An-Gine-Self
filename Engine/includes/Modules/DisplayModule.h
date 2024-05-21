#pragma once
#include "Modules/Module.h"

class DisplayModule final : public Module
{

#pragma region Event

	/**
	 * @brief Initialise le module de débogage.
	 *
	 * Cette méthode est appelée pour initialiser le module de débogage.
	 */
	void Init() override;

	/**
	 * @brief Démarre le module de débogage.
	 *
	 * Cette méthode est appelée pour démarrer le module de débogage.
	 */
	void Start() override;

	/**
	 * @brief Met à jour le module de débogage à intervalles de temps fixes.
	 *
	 * Cette méthode est appelée à intervalles de temps fixes pour mettre à jour le module de débogage.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met à jour le module de débogage.
	 *
	 * Cette méthode est appelée pour mettre à jour le module de débogage.
	 */
	void Update() override;

	/**
	 * @brief Effectue des opérations de pré-rendu.
	 *
	 * Cette méthode est appelée pour effectuer des opérations de pré-rendu dans le module de débogage.
	 */
	void PreRender() override;

	/**
	 * @brief Rendu du module de débogage.
	 *
	 * Cette méthode est appelée pour effectuer le rendu du module de débogage.
	 */
	void Render() override;

	/**
	 * @brief Rendu des éléments de l'interface graphique du module de débogage.
	 *
	 * Cette méthode est appelée pour effectuer le rendu des éléments de l'interface graphique du module de débogage.
	 */
	void RenderGui() override;

	/**
	 * @brief Effectue des opérations de post-rendu.
	 *
	 * Cette méthode est appelée pour effectuer des opérations de post-rendu dans le module de débogage.
	 */
	void PostRender() override;

	/**
	 * @brief Libère les ressources utilisées par le module de débogage.
	 *
	 * Cette méthode est appelée pour libérer les ressources utilisées par le module de débogage.
	 */
	void Release() override;

	/**
	 * @brief Finalise le module de débogage.
	 *
	 * Cette méthode est appelée pour finaliser le module de débogage avant sa destruction.
	 */
	void Finalize() override;

#pragma endregion

};

