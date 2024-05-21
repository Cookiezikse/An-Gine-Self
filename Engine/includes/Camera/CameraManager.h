#pragma once

// std
#include <cstdint>
#include <stack>
#include <string>
#include <vector>

// Camera
#include "Camera/BaseCamera.h"
#include "Modules/Module.h"

namespace lve
{
	class BaseCamera;

	// TODO: Make inherit from System
	/**
	 * @brief Gère les caméras dans le moteur de jeu.
	 *
	 * Cette classe gère les différentes caméras utilisées dans le moteur de jeu.
	 * Elle permet de créer, de supprimer et de manipuler les caméras, ainsi que de gérer les vues et projections associées.
	 * La classe CameraManager est une classe finale, ce qui signifie qu'elle ne peut pas être dérivée.
	 */
	class CameraManager final : public Module
	{
		public:
			/**
			 * @brief Constructeur de la classe CameraManager.
			 *
			 * Ce constructeur initialise un nouvel objet CameraManager.
			 */
			CameraManager();

			/**
			 * @brief Destructeur de la classe CameraManager.
			 *
			 * Ce destructeur libère les ressources associées à l'objet CameraManager.
			 */
			~CameraManager();

			/**
			 * @brief Appelée après un changement de scène.
			 *
			 * Cette méthode est appelée après qu'une scène a été changée dans le moteur de jeu.
			 * Elle peut être utilisée pour effectuer des actions spécifiques liées au changement de scène.
			 *
			 * @note Cette méthode est constante et ne modifie pas l'état de l'objet CameraManager.
			 */
			void OnPostSceneChange() const;


			/**
			 * @brief Ajoute une nouvelle caméra au gestionnaire de caméras.
			 *
			 * Cette méthode ajoute une nouvelle caméra au gestionnaire de caméras.
			 *
			 * @param _camera Un pointeur vers la caméra à ajouter.
			 * @param _bSwitchTo Indique si la nouvelle caméra doit être définie comme caméra actuelle (par défaut : false).
			 */
			void AddCamera(BaseCamera* _camera, bool _bSwitchTo = false);

			// Clears stack and pushes the given camera onto it, then returns a pointer to it


			/**
			 * @brief Fait passer la caméra actuelle à la suivante ou à la précédente dans la liste des caméras gérées.
			 *
			 * Cette méthode fait passer la caméra actuelle à la suivante ou à la précédente dans la liste des caméras gérées.
			 *
			 * @param _deltaIndex Le delta pour l'index de la caméra (positif pour passer à la caméra suivante, négatif pour passer à la précédente).
			 * @param _bAlignWithPrevious Indique si la nouvelle caméra doit être alignée avec la position et l'orientation de la caméra précédente (par défaut : true).
			 * @return Un pointeur vers la caméra actuelle après le changement. Si aucune caméra n'est actuellement définie ou si l'index spécifié est en dehors de la plage des caméras gérées, retourne nullptr.
			 */
			BaseCamera* CycleCamera(uint32_t _deltaIndex, bool _bAlignWithPrevious = true);

			/**
			 * @brief Ajoute une nouvelle caméra à la pile et la définit comme caméra actuelle.
			 *
			 * Cette méthode ajoute une nouvelle caméra à la pile des caméras gérées et la définit comme caméra actuelle.
			 *
			 * @param _camera Un pointeur vers la caméra à ajouter à la pile et à définir comme caméra actuelle.
			 * @param _bAlignWithPrevious Indique si la nouvelle caméra doit être alignée avec la position et l'orientation de la caméra précédente (par défaut : true).
			 * @param _bInitialize Indique si la nouvelle caméra doit être initialisée après son ajout (par défaut : true).
			 * @return Un pointeur vers la caméra actuelle après l'ajout. Si _bInitialize est true et que l'initialisation échoue, retourne nullptr.
			 */
			BaseCamera* PushCamera(BaseCamera* _camera, bool _bAlignWithPrevious, bool _bInitialize);

			/**
			 * @brief Ajoute une nouvelle caméra par son nom à la pile et la définit comme caméra actuelle.
			 *
			 * Cette méthode ajoute une nouvelle caméra par son nom à la pile des caméras gérées et la définit comme caméra actuelle.
			 *
			 * @param _name Le nom de la caméra à ajouter à la pile et à définir comme caméra actuelle.
			 * @param _bAlignWithPrevious Indique si la nouvelle caméra doit être alignée avec la position et l'orientation de la caméra précédente (par défaut : true).
			 * @param _bInitialize Indique si la nouvelle caméra doit être initialisée après son ajout (par défaut : true).
			 * @return Un pointeur vers la caméra actuelle après l'ajout. Si _bInitialize est true et que l'initialisation échoue, retourne nullptr.
			 */
			BaseCamera* PushCameraByName(const std::string& _name, bool _bAlignWithPrevious, bool _bInitialize);

			/**
			 * @brief Supprime la caméra actuelle de la pile et restaure la caméra précédente.
			 *
			 * Cette méthode supprime la caméra actuelle de la pile des caméras gérées et restaure la caméra précédente comme caméra actuelle.
			 *
			 * @param _bAlignWithCurrent Indique si la nouvelle caméra actuelle doit être alignée avec la position et l'orientation de la caméra supprimée (par défaut : false).
			 */
			void PopCamera(bool _bAlignWithCurrent = false);

			#pragma region Getter

			/**
			 * @brief Obtient un pointeur vers la caméra actuelle.
			 *
			 * Cette méthode retourne un pointeur vers la caméra actuellement active dans le gestionnaire de caméras.
			 *
			 * @return Un pointeur vers la caméra actuelle. Si aucune caméra n'est actuellement définie, retourne nullptr.
			 */
			[[nodiscard]] BaseCamera* GetCurrentCamera() const;

			/**
			 * @brief Obtient un pointeur vers une caméra par son nom.
			 *
			 * Cette méthode recherche une caméra par son nom dans le gestionnaire de caméras et retourne un pointeur vers la première caméra trouvée avec le nom spécifié.
			 *
			 * @param _name Le nom de la caméra à rechercher.
			 * @return Un pointeur vers la caméra avec le nom spécifié. Si aucune caméra avec ce nom n'est trouvée, retourne nullptr.
			 */
			[[nodiscard]] BaseCamera* GetCameraByName(const std::string& _name) const;

			#pragma endregion

			#pragma region Setter

			/** @brief Définit une caméra spécifique comme caméra actuelle.
			 *
			 * Cette méthode définit une caméra spécifique comme caméra actuelle dans le gestionnaire de caméras.
			 *
			 * @param _camera Un pointeur vers la caméra à définir comme caméra actuelle.
			 * @param _bAlignWithPrevious Indique si la nouvelle caméra doit être alignée avec la position et l'orientation de la caméra précédente (par défaut : true).
			 * @return Un pointeur vers la caméra actuelle après le changement. Si la caméra spécifiée n'est pas dans la liste des caméras gérées, retourne nullptr.
			 */
			BaseCamera* SetCamera(BaseCamera* _camera, bool _bAlignWithPrevious);
			/**
			 * @brief Définit une caméra spécifique par son nom comme caméra actuelle.
			 *
			 * Cette méthode définit une caméra spécifique par son nom comme caméra actuelle dans le gestionnaire de caméras.
			 *
			 * @param _name Le nom de la caméra à définir comme caméra actuelle.
			 * @param _bAlignWithPrevious Indique si la nouvelle caméra doit être alignée avec la position et l'orientation de la caméra précédente (par défaut : true).
			 * @return Un pointeur vers la caméra actuelle après le changement. Si aucune caméra avec le nom spécifié n'est trouvée, retourne nullptr.
			 */
			BaseCamera* SetCameraByName(const std::string& _name, bool _bAlignWithPrevious);
			#pragma endregion

			/**
			 * @brief Affiche les objets ImGui associés au gestionnaire de caméras.
			 *
			 * Cette méthode affiche les objets ImGui associés au gestionnaire de caméras, permettant ainsi de visualiser et de modifier les paramètres des caméras.
			 *
			 * @note Cette méthode est généralement utilisée pour afficher une interface utilisateur graphique dans le cadre d'un éditeur de niveau ou d'un mode de débogage.
			 */
			void DrawImGuiObjects();

			/**
			 * @brief Aligne les paramètres de position, de rotation et de champ de vision d'une caméra source avec une caméra cible.
			 *
			 * Cette méthode copie les paramètres de position, de rotation et de champ de vision d'une caméra source vers une caméra cible.
			 *
			 * @param _from La caméra source à partir de laquelle copier les paramètres.
			 * @param _to La caméra cible vers laquelle copier les paramètres.
			 */
			void AlignCameras(const BaseCamera* _from, BaseCamera* _to);


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

		private:
			/*EventReply OnActionEvent(Action action, ActionEvent actionEvent);
			ActionCallback<CameraManager> m_ActionCallback;*/

			/**
			 * @brief Obtient l'index d'une caméra dans la liste des caméras gérées.
			 *
			 * Cette méthode recherche une caméra spécifiée dans la liste des caméras gérées par le gestionnaire de caméras
			 * et retourne son index dans la liste.
			 *
			 * @param _camera Un pointeur vers la caméra à rechercher.
			 * @return L'index de la caméra dans la liste des caméras gérées. Si la caméra spécifiée n'est pas trouvée, retourne UINT32_MAX.
			 */
			uint32_t GetCameraIndex(const BaseCamera* _camera) const;


			// TODO: Roll custom stack class
			// Stack containing temporary cameras, the topmost of which is the current camera
			// Always contains at least one element
			std::stack<BaseCamera*> cameraStack; ///< Pile contenant les caméras gérées par le gestionnaire de caméras.

			// All cameras, unordered
			std::vector<BaseCamera*> cameras;    ///< Vecteur contenant toutes les caméras gérées par le gestionnaire de caméras, sans ordre spécifique.

			bool bInitialized = false;            ///< Booléen indiquant si le gestionnaire de caméras a été initialisé ou non.

	};
};
