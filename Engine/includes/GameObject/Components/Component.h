#pragma once
#include <string>
#include "GameObject/GameObject.h"

#ifdef ENGINECORE_EXPORTS
#define ENGINEDLL_API __declspec(dllexport)
#else
#define ENGINEDLL_API __declspec(dllimport)
#endif


#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameObject;

/**
 * @brief Classe de base pour tous les composants attachés à un GameObject.
 *
 * Les composants sont des entités modulaires qui ajoutent des fonctionnalités spécifiques
 * à un GameObject, telles que la transformation, la physique, le rendu, etc.
 */
//class ENGINEDLL_API Component
class  Component
{
	public:
		Component() = default;
		explicit Component(GameObject* _owner);
		virtual  ~Component() = default;

#pragma region Event
		virtual void Init();
		virtual void Start();
		virtual void FixedUpdate();
		virtual void Update() ;
		virtual void UpdateEditor() ;
		virtual void PreRender();
		virtual void Render();
		virtual void RenderGui();
		virtual void PostRender();
		virtual void Release();
		virtual void Finalize();

#pragma endregion

#pragma region Getter

		/**
		 * @brief Obtient le GameObject propriétaire de ce composant.
		 *
		 * @return Un pointeur vers le GameObject propriétaire de ce composant.
		 */
		GameObject* GetOwner() const { return owner; }

		/**
		 * @brief Obtient le nom de ce composant.
		 *
		 * @return Le nom de ce composant.
		 */
		std::string GetName() const { return name; }

		/**
		 * @brief Obtient l'état d'activité de ce composant.
		 *
		 * @return true si le composant est actif, sinon false.
		 */
		bool GetActive() const { return isActive; }

		/**
		 * @brief Obtient l'état de visibilité de ce composant.
		 *
		 * @return true si le composant est visible, sinon false.
		 */
		bool GetVisible() const { return isVisible; }

#pragma endregion

#pragma region Setter

		/**
		 * @brief Définit le GameObject propriétaire de ce composant.
		 *
		 * @param _owner Le GameObject à définir comme propriétaire de ce composant.
		 */
		void        SetOwner(GameObject* _owner) { owner = _owner; }

		/**
		 * @brief Définit le nom de ce composant.
		 *
		 * @param _name Le nom à attribuer à ce composant.
		 */
		void        SetName(const std::string& _name) { name = _name; }

		/**
		 * @brief Définit le nom de ce composant à partir d'une chaîne de caractères.
		 *
		 * @param _name La chaîne de caractères à utiliser comme nom pour ce composant.
		 */
		void        SetName(const char* _name) { name = _name; }

		/**
		 * @brief Définit l'état d'activité de ce composant.
		 *
		 * @param _state L'état d'activité à définir pour ce composant.
		 */
		void SetActive(const bool& _state) { isActive = _state; }

		/**
		 * @brief Définit l'état de visibilité de ce composant.
		 *
		 * @param _state L'état de visibilité à définir pour ce composant.
		 */
		void SetVisible(const bool& _state) { isVisible = _state; }

		/**
		 * @brief Active ou désactive ce composant et le rend visible ou invisible.
		 *
		 * Cette fonction active ou désactive ce composant et le rend visible ou invisible en fonction de l'état spécifié.
		 *
		 * @param _state L'état d'activation et de visibilité à définir.
		 */
		void SetActiveAndVisible(const bool& _state)
		{
			isVisible = _state;
			isActive  = _state;
		}

		// Méthode virtuelle pour convertir en JSON
		virtual json toJson() const {
			json j;
			j["type"] = type;
			j["name"] = name;
			j["isActive"] = isActive;
			j["isVisible"] = isVisible;
			return j;
		}

		// Méthode virtuelle pour initialiser à partir de JSON
		virtual void fromJson(const json& j) {
			type = j["type"];
			name = j["name"];
			isActive = j["isActive"];
			isVisible = j["isVisible"];
		}

#pragma endregion

	protected:

		GameObject* owner = nullptr; /**< Pointeur vers le GameObject propriétaire de ce composant. */
		std::string name = "Component"; /**< Nom de ce composant. */
		std::string type = "Component"; /**< Nom de ce composant. */
		bool isActive = true; /**< Indique si ce composant est actif. */
		bool isVisible = true; /**< Indique si ce composant est visible. */
};
