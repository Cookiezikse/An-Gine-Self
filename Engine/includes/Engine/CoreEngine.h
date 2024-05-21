#pragma once

#include "Modules/ModuleManager.h"

#ifdef ENGINECORE_EXPORTS
#define ENGINEDLL_API __declspec(dllexport)
#else
#define ENGINEDLL_API __declspec(dllimport)
#endif

enum EngineMode
{
	Editor,
	Play,
	Pause
};

//class ENGINEDLL_API Engine
class Engine
{
	public:

		/**
		 * @brief Obtient une instance unique du moteur.
		 * @return Un pointeur vers l'instance du moteur.
		 */
		static Engine* GetInstance();

		/**
		 * @brief Initialise le moteur.
		 */
		void Init() const;

		/**
		 * @brief Lance le moteur.
		 */
		void Run() const;

		/**
		 * @brief Quitte le moteur.
		 */
		void Quit() { shouldQuit = true; }

		/**
		 * @brief Obtient le gestionnaire de modules du moteur.
		 * @return Un pointeur vers le gestionnaire de modules.
		 */
		[[nodiscard]] ModuleManager* GetModuleManager() const { return moduleManager; }

		/**
		 * @brief Obtient le mode du moteur.
		 * @return Le mode actuel du moteur.
		 */
		[[nodiscard]] EngineMode GetEngineMode() const { return engineMode; }

		/**
		 * @brief Définit le mode du moteur.
		 * @param _engineMode Le nouveau mode du moteur.
		 */
		void SetEngineMode(const EngineMode _engineMode) { engineMode = _engineMode; }

		/**
		 * @brief Définit le mode du moteur comme "Play".
		 */
		void PlayEngineMode() { engineMode = Play; }

		/**
		 * @brief Définit le mode du moteur comme "Pause".
		 */
		void PauseEngineMode() { engineMode = Pause; }

		/**
		 * @brief Définit le mode du moteur comme "Editor".
		 */
		void EditorEngineMode() { engineMode = Editor; }
		

	private:
		static Engine* instance; ///< Instance unique de la classe Engine.
		ModuleManager* moduleManager = new ModuleManager; ///< Gestionnaire de modules du moteur.
		bool shouldQuit = false; ///< Indique si le moteur devrait quitter.
		EngineMode engineMode = Editor; ///< Mode actuel du moteur.
};
