#pragma once

// lve
#include "LveEngine/lve_constants.h"

// Librairies
#include <glm.hpp>

// std
#include <string>

namespace lve
{
	/**
 * @brief Enumération décrivant les différents types de caméras.
 *
 * Cette énumération définit les différents types de caméras pouvant être utilisés dans le moteur de jeu.
 */
	enum class CameraType
	{
		DEBUG_CAM,
		///< Caméra de débogage.
		OVERHEAD,
		///< Vue en hauteur.
		FIRST_PERSON,
		///< Vue à la première personne.
		TERMINAL,
		///< Caméra de terminal.
		VEHICLE,
		///< Caméra de véhicule.
		NONE ///< Aucun type spécifié.
	};

	/**
 * @brief Classe de base pour une caméra dans un moteur de jeu.
 *
 * Cette classe définit les fonctionnalités de base d'une caméra dans un moteur de jeu.
 */
	class BaseCamera
	{
		public:
			/**
 * @brief Constructeur de la classe BaseCamera.
 *
 * @param _cameraName Nom de la caméra.
 * @param _type Type de la caméra.
 * @param _bIsGameplayCam Booléen indiquant si c'est une caméra de gameplay.
 * @param _fov Champ de vision de la caméra en radians (par défaut : 45 degrés).
 * @param _zNear Distance du plan de coupe avant (par défaut : 0.5 unités).
 * @param _zFar Distance du plan de coupe arrière (par défaut : 1000.0 unités).
 */
			BaseCamera(std::string _cameraName, CameraType _type, bool _bIsGameplayCam,
			           float       _fov   = glm::radians(45.0f),
			           float       _zNear = 0.5f, float _zFar = 1000.0f);

			/**
			 * @brief Destructeur virtuel de la classe BaseCamera.
			 */
			virtual ~BaseCamera();

#pragma region Event
			/**
				* @brief Initialise le module.
				*/
			virtual void Init();

			/**
			 * @brief Démarre le module.
			 */
			virtual void Start();

			/**
			 * @brief Effectue une mise à jour fixe du module.
			 */
			virtual void FixedUpdate(const float& _deltaTime);

			/**
			 * @brief Met à jour le module.
			 */
			virtual void Update(const float& _deltaTime);

			/**
			 * @brief Fonction pré-rendu du module.
			 */
			virtual void PreRender();

			/**
			 * @brief Rendu du module.
			 */
			virtual void Render();

			/**
			 * @brief Rendu de l'interface graphique du module.
			 */
			virtual void RenderGui();

			/**
			 * @brief Fonction post-rendu du module.
			 */
			virtual void PostRender();

			/**
			 * @brief Libère les ressources utilisées par le module.
			 */
			virtual void Release();

			/**
			 * @brief Finalise le module.
			 */
			virtual void Finalize();
#pragma endregion

			/**
 * @brief Appelée après un changement de scène.
 *
 * Cette fonction est appelée après qu'une scène a été changée dans le moteur de jeu.
 * Elle peut être utilisée pour effectuer des actions spécifiques liées au changement de scène.
 * Par défaut, cette fonction ne fait rien et doit être redéfinie dans les classes dérivées selon les besoins.
 */
			virtual void OnPostSceneChange();

			/**
			 * @brief Appelée lorsqu'un objet est possédé par cette caméra.
			 *
			 * Cette fonction est appelée lorsqu'un objet est possédé par cette caméra dans le moteur de jeu.
			 * Elle peut être utilisée pour effectuer des actions spécifiques lors de la possession d'un objet par la caméra.
			 * Par défaut, cette fonction ne fait rien et doit être redéfinie dans les classes dérivées selon les besoins.
			 */
			virtual void OnPossess();

			/**
			 * @brief Appelée lorsqu'un objet est dépossédé par cette caméra.
			 *
			 * Cette fonction est appelée lorsqu'un objet est dépossédé par cette caméra dans le moteur de jeu.
			 * Elle peut être utilisée pour effectuer des actions spécifiques lors de la dépossession d'un objet par la caméra.
			 * Par défaut, cette fonction ne fait rien et doit être redéfinie dans les classes dérivées selon les besoins.
			 */
			virtual void OnDepossess();

			/**
 * @brief Obtient la matrice de vue-projection.
 *
 * Cette fonction retourne la matrice de vue-projection associée à cette caméra.
 *
 * @return La matrice de vue-projection.
 */
			[[nodiscard]] glm::mat4 GetViewProjection() const { return viewProjection; }

			/**
			 * @brief Obtient la matrice de vue.
			 *
			 * Cette fonction retourne la matrice de vue associée à cette caméra.
			 *
			 * @return La matrice de vue.
			 */
			[[nodiscard]] glm::mat4 GetView() const { return view; }

			/**
			 * @brief Obtient la matrice de projection.
			 *
			 * Cette fonction retourne la matrice de projection associée à cette caméra.
			 *
			 * @return La matrice de projection.
			 */
			[[nodiscard]] glm::mat4 GetProjection() const { return projection; }


			/**
 * @brief Oriente la caméra vers un point donné.
 *
 * Cette fonction oriente la caméra vers un point donné dans l'espace.
 *
 * @param _point Le point vers lequel orienter la caméra.
 * @param _speed La vitesse à laquelle effectuer l'orientation (par défaut : 1.0).
 */
			void LookAt(glm::vec3 _point, float _speed = 1.0f); // speed: Lerp amount to new rotation

			/**
			 * @brief Translate la caméra selon un vecteur de translation donné.
			 *
			 * Cette fonction translate la position de la caméra selon un vecteur de translation donné.
			 *
			 * @param _translation Le vecteur de translation à appliquer à la position de la caméra.
			 */
			void Translate(glm::vec3 _translation);

			/**
			 * @brief Définit la direction de vue de la caméra en utilisant des angles de tangage et de lacet.
			 *
			 * Cette fonction définit la direction de vue de la caméra en utilisant des angles de tangage (yaw) et de lacet (pitch)
			 * spécifiés en radians.
			 *
			 * @param _yawRad   L'angle de lacet en radians.
			 * @param _pitchRad L'angle de tangage en radians.
			 */
			void SetViewDirection(float _yawRad, float _pitchRad);


			/**
 * @brief Réinitialise la position de la caméra.
 *
 * Cette fonction réinitialise la position de la caméra à sa position par défaut.
 */
			void ResetPosition();

			/**
			 * @brief Réinitialise l'orientation de la caméra.
			 *
			 * Cette fonction réinitialise l'orientation de la caméra à son orientation par défaut.
			 */
			void ResetOrientation();

			/**
			 * @brief Obtient le nom de la caméra.
			 *
			 * Cette fonction retourne le nom de la caméra.
			 *
			 * @return Le nom de la caméra.
			 */
			[[nodiscard]] std::string GetName() const { return name; }

			/**
			 * @brief Calcule l'exposition de la caméra.
			 *
			 * Cette fonction calcule l'exposition de la caméra en fonction de son environnement ou de ses paramètres internes.
			 * Elle peut être utilisée pour ajuster automatiquement les paramètres de la caméra en fonction de la luminosité ambiante ou d'autres facteurs.
			 */
			void CalculateExposure();


			// Contrôle de l'exposition
			float aperture         = 1.0f;        /**< Ouverture du diaphragme en f-stops. */
			float shutterSpeed     = 1.0f / 8.0f; /**< Vitesse d'obturation en secondes. */
			float lightSensitivity = 800.0f;      /**< Sensibilité ISO. */
			float exposure         = 0.0f;        /**< Exposition. */

			float fov   = 0.0f; /**< Champ de vision (Field of View). */
			float zNear = 0.0f; /**< Plan rapproché du volume de vue. */
			float zFar  = 0.0f; /**< Plan éloigné du volume de vue. */

			float moveSpeed = 0.0f;
			/**< Vitesse de déplacement (WASD ou stick analogique gauche de la manette de jeu). */
			float panSpeed = 0.0f; /**< Vitesse de panoramique (bouton du milieu de la souris). */
			float dragDollySpeed = 0.0f; /**< Vitesse de déplacement par glisser (bouton droit de la souris). */
			float scrollDollySpeed = 0.0f; /**< Vitesse de déplacement par défilement (molette de la souris). */
			float orbitingSpeed = 0.0f; /**< Vitesse de rotation orbitale (Alt + glisser bouton gauche de la souris). */
			float mouseRotationSpeed = 0.0f;
			/**< Vitesse de rotation de la souris (glisser bouton gauche de la souris). */
			float gamepadRotationSpeed    = 0.0f; /**< Vitesse de rotation du stick droit de la manette de jeu. */
			float moveSpeedFastMultiplier = 0.0f; /**< Multiplicateur de vitesse de déplacement rapide. */
			float moveSpeedSlowMultiplier = 0.0f; /**< Multiplicateur de vitesse de déplacement lente. */
			float turnSpeedFastMultiplier = 0.0f; /**< Multiplicateur de vitesse de rotation rapide. */
			float turnSpeedSlowMultiplier = 0.0f; /**< Multiplicateur de vitesse de rotation lente. */
			float panSpeedFastMultiplier  = 0.0f; /**< Multiplicateur de vitesse de panoramique rapide. */
			float panSpeedSlowMultiplier  = 0.0f; /**< Multiplicateur de vitesse de panoramique lente. */
			float rollRestorationSpeed    = 0.0f; /**< Vitesse de restauration du roulis. */

			glm::vec3 position;     /**< Position de la caméra. */
			glm::vec3 prevPosition; /**< Position précédente de la caméra. */
			glm::vec3 velocity;     /**< Vélocité de la caméra. */

			float     yaw;     /**< Angle de lacet (yaw). */
			float     pitch;   /**< Angle de tangage (pitch). */
			float     roll;    /**< Angle de roulis (roll). */
			glm::vec3 forward; /**< Vecteur de direction avant de la caméra. */
			glm::vec3 up;      /**< Vecteur de direction vers le haut de la caméra. */
			glm::vec3 right;   /**< Vecteur de direction droite de la caméra. */

			bool bIsGameplayCam = true;  /**< Indique si la caméra est en mode gameplay. */
			bool bIsFirstPerson = false; /**< Indique si la caméra est en mode première personne. */
			bool bDebugCyclable = true;  /**< Indique si le mode de débogage est cyclable. */
			bool bPossessPlayer = false; /**< Indique si le joueur est possédé par la caméra. */


			CameraType type;

		protected:
			/**
 * @brief Calcule les vecteurs d'axe à partir des angles de tangage et de lacet.
 *
 * Cette fonction calcule les vecteurs d'axe (avant, haut et droite) à partir des angles de tangage (pitch) et de lacet (yaw)
 * actuellement définis pour la caméra.
 */
			void CalculateAxisVectorsFromPitchAndYaw(); // Sets right, up, and forward based on yaw and pitch

			/**
			 * @brief Calcule les angles de tangage et de lacet à partir de la direction avant de la caméra.
			 *
			 * Cette fonction calcule les angles de tangage (pitch) et de lacet (yaw) à partir de la direction avant actuelle de la caméra.
			 */
			void CalculateYawAndPitchFromForward();

			/**
			 * @brief Recalcule la matrice de vue-projection.
			 *
			 * Cette fonction recalcule la matrice de vue-projection de la caméra en fonction de ses paramètres internes, tels que la position et l'orientation.
			 * Elle est généralement appelée après avoir modifié la position ou l'orientation de la caméra pour mettre à jour la vue et la projection.
			 */
			void RecalculateViewProjection();


			/**
 * @brief Applique un jitter à une matrice donnée.
 *
 * Cette fonction applique un jitter à une matrice de transformation donnée.
 * Le jitter peut être utilisé pour ajouter un léger décalage à la matrice, souvent utilisé pour créer un effet de tremblement ou de mouvement aléatoire.
 *
 * @param _matrix La matrice à laquelle appliquer le jitter. La matrice est modifiée directement.
 */
			void JitterMatrix(glm::mat4& _matrix);

			/**
			 * @brief Limite l'angle de tangage de la caméra.
			 *
			 * Cette fonction limite l'angle de tangage (pitch) de la caméra dans une plage acceptable.
			 * Elle est utilisée pour s'assurer que la caméra ne dépasse pas des limites définies par l'utilisateur ou par le moteur de jeu.
			 */
			void ClampPitch();


			// Exposure calculations taken from Google's Filament rendering engine
			// Computes the camera's EV100
			// aperture measured in f-stops
			// shutterSpeed measured in seconds
			// sensitivity measured in ISO

			/**
 * @brief Calcule la valeur EV100 à partir des paramètres de l'appareil photo.
 *
 * Cette fonction calcule la valeur EV100 (exposition relative à 100 ISO et une seconde de temps d'exposition) à partir des paramètres de l'appareil photo :
 * l'ouverture de l'objectif, la vitesse d'obturation et la sensibilité ISO.
 *
 * @param _aperture L'ouverture de l'objectif de l'appareil photo.
 * @param _shutterSpeed La vitesse d'obturation de l'appareil photo.
 * @param _sensitivity La sensibilité ISO de l'appareil photo.
 * @return La valeur EV100 calculée.
 */
			static float CalculateEv100(float _aperture, float _shutterSpeed, float _sensitivity);

			/**
			 * @brief Calcule le facteur de normalisation d'exposition à partir de la valeur EV100 de la caméra.
			 *
			 * Cette fonction calcule le facteur de normalisation de l'exposition à partir de la valeur EV100 de la caméra.
			 * Le facteur de normalisation est utilisé pour ajuster automatiquement les paramètres d'exposition de la caméra en fonction de la luminosité ambiante ou d'autres facteurs.
			 *
			 * @param _ev100 La valeur EV100 de la caméra.
			 * @return Le facteur de normalisation de l'exposition calculé.
			 */
			static float ComputeExposureNormFactor(float _ev100);
			// Computes the exposure normalization factor from the camera's EV100


			bool mBInitialized = false; ///< Booléen indiquant si la caméra a été initialisée ou non.

			std::string name; ///< Nom de la caméra.

			glm::mat4 view;           ///< Matrice de vue de la caméra.
			glm::mat4 projection;     ///< Matrice de projection de la caméra.
			glm::mat4 viewProjection; ///< Matrice de vue-projection combinant la vue et la projection.
	};
} // namespace lve
