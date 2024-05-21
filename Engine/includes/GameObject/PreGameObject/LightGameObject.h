#pragma once
#include "GameObject/GameObject.h"
#include "LveEngine/lve_constants.h"

namespace lve
{
	/**
	 * @brief Représente un objet lumière dans le jeu.
	 *
	 * Cette classe représente un objet lumière dans le jeu. Un objet lumière est un élément
	 * utilisé pour simuler l'éclairage dans une scène tridimensionnelle.
	 */
	class LightGameObject
	{
		public:
			/**
			 * @brief Crée un nouvel objet lumière.
			 *
			 * Cette méthode statique crée un nouvel objet lumière avec les paramètres spécifiés.
			 *
			 * @param _intensity L'intensité de la lumière (par défaut : 10.0).
			 * @param _radius Le rayon d'illumination de la lumière (par défaut : 1.0).
			 * @param _position La position initiale de l'objet lumière dans l'espace tridimensionnel (par défaut : glm::vec3(0, 0, 0)).
			 * @param _rotate La rotation initiale de l'objet lumière (par défaut : glm::vec3(0, 0, 0)).
			 * @param _color La couleur de la lumière (par défaut : glm::vec3(1, 1, 1)).
			 * @return Un pointeur vers le nouvel objet lumière créé.
			 */
			static GameObject* Create(float     _intensity = 10.f, float _radius = 1.f, glm::vec3 _position = VEC3_ZERO,
			                          glm::vec3 _rotate    = VEC3_ZERO, glm::vec3 _color = VEC3_ONE);
	};
}
