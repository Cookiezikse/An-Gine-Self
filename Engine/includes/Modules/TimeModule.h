#pragma once
#include <chrono>

#include "Modules/Module.h"

/**
 * @brief Module de gestion du temps pour le moteur de jeu.
 */
class TimeModule : public Module
{
	public:

		/**
		 * @brief Constructeur par défaut de la classe TimeModule.
		 */
		TimeModule() = default;

		/**
		 * @brief Récupère le delta time en secondes.
		 * @return Le delta time en secondes.
		 */
		[[nodiscard]] static float GetDeltaTime()
		{
			return deltaTime;
		}

		/**
		 * @brief Récupère le temps écoulé depuis la dernière frame en secondes.
		 * @return Le temps écoulé depuis la dernière frame en secondes.
		 */
		[[nodiscard]] float GetElapsedTime() const
		{
			const auto                         current_time = std::chrono::high_resolution_clock::now();
			const std::chrono::duration<float> duration     = current_time - currentTime;
			return duration.count();
		}

		/**
		 * @brief Récupère le delta time en millisecondes.
		 * @return Le delta time en millisecondes.
		 */
		[[nodiscard]] static float GetDeltaTimeMilliseconds()
		{
			return deltaTime * 1000.0f;
		}

		/**
		 * @brief Récupère le delta time en secondes.
		 * @return Le delta time en secondes.
		 */
		[[nodiscard]] static float GetDeltaTimeSeconds()
		{
			return deltaTime;
		}

		/**
		 * @brief Récupère le delta time en minutes.
		 * @return Le delta time en minutes.
		 */
		[[nodiscard]] static float GetDeltaTimeMinutes()
		{
			return deltaTime / 60.0f;
		}

		/**
		 * @brief Récupère le delta time en heures.
		 * @return Le delta time en heures.
		 */
		[[nodiscard]] static float GetDeltaTimeHours()
		{
			return deltaTime / 3600.0f;
		}

		/**
		 * @brief Réinitialise le temps.
		 */
		void Reset()
		{
			currentTime = std::chrono::high_resolution_clock::now();
			beginTime   = std::chrono::high_resolution_clock::now();
			deltaTime   = 0.0f;
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
		 * @brief Met à jour le temps écoulé depuis la dernière frame.
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

#pragma endregion

		static float deltaTime;

	private:
		std::chrono::high_resolution_clock::time_point beginTime;
		std::chrono::high_resolution_clock::time_point currentTime;
};
