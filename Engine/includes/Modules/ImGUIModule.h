#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "rhi.h"

#include <functional>


#include "Module.h"
#include "Modules/Module.h"
#include "ModuleManager.h"
#include "Modules/WindowModule.h"
#include "Modules/SoundSystemModule.h"
#include "Modules/WindowModule.h"

#include "ImGUIInterface.h"
#include "ImGUIAudio.h"

#include "GameObject/PreGameObject/LightGameObject.h"
#include "Scene/SceneManager.h"
#include "RHIVulkanModule.h"
#include "TCP/Errors.h"
#include <cstdarg>

#include "TCP/Client/TCPClientStart.h"
#include "TCP/Server/TCPServerStart.h"
#include "TCP/StatusMessage.h"
#include "TCP/Server/Server.h"

#include "Console.h"

class RHIModule;

/**
 * @brief Classe d�di�e � la gestion de l'interface utilisateur avec ImGui dans le contexte du module de rendu.
 */
class ImGuiModule final : public Module
{
public:

	//TimeModule* timeModule = nullptr;
	vk::Fence         immFence;
	vk::CommandBuffer immCommandBuffer;
	vk::CommandPool   immCommandPool;


	// ----------========== IMGUI SETTINGS ==========---------- //

	/**
	 * @brief Initialise le module ImGui, pr�parant les ressources n�cessaires pour son fonctionnement.
	 */
	void Init() override;

	/**
	 * @brief D�marre le module ImGui et pr�pare l'interface utilisateur pour l'interaction.
	 */
	void Start() override;

	/**
	 * @brief Ex�cute des mises � jour r�guli�res � intervalles fixes, typiquement pour la logique de jeu.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met � jour l'�tat du module ImGui avant le rendu.
	 */
	void Update() override;

	/**
	 * @brief Pr�pare le module ImGui pour le rendu, mais avant que le rendu ne soit effectu�.
	 */
	void PreRender() override;

	/**
	 * @brief Ex�cute le rendu de l'interface utilisateur ImGui.
	 */
	void Render() override;

	/**
	 * @brief Dessine l'interface graphique sp�cifique au module, comme les menus et les fen�tres.
	 */
	void RenderGui() override;

	/**
	 * @brief Op�rations � effectuer apr�s le rendu principal, souvent utilis�es pour les nettoyages.
	 */
	void PostRender() override;

	/**
	 * @brief Lib�re les ressources utilis�es par le module ImGui.
	 */
	void Release() override;

	/**
	 * @brief Nettoie l'�tat du module ImGui et ferme l'interface utilisateur.
	 */
	void Finalize() override;

	/**
	 * @brief Soumet une commande graphique imm�diate � ex�cuter.
	 * @param _function La fonction qui contient les commandes graphiques � ex�cuter.
	 */
	void ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function) const;


	// ----------========== IMGUI SHOWN ==========---------- //

	/**
	 * @brief Construit les �l�ments de l'interface utilisateur � afficher, incluant les fen�tres "Hierarchy" et "Inspector".
	 */
	void GetGui();

	/**
	 * @brief Fixe la fen�tre de l'interface utilisateur � une position sp�cifique.
	 * @param _windowName Nom de la fen�tre � ancrer.
	 */
	void AnchorWindow(const std::string& _windowName);


	// ----------========== DRAW WINDOWS ==========---------- //

	/**
	 * @brief Dessine la hi�rarchie des objets de la sc�ne dans l'interface utilisateur.
	 */
	void DrawHierarchyWindow();

	/**
	 * @brief Dessine l'inspecteur des propri�t�s pour l'objet s�lectionn�.
	 */
	void DrawInspectorWindow();

	/**
	 * @brief Dessine la fen�tre de chat.
	 */
	void DrawTchatWindow();

	/**
	 * @brief Affiche et permet l'�dition des propri�t�s de transformation d'un objet.
	 * @param _transform Pointeur vers l'objet Transform � afficher et �diter.
	 */
	void DisplayTransform(Transform* _transform);

	void DrawModesWindow();

	/**
	 * @brief Dessine les param�tres de l'interface utilisateur du moteur de jeu.
	 */
	void DrawSettingsWindow();

	void DrawConsoleWindow();

	void DrawFilesExplorerWindow();

	void DrawProjectSaveWindow();



	// ----------========== POPUPS ==========---------- //

	/**
	 * @brief Affiche la popup de renommage pour l'entit� s�lectionn�e.
	 */
	void ShowRenamePopup();


	// ----------========== OBJECT OPERATIONS ==========---------- //

	/**
	 * @brief Renomme l'objet de jeu s�lectionn�.
	 * @param _gameObject Pointeur vers l'objet de jeu � renommer.
	 * @param _newName Nouveau nom � attribuer � l'objet de jeu.
	 */
	void RenameGameObject(GameObject* _gameObject, const std::string& _newName);

	/**
	 * @brief Cr�e un nouveau GameObject du type sp�cifi� et l'ajoute � la sc�ne active.
	 * @param _type Type de GameObject � cr�er, comme Cube, Light ou Plane.
	 * @param _otherType
	 */
	void CreateSpecificGameObject(GameObjectType _type, int _otherType = 0);

	void HandleShortcuts();

	void DuplicateGameObject(GameObject* _gameObject);

	void DeleteGameObject(GameObject* _gameObject);

	/**
	* @brief Ajoute un nouveau texte aux logs de la console.
	* @param _newText Le nouveau texte à ajouter.
	*/
	void AddLog(std::string _newText) { 
		Console& console = Console::getInstance();
		console.AddLog(_newText);
	}

	/**
	* @brief Efface tous les logs de la console.
	*/
	void ClearLog() {
		Console& console = Console::getInstance();
		console.ClearLogs();
	}

	/**
	 * @brief Obtient l'état du filtre de mise en garde.
	 * @return L'état actuel du filtre de mise en garde.
	 */
	bool GetFilterWarning() { return filterWarning; }

	/**
	 * @brief Obtient l'état du filtre d'erreur.
	 * @return L'état actuel du filtre d'erreur.
	 */
	bool GetFilterError() { return filterError; }

	/**
	 * @brief Obtient l'état du filtre simple.
	 * @return L'état actuel du filtre simple.
	 */
	bool GetFilterSimple() { return filterSimple; }

	/**
	 * @brief Définit l'état du filtre de mise en garde.
	 * @param _newState Le nouvel état du filtre de mise en garde.
	 */
	void SetFilterWarning(bool _newState) { filterWarning = _newState; }

	/**
	 * @brief Définit l'état du filtre d'erreur.
	 * @param _newState Le nouvel état du filtre d'erreur.
	 */
	void SetFilterError(bool _newState) { filterError = _newState; }

	/**
	 * @brief Définit l'état du filtre simple.
	 * @param _newState Le nouvel état du filtre simple.
	 */
	void SetFilterSimple(bool _newState) { filterSimple = _newState; }

	/**
	 * @brief Obtient le fichier à rechercher.
	 * @return Le fichier à rechercher.
	 */
	std::string GetFileToLook() { return fileToLook; }

	/**
	 * @brief Définit le fichier à rechercher.
	 * @param _newFileToLook Le nouveau fichier à rechercher.
	 */
	void SetFileToLook(std::string _newFileToLook) { fileToLook = _newFileToLook; }

	/**
	 * @brief Obtient le répertoire actuel.
	 * @return Le répertoire actuel.
	 */
	std::string GetCurrentDir() { return currentDir; }

	/**
	 * @brief Définit le répertoire actuel.
	 * @param _newDir Le nouveau répertoire actuel.
	 */
	void SetCurrentDir(std::string _newDir) { currentDir = _newDir; }

	/**
	 * @brief Obtient la recherche de fichier.
	 * @return La recherche de fichier.
	 */
	std::string GetFileSearch() { return fileSearch; }

	/**
	 * @brief Définit la recherche de fichier.
	 * @param _newFileSearch La nouvelle recherche de fichier.
	 */
	void SetFileSearch(std::string _newFileSearch) { fileSearch = _newFileSearch; }

	/**
	 * @brief Obtient l'état du filtre pour les fichiers .obj.
	 * @return L'état actuel du filtre pour les fichiers .obj.
	 */
	bool GetFilterObj() { return filterObj; }

	/**
	 * @brief Obtient l'état du filtre pour les images supportées.
	 * @return L'état actuel du filtre pour les images supportées.
	 */
	bool GetFilterSupportedImages() { return filterSupportedImages; }

	/**
	 * @brief Obtient l'état du filtre pour les autres types de fichiers.
	 * @return L'état actuel du filtre pour les autres types de fichiers.
	 */
	bool GetFilterOther() { return filterOther; }

	/**
	 * @brief Obtient l'état du filtre pour les répertoires.
	 * @return L'état actuel du filtre pour les répertoires.
	 */
	bool GetFilterDirs() { return filterDirs; }


	/**
	 * @brief Définit l'état du filtre pour les fichiers .obj.
	 * @param _newState Le nouvel état du filtre pour les fichiers .obj.
	 */
	void SetFilterObj(bool _newState) { filterObj = _newState; }

	/**
	 * @brief Définit l'état du filtre pour les images supportées.
	 * @param _newState Le nouvel état du filtre pour les images supportées.
	 */
	void SetFilterSupportedImages(bool _newState) { filterSupportedImages = _newState; }

	/**
	 * @brief Définit l'état du filtre pour les autres types de fichiers.
	 * @param _newState Le nouvel état du filtre pour les autres types de fichiers.
	 */
	void SetFilterOther(bool _newState) { filterOther = _newState; }

	/**
	 * @brief Définit l'état du filtre pour les répertoires.
	 * @param _newState Le nouvel état du filtre pour les répertoires.
	 */
	void SetFilterDirs(bool _newState) { filterDirs = _newState; }

	/**
	 * @brief Obtient l'état d'affichage de la popup d'erreur.
	 * @return L'état actuel d'affichage de la popup d'erreur.
	 */
	bool GetShowPopupError() { return showPopupError; }

	/**
	 * @brief Définit l'état d'affichage de la popup d'erreur.
	 * @param _newState Le nouvel état d'affichage de la popup d'erreur.
	 */
	void SetShowPopupError(bool _newState) { showPopupError = _newState; }

	/**
	 * @brief Obtient le nom de la scène à nommer.
	 * @return Le nom de la scène à nommer.
	 */
	std::string GetSceneToName() { return sceneToName; }

	/**
	 * @brief Définit le nom de la scène à nommer.
	 * @param _newString Le nouveau nom de la scène à nommer.
	 */
	void SetSceneToName(std::string _newString) { sceneToName = _newString; }

	std::string GetSearch() { return search; }
	void SetSearch(std::string _newSearch) { search = _newSearch; }

protected:
	vk::Device device; ///< P�riph�rique utilis� pour le rendu avec Vulkan.
	vk::Queue graphicsQueue; ///< File d'attente graphique pour la soumission des commandes Vulkan.

	TCPClientStart clientTCP;///< Instance du client TCP.
	TCPServerStart serverTCP;///< Instance du serveur TCP.
	bool isConnectedTCPClient = false;///< Indicateur indiquant si le client TCP est connecté.
	bool isConnectedTCPServer = false;///< Indicateur indiquant si le serveur TCP est connecté.

	WindowModule* windowModule = nullptr; ///< Module de gestion de la fenêtre de l'application.
	RHIModule* rhiModule = nullptr; ///< Module de gestion du rendu bas-niveau (RHI).
	SceneManager* sceneManager = nullptr; ///< Gestionnaire de sc�nes de l'application.

	GameObject* selectedGameObject = nullptr; ///< Pointeur vers l'objet de jeu actuellement s�lectionn�.

	bool isRenamePopupOpen = false; ///< Indique si la popup de renommage est ouverte ou non.
	int entityToRename = -1; ///< Index de l'entit� � renommer.
	char renameBuffer[256]; ///< Buffer pour le nouveau nom de l'entit� � renommer.
	int sceneToRename = -1; ///< Index de la sc�ne � renommer.
	char renameSceneBuffer[100]; ///< Buffer pour le nouveau nom de la sc�ne � renommer.

	bool openPositionEdit = false; ///< Indique si l'�dition de la position est ouverte ou non.
	bool openScaleEdit = false; ///< Indique si l'�dition de l'�chelle est ouverte ou non.
	glm::vec3 positionEdit; ///< Stocke la position actuelle pour l'�dition.
	float rotationEdit; ///< Stocke la rotation actuelle pour l'�dition.
	glm::vec3 scaleEdit; ///< Stocke l'�chelle actuelle pour l'�dition.

	std::vector<bool> isOpen; ///< Vecteur pour suivre l'�tat ouvert/ferm� des diff�rents panneaux de l'interface utilisateur.

	bool textureView = false; ///< Indicateur de visualisation de texture.
	bool changeScaleLinked = false; ///< Indicateur de changement d'échelle lié.
	bool filterSimple = true; ///< Indicateur de filtre simple activé.
	bool filterError = true; ///< Indicateur de filtre d'erreurs activé.
	bool filterWarning = true; ///< Indicateur de filtre de mises en garde activé.

	bool filterSupportedImages = true; ///< Indicateur de filtre des images supportées activé.
	bool filterObj = true; ///< Indicateur de filtre pour les fichiers .obj activé.
	bool filterOther = true; ///< Indicateur de filtre pour les autres types de fichiers activé.
	bool filterDirs = true; ///< Indicateur de filtre pour les répertoires activé.

	bool refreshFileExplorer = true; ///< Indicateur de rafraîchissement de l'explorateur de fichiers activé.
	bool addTexButton = false; ///< Indicateur de bouton d'ajout de texture activé.

	bool showPopupError = false; ///< Indicateur d'affichage de la popup d'erreur.
	bool showPopupProject = true; ///< Indicateur d'affichage de la popup de projet.

	std::vector<std::wstring> fileNames; ///< Liste des noms de fichiers.

	std::vector<VkDescriptorSet>* ListDescriptorsImGui = new std::vector<VkDescriptorSet>(); ///< Liste des descripteurs ImGui.

	std::string search;

	std::string fileSearch; ///< Recherche de fichier.

	std::string fileToLook; ///< Fichier à rechercher.
	std::string currentDir = "./Textures"; ///< Répertoire actuel.
	std::string sceneToName; ///< Scène à nommer.

	char ipBuffer[64] = "";  // Buffer pour l'adresse IP
	char portBuffer[6] = ""; // Buffer pour le port, suffisant pour contenir des nombres jusqu'� 65535
	char messageBuffer[256] = ""; // Buffer pour le message � envoyer
	std::vector<std::string> messageLogs; // Liste des messages re�us

	ImGUIAudio* imGuiAudio; ///< Interface utilisateur pour les contr�les audio.
	SoundSystemModule* soundModule; ///< Module de gestion du syst�me audio.

	/**
	 * @brief Destructeur par d�faut.
	 */
	~ImGuiModule() = default;
};
