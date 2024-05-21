#pragma once

#include <vector>
#include "Modules/Module.h"

/**
 * @brief Classe ModuleManager.
 *
 * Cette classe représente le gestionnaire de modules dans le système.
 * Le ModuleManager est responsable de la création, de la gestion et de la communication entre les différents modules de l'application.
 */
class ModuleManager
{
	public:
		ModuleManager() = default;
		~ModuleManager();

		/**
		 * @brief Crée les modules par défaut.
		 */
		void CreateDefaultModules();

	#pragma region Event
		/**
		 * @brief Initialise le module.
		 */
		void Init() const;

		/**
		 * @brief Démarre le module.
		 */
		void Start() const;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() const;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() const;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() const;

		/**
		 * @brief Rendu du module.
		 */
		void Render() const;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() const;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() const;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() const;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() const;

#pragma endregion

		/**
		 * @brief Crée une instance du module de type T.
		 * @return Un pointeur vers le module créé.
		 */
		template <class T>
		T* CreateModule();

		/**
		 * @brief Obtient une instance du module de type T.
		 * @return Un pointeur vers le module obtenu, ou nullptr si le module n'existe pas.
		 */
		template <class T>
		T* GetModule();

	private:
		std::vector<Module*> modules; /**< Vecteur contenant des pointeurs vers les modules. */
};

template <class T>
T* ModuleManager::CreateModule()
{
	T* module            = new T();
	module->moduleManager = this;
	modules.push_back(module);
	return module;
}

template <class T>
T* ModuleManager::GetModule()
{
	for (Module*& module : modules)
	{
		if (T* module_cast = dynamic_cast<T*>(module)) return module_cast;
	}
	return nullptr;
}
