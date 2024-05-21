#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

namespace lve
{
	class LveCamera
	{
		public:
			/**
			* @brief Définit une projection orthographique.
			*
			* Cette fonction configure la matrice de projection pour une projection orthographique.
			* Une projection orthographique est une méthode de projection en infographie où les objets sont projetés sur un plan parallèle à l'écran, ce qui donne une représentation sans perspective.
			*
			* @param _left La position du plan de gauche de la vue orthographique.
			* @param _right La position du plan de droite de la vue orthographique.
			* @param _top La position du plan de dessus de la vue orthographique.
			* @param _bottom La position du plan de dessous de la vue orthographique.
			* @param _near La distance au plan de vue proche.
			* @param _far La distance au plan de vue éloignée.
			*/
			void SetOrthographicProjection(float _left, float _right, float _top, float _bottom, float _near,
			                               float _far);

			/**
			 * @brief Définit une projection en perspective.
			 *
			 * Cette fonction configure la matrice de projection pour une projection en perspective.
			 * Une projection en perspective est une méthode de projection en infographie qui crée une illusion de profondeur en donnant aux objets plus éloignés l'air d'être plus petits qu'ils ne le sont réellement.
			 *
			 * @param _fovY L'angle de champ vertical de la vue en degrés.
			 * @param _aspect Le rapport d'aspect de la vue, défini comme la largeur de la fenêtre divisée par sa hauteur.
			 * @param _near La distance au plan de vue proche.
			 * @param _far La distance au plan de vue éloignée.
			 *
			 * @note Cette fonction exige que `_aspect` soit différent de zéro.
			 * @note L'angle de champ vertical `_fovY` est spécifié en degrés.
			 */
			void SetPerspectiveProjection(float _fovY, float _aspect, float _near, float _far);

			/**
			 * @brief Définit la direction de vue de la caméra.
			 *
			 * Cette fonction configure la matrice de vue pour orienter la caméra dans une direction spécifiée.
			 * La matrice de vue est utilisée en infographie pour transformer les coordonnées des objets de l'espace monde à l'espace caméra, en tenant compte de la position de la caméra et de sa direction de vue.
			 *
			 * @param _position La position de la caméra dans l'espace.
			 * @param _direction La direction vers laquelle la caméra fait face.
			 * @param _up La direction considérée comme étant "vers le haut" pour la caméra.
			 *
			 * @note Les vecteurs _direction et _up doivent être normalisés pour garantir des résultats corrects.
			 */
			void SetViewDirection(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _up = glm::vec3{0.f, -1.f, 0.f});

			/**
			 * @brief Définit la cible de vue de la caméra.
			 *
			 * Cette fonction configure la matrice de vue pour orienter la caméra vers une cible spécifiée.
			 * La matrice de vue est utilisée en infographie pour transformer les coordonnées des objets de l'espace monde à l'espace caméra, en tenant compte de la position de la caméra et de sa direction de vue.
			 *
			 * @param _position La position de la caméra dans l'espace.
			 * @param _target La position de la cible vers laquelle la caméra doit être orientée.
			 * @param _up La direction considérée comme étant "vers le haut" pour la caméra.
			 *
			 * @note La différence entre _target et _position est utilisée pour calculer la direction de vue de la caméra.
			 */
			void SetViewTarget(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up = glm::vec3{0.f, -1.f, 0.f});

			/**
			 * @brief Définit la vue de la caméra en utilisant les rotations YXZ.
			 *
			 * Cette fonction configure la matrice de vue pour orienter la caméra en fonction des rotations autour des axes Y, X et Z.
			 * La matrice de vue est utilisée en infographie pour transformer les coordonnées des objets de l'espace monde à l'espace caméra, en tenant compte de la position de la caméra et de son orientation.
			 *
			 * @param _position La position de la caméra dans l'espace.
			 * @param _rotation Les rotations autour des axes Y, X et Z, respectivement, spécifiées en radians.
			 *
			 * @note Les rotations sont appliquées dans l'ordre suivant : d'abord autour de l'axe Y, puis autour de l'axe X et enfin autour de l'axe Z.
			 */
			void SetViewYXZ(glm::vec3 _position, glm::vec3 _rotation);

			/**
			 * @brief Renvoie la matrice de projection.
			 *
			 * Cette fonction renvoie la matrice de projection stockée en interne.
			 * La matrice de projection est généralement utilisée en infographie pour projeter des coordonnées 3D sur un écran 2D.
			 *
			 * @return const glm::mat4& La matrice de projection.
			 */
			[[nodiscard]] const glm::mat4& GetProjection() const { return projectionMatrix; }

			/**
			* @brief Renvoie la matrice de vue.
			*
			* Cette fonction renvoie la matrice de vue stockée en interne.
			* La matrice de vue est utilisée dans les systèmes de graphiques 3D pour transformer les coordonnées des objets de l'espace monde à l'espace caméra.
			*
			* @return const glm::mat4& La matrice de vue.
			*/
			[[nodiscard]] const glm::mat4& GetView() const { return viewMatrix; }


			/**
			 * @brief Obtient la matrice de vue inverse.
			 *
			 * @return La référence constante vers la matrice de vue inverse.
			 */
			[[nodiscard]] const glm::mat4& GetInverseView() const { return inverseViewMatrix; }

			/**
			 * @brief Obtient la position de la caméra.
			 *
			 * @return La position de la caméra en tant que vecteur glm::vec3.
			 */
			glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }

		private:
			glm::mat4 projectionMatrix{1.f};  /**< Matrice de projection. */
			glm::mat4 viewMatrix{1.f};        /**< Matrice de vue. */
			glm::mat4 inverseViewMatrix{1.f}; /**< Matrice de vue inverse. */
	};
}
