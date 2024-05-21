#pragma once
#include <iostream>

#include "Modules/Module.h"


/**
 * @brief Classe représentant un module de débogage.
 *
 * Cette classe hérite de la classe Module et permet de gérer les fonctionnalités de débogage dans le jeu.
 */
class DebugModule : public Module
{
	public:

		/**
	 * @brief Destructeur virtuel de la classe DebugModule.
	 *
	 * Ce destructeur virtuel permet une destruction appropriée des objets de la classe DebugModule lorsqu'ils sont utilisés polymorphiquement.
	 */
		virtual ~DebugModule();

		/**
		 * @brief Énumération représentant les types de journaux.
		 *
		 * Cette énumération définit les différents types de journaux utilisés pour les messages de débogage.
		 */
		enum class TypeLog
		{
			INFO,
			WARNING,
			ERROR,
			SUCCESS
		};

#pragma region Logs

		/**
		 * @brief Enregistre un message de journal avec un niveau de priorité spécifié.
		 *
		 * Cette méthode statique enregistre un message de journal avec un niveau de priorité spécifié.
		 *
		 * @param _message Le message à enregistrer dans le journal.
		 * @param _level Le niveau de priorité du message (par défaut: TypeLog::INFO).
		 *        Les valeurs possibles sont TypeLog::INFO, TypeLog::WARNING, TypeLog::ERROR et TypeLog::SUCCESS.
		 */
		static void Log(const std::string& _message, const TypeLog _level = TypeLog::INFO);

		/**
		 * @brief Enregistre un message d'erreur dans le journal.
		 *
		 * Cette méthode statique enregistre un message d'erreur dans le journal avec le niveau de priorité TypeLog::ERROR.
		 *
		 * @param _message Le message d'erreur à enregistrer dans le journal.
		 */
		static void LogError(const std::string& _message)
		{
			Log(_message, TypeLog::ERROR);
		}

		/**
		 * @brief Enregistre un message d'avertissement dans le journal.
		 *
		 * Cette méthode statique enregistre un message d'avertissement dans le journal avec le niveau de priorité TypeLog::WARNING.
		 *
		 * @param _message Le message d'avertissement à enregistrer dans le journal.
		 */
		static void LogWarning(const std::string& _message)
		{
			Log(_message, TypeLog::WARNING);
		}

		/**
		 * @brief Enregistre un message d'information dans le journal.
		 *
		 * Cette méthode statique enregistre un message d'information dans le journal avec le niveau de priorité TypeLog::INFO.
		 *
		 * @param _message Le message d'information à enregistrer dans le journal.
		 */
		static void LogInfo(const std::string& _message)
		{
			Log(_message, TypeLog::INFO);
		}

		/**
		 * @brief Enregistre un message de réussite dans le journal.
		 *
		 * Cette méthode statique enregistre un message de réussite dans le journal avec le niveau de priorité TypeLog::SUCCESS.
		 *
		 * @param _message Le message de réussite à enregistrer dans le journal.
		 */
		static void LogSuccess(const std::string& _message)
		{
			Log(_message, TypeLog::SUCCESS);
		}

#pragma endregion

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

#pragma endregion

};
