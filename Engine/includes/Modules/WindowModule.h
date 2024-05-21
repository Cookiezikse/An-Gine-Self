#pragma once
#include "Modules/Module.h"

#include "LveEngine/lve_descriptors.h"
#include "LveEngine/lve_window.h"
#include "Modules/InputModule.h"
//std
#include <iostream>
#include <stb_image.h>
#include <string>

#include "GameObject/GameObject.h"

#define WINDOW_CONFIG_LOCATION "Saves\\window_config.json"

namespace Inputs
{
	/**
	 * @brief Actions associées à une touche ou un bouton.
	 */
	enum class KeyAction;

	/**
	 * @brief Boutons de la souris.
	 */
	enum class MouseButton;

	/**
	 * @brief Codes des touches du clavier.
	 */
	enum class KeyCode;
}

class InputModule;

class SceneManager;

/**
 * @brief Types de curseurs GLFW.
 */
enum GlfwCursorType
{
	ARROW,
	/**< Curseur fl�che. */
	IBEAM,
	/**< Curseur I-beam (curseur de texte). */
	CROSSHAIR,
	/**< Curseur croix. */
	POINTING_HAND,
	/**< Curseur main pointant. */
	RESIZE_EW,
	/**< Curseur redimensionner horizontal. */
	RESIZE_NS,
	/**< Curseur redimensionner vertical. */
	RESIZE_NWSE,
	/**< Curseur redimensionner diagonale NW-SE. */
	RESIZE_NESW,
	/**< Curseur redimensionner diagonale NE-SW. */
	RESIZE_ALL,
	/**< Curseur redimensionner dans toutes les directions. */
	NOT_ALLOWED /**< Curseur non autoris�. */
};

/**
 * @brief Enum�ration des modes de curseurs GLFW.
 *
 * Cette �num�ration repr�sente les diff�rents modes de curseurs GLFW.
 * Ces modes d�finissent le comportement du curseur sur l'�cran.
 */
enum GlfwCursorMode
{
	DISABLED,
	/**< Curseur d�sactiv�. */
	HIDDEN,
	/**< Curseur cach�. */
	NORMAL,
	/**< Curseur normal (visible). */
	CAPTURED,
	/**< Curseur captur� (exclusif � la fen�tre). */

	_NONE
};

enum class WindowMode
{
	WINDOWED,
	WINDOWED_FULLSCREEN,
	// (aka "Borderless windowed")
	FULLSCREEN,

	_NONE
};

/**
 * @brief Tableau de chaînes de caractères représentant les modes de fenêtre.
 *
 * Ce tableau contient les noms associés à chaque mode de fenêtre.
 */
static const char* WindowModeStrings[] =
{
	"Windowed",
	"Windowed Fullscreen",
	"Fullscreen",

	"NONE"
};

/**
 * @brief Classe WindowModule.
 *
 * Cette classe représente le gestionnaire de fen�tres dans le système.
 * Elle hérite de la classe Module, ce qui lui permet d'être intégrée dans le syst�me de modules.
 * Le WindowModule est responsable de la gestion et de la manipulation des fenêtres de l'application.
 */
class WindowModule final : public Module
{
	public:
		static constexpr int WIDTH  = 800; ///< Largeur de la fen�tre par d�faut.
		static constexpr int HEIGHT = 600; ///< Hauteur de la fen�tre par d�faut.


		/**
		 * @brief Constructeur de la classe WindowModule.
		 *
		 * Ce constructeur initialise un nouvel objet WindowModule.
		 */
		WindowModule();

		/**
		 * @brief Destructeur de la classe WindowModule.
		 *
		 * Ce destructeur libère les ressources associées à l'objet WindowModule.
		 */
		~WindowModule();

		/**
		 * @brief Constructeur de copie supprimé.
		 *
		 * Ce constructeur de copie est supprimé pour empêcher la duplication d'objets WindowModule.
		 */
		WindowModule(const WindowModule&) = delete;

		/**
		 * @brief Opérateur d'affectation de copie supprimé.
		 *
		 * Cet opérateur d'affectation de copie est supprimé pour empêcher la duplication d'objets WindowModule.
		 */
		WindowModule& operator=(const WindowModule&) = delete;

		/**
		 * @brief Obtient les dimensions de la fenêtre.
		 *
		 * Cette méthode retourne les dimensions de la fenêtre sous forme d'un objet vk::Extent2D.
		 *
		 * @return Les dimensions de la fenêtre.
		 */
		[[nodiscard]] vk::Extent2D GetExtent() const
		{
			return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
		}

		/**
		 * @brief Indique si la fenêtre a été redimensionnée depuis la dernière vérification.
		 *
		 * Cette méthode retourne vrai si la fenêtre a été redimensionnée depuis la dernière vérification, sinon faux.
		 *
		 * @return Vrai si la fenêtre a été redimensionnée, sinon faux.
		 */
		[[nodiscard]] bool WasWindowResized() const { return bFrameBufferResize; }


		void ResetWindowResizedFlag() { bFrameBufferResize = false; }

		/**
		 * @brief Crée une surface Vulkan associée à une fenêtre.
		 *
		 * Cette fonction crée une surface Vulkan associée à une fenêtre, permettant à Vulkan de dessiner dans cette fenêtre.
		 *
		 * @param _instance L'instance Vulkan utilisée pour créer la surface.
		 * @param _surface Un pointeur vers l'objet de surface Vulkan à créer. Ce pointeur sera mis à jour pour contenir la surface créée.
		 * @throws Une exception en cas d'échec lors de la création de la surface.
		 */
		void CreateWindowSurface(vk::Instance _instance, vk::SurfaceKHR* _surface) const;

		/**
		 * @brief Obtient un pointeur vers la fenêtre GLFW.
		 *
		 * Cette fonction retourne un pointeur vers la fenêtre GLFW associée à l'instance actuelle.
		 *
		 * @return Un pointeur vers la fenêtre GLFW.
		 */
		[[nodiscard]] GLFWwindow* GetGlfwWindow() const { return window; }

		/**
		 * @brief Réinitialise le curseur de la fenêtre.
		 *
		 * Cette fonction réinitialise le curseur de la fenêtre à sa forme par défaut.
		 */
		void ResetCursorWindow() const { glfwSetCursor(window, nullptr); }

		/**
		 * @brief Change la forme du curseur de la fenêtre en un cube coloré.
		 *
		 * Cette fonction change la forme du curseur de la fenêtre en un cube coloré, avec la possibilité de spécifier la couleur du cube.
		 *
		 * @param _color La couleur du cube. La valeur par défaut est 255.
		 */
		void CubeCursorWindow(int _color = 255) const;

		/**
		 * @brief Définit le curseur standard de la fenêtre.
		 *
		 * Cette fonction définit le curseur standard de la fenêtre à un type spécifié.
		 *
		 * @param _cursorType Le type de curseur à définir.
		 */
		void StandardCursorWindow(GlfwCursorType _cursorType) const;

		/**
		 * @brief Définit le mode de curseur d'entrée pour la fenêtre.
		 *
		 * Cette fonction définit le mode de curseur d'entrée pour la fenêtre à un mode spécifié.
		 *
		 * @param _mode Le mode de curseur d'entrée à définir.
		 */
		void SetInputCursorMode(GlfwCursorMode _mode);

		/**
		 * @brief Vérifie si la fenêtre doit être fermée.
		 *
		 * Cette méthode vérifie si la fenêtre doit être fermée en interrogeant la fonction GLFW glfwWindowShouldClose().
		 *
		 * @return true si la fenêtre doit être fermée, sinon false.
		 */
		[[nodiscard]] bool ShouldClose() const { return glfwWindowShouldClose(window); }

		/**
		 * @brief Traite tous les événements de la fenêtre en attente.
		 *
		 * Cette méthode traite tous les événements de la fenêtre en attente en appelant la fonction GLFW glfwPollEvents().
		 * Cela permet de garantir que tous les événements de la fenêtre sont traités de manière appropriée.
		 */
		void PollEvents() { glfwPollEvents(); }

		#pragma region Getter

		#pragma region Size

		/**
		 * @brief Obtient la largeur de la fenêtre.
		 *
		 * Cette méthode retourne la largeur de la fenêtre en pixels.
		 *
		 * @return La largeur de la fenêtre.
		 */
		[[nodiscard]] int GetWidth() const { return size.x; }

		/**
		 * @brief Obtient la hauteur de la fenêtre.
		 *
		 * Cette méthode retourne la hauteur de la fenêtre en pixels.
		 *
		 * @return La hauteur de la fenêtre.
		 */
		[[nodiscard]] int GetHeight() const { return size.y; }

		/**
		 * @brief Obtient la taille de la fenêtre.
		 *
		 * Cette méthode retourne la taille de la fenêtre sous forme d'un objet glm::vec2, représentant la largeur et la hauteur en pixels.
		 *
		 * @return La taille de la fenêtre.
		 */
		[[nodiscard]] glm::vec2 GetSize() const { return size; }

		/**
		 * @brief Obtient la largeur du framebuffer.
		 *
		 * Cette méthode retourne la largeur du framebuffer en pixels.
		 *
		 * @return La largeur du framebuffer.
		 */
		[[nodiscard]] int GetFrameBufferWidth() const { return frameBufferSize.x; }

		/**
		 * @brief Obtient la hauteur du framebuffer.
		 *
		 * Cette méthode retourne la hauteur du framebuffer en pixels.
		 *
		 * @return La hauteur du framebuffer.
		 */
		[[nodiscard]] int GetFrameBufferHeight() const { return frameBufferSize.y; }

		/**
		 * @brief Obtient la taille du framebuffer.
		 *
		 * Cette méthode retourne la taille du framebuffer sous forme d'un objet glm::vec2, représentant la largeur et la hauteur en pixels.
		 *
		 * @return La taille du framebuffer.
		 */
		[[nodiscard]] glm::vec2 GetFrameBufferSize() const { return frameBufferSize; }

		#pragma endregion

		#pragma region Position

		/**
		 * @brief Obtient la position de la fenêtre.
		 *
		 * Cette méthode retourne la position de la fenêtre sous forme d'un objet glm::ivec2, représentant les coordonnées x et y du coin supérieur gauche de la fenêtre en pixels.
		 *
		 * @return La position de la fenêtre.
		 */
		[[nodiscard]] glm::ivec2 GetPosition() const { return position; }

		/**
		 * @brief Obtient la position horizontale de la fenêtre.
		 *
		 * Cette méthode retourne la position horizontale (coordonnée x) du coin supérieur gauche de la fenêtre en pixels.
		 *
		 * @return La position horizontale de la fenêtre.
		 */
		[[nodiscard]] int GetPositionX() const { return position.x; }

		/**
		 * @brief Obtient la position verticale de la fenêtre.
		 *
		 * Cette méthode retourne la position verticale (coordonnée y) du coin supérieur gauche de la fenêtre en pixels.
		 *
		 * @return La position verticale de la fenêtre.
		 */
		[[nodiscard]] int GetPositionY() const { return position.y; }

		/**
		 * @brief Obtient la position initiale de la fenêtre.
		 *
		 * Cette méthode retourne la position initiale de la fenêtre sous forme d'un objet glm::ivec2, représentant les coordonnées x et y du coin supérieur gauche de la fenêtre au moment de sa création en pixels.
		 *
		 * @return La position initiale de la fenêtre.
		 */
		[[nodiscard]] glm::ivec2 GetStartingPosition() const { return startingPosition; }

		/**
		 * @brief Obtient la position horizontale initiale de la fenêtre.
		 *
		 * Cette méthode retourne la position horizontale initiale (coordonnée x) du coin supérieur gauche de la fenêtre au moment de sa création en pixels.
		 *
		 * @return La position horizontale initiale de la fenêtre.
		 */
		[[nodiscard]] int GetStartingPositionX() const { return startingPosition.x; }

		/**
		 * @brief Obtient la position verticale initiale de la fenêtre.
		 *
		 * Cette méthode retourne la position verticale initiale (coordonnée y) du coin supérieur gauche de la fenêtre au moment de sa création en pixels.
		 *
		 * @return La position verticale initiale de la fenêtre.
		 */
		[[nodiscard]] int GetStartingPositionY() const { return startingPosition.y; }

		[[nodiscard]] glm::vec2 GetMousePosition() {
			double posX, posY;
			glfwGetCursorPos(window, &posX, &posY);
			return glm::vec2((int)posX, (int)posY);
		}

		#pragma endregion

		/**
		 * @brief Vérifie si un redimensionnement du framebuffer de la fenêtre est en cours.
		 *
		 * Cette méthode retourne true si un redimensionnement du framebuffer de la fenêtre est en cours, sinon elle retourne false.
		 *
		 * @return true si un redimensionnement du framebuffer de la fenêtre est en cours, sinon false.
		 */
		[[nodiscard]] bool GetFrameBufferResize() const { return bFrameBufferResize; }

		/**
		 * @brief Vérifie si la fenêtre a le focus.
		 *
		 * Cette méthode retourne true si la fenêtre a le focus (est au premier plan et est active), sinon elle retourne false.
		 *
		 * @return true si la fenêtre a le focus, sinon false.
		 */
		[[nodiscard]] bool HasFocus() const { return bHasFocus; }

		#pragma region Name&Title

		/**
		 * @brief Obtient le nom de la fenêtre.
		 *
		 * Cette méthode retourne le nom de la fenêtre.
		 *
		 * @return Le nom de la fenêtre.
		 */
		[[nodiscard]] std::string GetName() const { return windowName; }

		/**
		 * @brief Obtient le titre de la fenêtre.
		 *
		 * Cette méthode retourne le titre de la fenêtre affiché dans la barre de titre.
		 *
		 * @return Le titre de la fenêtre.
		 */
		[[nodiscard]] std::string GetWindowTitle() const { return windowTitle; }

		#pragma endregion

		/**
		 * @brief Obtient le rapport largeur-hauteur de la fenêtre.
		 *
		 * Cette méthode retourne le rapport largeur-hauteur de la fenêtre, calculé en divisant la largeur par la hauteur.
		 *
		 * @return Le rapport largeur-hauteur de la fenêtre.
		 */
		[[nodiscard]] float GetAspectRatio() const { return static_cast<float>(size.x) / static_cast<float>(size.y); }


		/**
		 * @brief Obtient l'inverse du rapport largeur-hauteur de la fenêtre.
		 *
		 * Cette méthode retourne l'inverse du rapport largeur-hauteur de la fenêtre, calculé en divisant la hauteur par la largeur.
		 *
		 * @return L'inverse du rapport largeur-hauteur de la fenêtre.
		 */
		[[nodiscard]] float GetInvAspectRatio() const
		{
			return static_cast<float>(size.y) / static_cast<float>(size.x);
		}

		/**
		 * @brief Vérifie si la synchronisation verticale est activée.
		 *
		 * Cette méthode retourne true si la synchronisation verticale (VSync) est activée pour la fenêtre, sinon elle retourne false.
		 *
		 * @return true si la synchronisation verticale est activée, sinon false.
		 */
		[[nodiscard]] bool GetVSyncEnabled() const { return bVSyncEnabled; }

		#pragma region Mode

		/**
		 * @brief Obtient le mode de curseur de la fenêtre.
		 *
		 * Cette méthode retourne le mode de curseur actuel de la fenêtre.
		 *
		 * @return Le mode de curseur de la fenêtre.
		 */
		[[nodiscard]] GlfwCursorMode GetCursorMode() const { return cursorMode; }

		/**
		 * @brief Obtient le mode de fenêtre actuel.
		 *
		 * Cette méthode retourne le mode de fenêtre actuellement utilisé.
		 *
		 * @return Le mode de fenêtre actuel.
		 */
		[[nodiscard]] WindowMode GetWindowMode() const { return currentWindowMode; }

		#pragma endregion

		/**
		 * @brief Obtient le texte actuellement copié dans le presse-papiers de la fenêtre.
		 *
		 * Cette méthode retourne le texte actuellement copié dans le presse-papiers de la fenêtre.
		 *
		 * @return Le texte actuellement copié dans le presse-papiers de la fenêtre.
		 */
		[[nodiscard]] const char* GetClipboardText() const { return glfwGetClipboardString(window); }

		#pragma endregion

		#pragma region Setter

		#pragma region Size

		/**
		 * @brief Définit la taille de la fenêtre.
		 *
		 * Cette méthode définit la taille de la fenêtre à la nouvelle taille spécifiée.
		 *
		 * @param _newSize La nouvelle taille de la fenêtre.
		 */
		void SetSize(const glm::ivec2& _newSize) { size = _newSize; }

		/**
		 * @brief Définit la taille de la fenêtre en spécifiant la largeur et la hauteur.
		 *
		 * Cette méthode définit la taille de la fenêtre en spécifiant la largeur et la hauteur.
		 *
		 * @param _newWidth La nouvelle largeur de la fenêtre.
		 * @param _newHeight La nouvelle hauteur de la fenêtre.
		 */
		void SetSize(const int32_t& _newWidth, const int32_t& _newHeight) { size = glm::ivec2(_newWidth, _newHeight); }

		/**
		 * @brief Définit la largeur de la fenêtre.
		 *
		 * Cette méthode définit la largeur de la fenêtre à la nouvelle largeur spécifiée.
		 *
		 * @param _newWidth La nouvelle largeur de la fenêtre.
		 */
		void SetWidth(const int& _newWidth) { size.x = _newWidth; }

		/**
		 * @brief Définit la hauteur de la fenêtre.
		 *
		 * Cette méthode définit la hauteur de la fenêtre à la nouvelle hauteur spécifiée.
		 *
		 * @param _newHeight La nouvelle hauteur de la fenêtre.
		 */
		void SetHeight(const int& _newHeight) { size.y = _newHeight; }

		/**
		 * @brief Définit la taille du framebuffer de la fenêtre.
		 *
		 * Cette méthode définit la taille du framebuffer de la fenêtre à la nouvelle taille spécifiée.
		 *
		 * @param _newSize La nouvelle taille du framebuffer de la fenêtre.
		 */
		void SetFrameBufferSize(const glm::vec2& _newSize) { frameBufferSize = _newSize; }

		/**
		 * @brief Définit la taille du framebuffer de la fenêtre en spécifiant la largeur et la hauteur.
		 *
		 * Cette méthode définit la taille du framebuffer de la fenêtre en spécifiant la largeur et la hauteur.
		 *
		 * @param _newWidth La nouvelle largeur du framebuffer de la fenêtre.
		 * @param _newHeight La nouvelle hauteur du framebuffer de la fenêtre.
		 */
		void SetFrameBufferSize(const int32_t& _newWidth, const int32_t& _newHeight)
		{
			frameBufferSize = glm::vec2(_newWidth, _newHeight);
		}

		/**
		 * @brief Définit la largeur du framebuffer de la fenêtre.
		 *
		 * Cette méthode définit la largeur du framebuffer de la fenêtre à la nouvelle largeur spécifiée.
		 *
		 * @param _newWidth La nouvelle largeur du framebuffer de la fenêtre.
		 */

		void SetFrameBufferWidth(const int& _newWidth) { frameBufferSize.x = _newWidth; }
		/**
		 * @brief Définit la hauteur du framebuffer de la fenêtre.
		 *
		 * Cette méthode définit la hauteur du framebuffer de la fenêtre à la nouvelle hauteur spécifiée.
		 *
		 * @param _newHeight La nouvelle hauteur du framebuffer de la fenêtre.
		 */
		void SetFrameBufferHeight(const int& _newHeight) { frameBufferSize.y = _newHeight; }

		#pragma endregion


		#pragma region Position

		/**
		 * @brief Définit la position de la fenêtre en spécifiant les coordonnées x et y.
		 *
		 * Cette méthode définit la position de la fenêtre en spécifiant les coordonnées x et y.
		 *
		 * @param _position Les nouvelles coordonnées x et y de la fenêtre.
		 */
		void SetPosition(const glm::ivec2& _position)
		{
			if (window) glfwSetWindowPos(window, _position.x, _position.y);
			else startingPosition = _position;
			OnPositionChanged(_position);
		}

		/**
		 * @brief Définit la position de la fenêtre en spécifiant la coordonnée x et y.
		 *
		 * Cette méthode définit la position de la fenêtre en spécifiant la coordonnée x et y.
		 *
		 * @param _newPositionX La nouvelle coordonnée x de la fenêtre.
		 * @param _newPositionY La nouvelle coordonnée y de la fenêtre.
		 */
		void SetPosition(const int32_t& _newPositionX, const int32_t& _newPositionY)
		{
			if (window) glfwSetWindowPos(window, _newPositionX, _newPositionY);
			else startingPosition = {_newPositionX, _newPositionY};
			OnPositionChanged(_newPositionX, _newPositionY);
		}

		/**
		 * @brief Définit la coordonnée x de la position de la fenêtre.
		 *
		 * Cette méthode définit la coordonnée x de la position de la fenêtre à la nouvelle valeur spécifiée.
		 *
		 * @param _positionX La nouvelle coordonnée x de la position de la fenêtre.
		 */
		void SetPositionX(const int& _positionX)
		{
			if (window) glfwSetWindowPos(window, _positionX, position.y);
			else startingPosition = {_positionX, position.y};
			OnPositionChanged(_positionX, position.y);
		}

		/**
		 * @brief Définit la coordonnée y de la position de la fenêtre.
		 *
		 * Cette méthode définit la coordonnée y de la position de la fenêtre à la nouvelle valeur spécifiée.
		 *
		 * @param _positionY La nouvelle coordonnée y de la position de la fenêtre.
		 */
		void SetPositionY(const int& _positionY)
		{
			if (window) glfwSetWindowPos(window, position.x, _positionY);
			else startingPosition = {position.x, _positionY};
			OnPositionChanged(position.x, _positionY);
		}

		/**
		 * @brief Définit la position du curseur de la souris.
		 *
		 * Cette méthode définit la position du curseur de la souris à la nouvelle position spécifiée.
		 *
		 * @param _mousePosition La nouvelle position du curseur de la souris.
		 */
		void SetMousePosition(const glm::ivec2 _mousePosition) const
		{
			glfwSetCursorPos(window, _mousePosition.x, _mousePosition.y);
		}

		#pragma endregion


		#pragma region Name&Title

		/**
		 * @brief Définit le nom de la fenêtre.
		 *
		 * Cette méthode définit le nom de la fenêtre à la nouvelle valeur spécifiée.
		 *
		 * @param _newName Le nouveau nom de la fenêtre.
		 */
		void SetName(const std::string& _newName) { windowName = _newName; }

		/**
		 * @brief Définit le titre de la fenêtre affiché dans la barre de titre.
		 *
		 * Cette méthode définit le titre de la fenêtre affiché dans la barre de titre à la nouvelle valeur spécifiée.
		 *
		 * @param _windowTitle Le nouveau titre de la fenêtre.
		 */
		void SetWindowTitle(const std::string& _windowTitle)
		{
			windowTitle = _windowTitle;
			glfwSetWindowTitle(window, windowTitle.c_str());
		}

		#pragma endregion
		/**
		 * @brief Définit l'état de redimensionnement du framebuffer de la fenêtre.
		 *
		 * Cette méthode définit l'état de redimensionnement du framebuffer de la fenêtre à l'état spécifié.
		 *
		 * @param _state L'état de redimensionnement du framebuffer de la fenêtre.
		 */
		void SetFrameBufferResize(const bool& _state) { bFrameBufferResize = _state; }

		/**
		 * @brief Définit la fréquence de mise à jour du titre de la fenêtre.
		 *
		 * Cette méthode définit la fréquence de mise à jour du titre de la fenêtre en secondes.
		 *
		 * @param _updateFrequencyInSeconds La fréquence de mise à jour du titre de la fenêtre en secondes.
		 */

		void SetUpdateWindowTitleFrequency(const float& _updateFrequencyInSeconds)
		{
			updateWindowTitleFrequency = _updateFrequencyInSeconds;
		}

		/**
		 * @brief Active ou désactive la synchronisation verticale (VSync) de la fenêtre.
		 *
		 * Cette méthode active ou désactive la synchronisation verticale (VSync) de la fenêtre selon l'état spécifié.
		 *
		 * @param _bEnabled État de la synchronisation verticale (VSync).
		 */
		void SetVSyncEnabled(const bool& _bEnabled) { bVSyncEnabled = _bEnabled; }

		/**
		 * @brief Définit le mode du curseur de la fenêtre.
		 *
		 * Cette méthode définit le mode du curseur de la fenêtre à un mode spécifié.
		 *
		 * @param _mode Le mode du curseur de la fenêtre.
		 */
		void SetCursorMode(const GlfwCursorMode& _mode)
		{
			if (cursorMode != _mode) cursorMode = _mode;
			//g_InputManager->OnCursorModeChanged(_mode);
		}

		/**
		 * @brief Convertit le mode de curseur actuel en une chaîne de caractères.
		 *
		 * Cette méthode convertit le mode de curseur actuel en une chaîne de caractères représentant le mode.
		 *
		 * @return Une chaîne de caractères représentant le mode de curseur actuel.
		 */
		const char* WindowModeToStr() const { return WindowModeStrings[static_cast<int32_t>(GetCursorMode())]; }

		/**
		 * @brief Définit le mode de la fenêtre.
		 *
		 * Cette méthode définit le mode de la fenêtre à un mode spécifié.
		 *
		 * @param _mode Le mode de la fenêtre.
		 * @param _bForce Spécifie si le changement de mode doit être forcé même s'il est identique au mode actuel. La valeur par défaut est false.
		 */
		void SetWindowMode(const WindowMode& _mode, const bool _bForce = false)
		{
			if (_bForce || currentWindowMode != _mode)
			{
				currentWindowMode = _mode;

				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				if (!monitor)
				{
					std::cout << ("Failed to find primary monitor! Can't set window mode\n");
					return;
				}

				const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
				if (!video_mode)
				{
					std::cout << ("Failed to get monitor's video mode! Can't set window mode\n");
					return;
				}

				switch (_mode)
				{
					case WindowMode::FULLSCREEN:
						{
							glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height,
							                     video_mode->refreshRate);
						}
						break;
					case WindowMode::WINDOWED_FULLSCREEN:
						{
							glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height,
							                     video_mode->refreshRate);
							lastNonFullscreenWindowMode = WindowMode::WINDOWED_FULLSCREEN;
						}
						break;
					case WindowMode::WINDOWED:
						{
							//CHECK(lastWindowedSize.x != 0 && lastWindowedSize.y != 0);

							if (lastWindowedPos.y == 0)
								// When in windowed mode a y position of 0 means the title bar isn't
								// visible. This will occur if the app launched in fullscreen since
								// the last y position to never have been set to a valid value.
								lastWindowedPos.y = 40;

							glfwSetWindowMonitor(window, nullptr, lastWindowedPos.x, lastWindowedPos.y,
							                     lastWindowedSize.x, lastWindowedSize.y, video_mode->refreshRate);
							lastNonFullscreenWindowMode = WindowMode::WINDOWED;
						}
						break;
					case WindowMode::_NONE:
					default:
						{
							std::cout << ("Unhandled window mode: %u\n", static_cast<uint32_t>(_mode));
						}
						break;
				}
			}
		}

		/**
		 * @brief Définit le texte du presse-papiers.
		 *
		 * Cette méthode définit le texte du presse-papiers avec la chaîne spécifiée.
		 *
		 * @param _text La chaîne à copier dans le presse-papiers.
		 */
		void SetClipboardText(const char* _text) const
		{
			glfwSetClipboardString(window, _text);
		}

		/**
		 * @brief Définit l'icône de la fenêtre.
		 *
		 * Cette méthode définit l'icône de la fenêtre avec l'image spécifiée dans le fichier.
		 *
		 * @param _fileName Le nom du fichier contenant l'image de l'icône de la fenêtre.
		 */
		void SetWindowIcon(const std::string& _fileName) const
		{
			int width, height, nr_channels;
			if (unsigned char* data = stbi_load(_fileName.c_str(), &width, &height, &nr_channels, 0))
			{
				GLFWimage icon;
				icon.width  = width;
				icon.height = height;
				icon.pixels = data;
				glfwSetWindowIcon(window, 1, &icon);
			}
		}

		#pragma endregion

		/**
		 * @brief Appelé lorsque la taille de la fenêtre est modifiée.
		 *
		 * Cette méthode est appelée lorsque la taille de la fenêtre est modifiée avec les dimensions spécifiées.
		 *
		 * @param _width La nouvelle largeur de la fenêtre.
		 * @param _height La nouvelle hauteur de la fenêtre.
		 */
		void OnSizeChanged(const int32_t _width, const int32_t _height)
		{
			size            = glm::ivec2(_width, _height);
			frameBufferSize = size;
			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedSize = size;

			/*if (g_Renderer)
			{
				g_Renderer->OnWindowSizeChanged(_width, _height);
			}*/
		}

		/**
		 * @brief Appelé lorsque la taille de la fenêtre est modifiée.
		 *
		 * Cette méthode est appelée lorsque la taille de la fenêtre est modifiée avec la nouvelle position spécifiée.
		 *
		 * @param _newPosition Les nouvelles dimensions de la fenêtre.
		 */
		void OnSizeChanged(const glm::ivec2& _newPosition)
		{
			size            = _newPosition;
			frameBufferSize = size;
			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedSize = size;

			/*if (g_Renderer)
			{
				g_Renderer->OnWindowSizeChanged(_width, _height);
			}*/
		}

		/**
		 * @brief Appelé lorsque la position de la fenêtre est modifiée.
		 *
		 * Cette méthode est appelée lorsque la position de la fenêtre est modifiée avec les coordonnées spécifiées.
		 *
		 * @param _newPositionX La nouvelle position en X de la fenêtre.
		 * @param _newPositionY La nouvelle position en Y de la fenêtre.
		 */
		void OnPositionChanged(const int32_t _newPositionX, const int32_t _newPositionY)
		{
			position = glm::ivec2{_newPositionX, _newPositionY};

			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedPos = position;
		}

		/**
		 * @brief Appelé lorsque la position de la fenêtre est modifiée.
		 *
		 * Cette méthode est appelée lorsque la position de la fenêtre est modifiée avec la nouvelle position spécifiée.
		 *
		 * @param _newPosition La nouvelle position de la fenêtre.
		 */
		void OnPositionChanged(const glm::ivec2& _newPosition)
		{
			position = _newPosition;

			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedPos = position;
		}

		//TODO : faire les fonctions de callback
		static Inputs::KeyAction GLFWActionToInputManagerAction(int32_t _glfwAction);
		static Inputs::KeyCode GLFWKeyToInputManagerKey(int32_t _glfwKey);
		static int32_t GLFWModsToInputManagerMods(int32_t _glfwMods);
		static Inputs::MouseButton GLFWButtonToInputManagerMouseButton(int32_t _glfwButton);

		static void GLFWMouseButtonCallback(GLFWwindow* _glfwWindow, int32_t _button, int32_t _action, int32_t _mods);
		static void GLFWKeyCallback(GLFWwindow* _glfwWindow, int32_t _key, int32_t _scancode, int32_t _action, int32_t _mods);
		static void GLFWCursorPosCallback(GLFWwindow* _glfwWindow, double _x, double _y);

		void MouseButtonCallback(Inputs::MouseButton _mouseButton, Inputs::KeyAction _action, int32_t _mods);
		void KeyCallback(Inputs::KeyCode _keyCode, Inputs::KeyAction _keyAction, int32_t _mods);
		void CursorPosCallback(double x, double y);
		/*void KeyCallback(Inputs::KeyCode keycode, Inputs::KeyAction action, int32_t mods){}
		void CharCallback(uint32_t character){}
		void MouseButtonCallback(Inputs::MouseButton mouseButton, Inputs::KeyAction action, int32_t mods){}
		void WindowFocusCallback(int32_t focused){}
		void CursorPosCallback(double x, double y){}
		void ScrollCallback(double xoffset, double yoffset){}
		void WindowSizeCallback(int32_t width, int32_t height, bool bMaximized, bool bIconified){}
		void WindowPosCallback(int32_t newX, int32_t newY){}
		void FrameBufferSizeCallback(int32_t width, int32_t height){}*/

		/**
		 * @brief Convertit une chaîne de caractères en mode de fenêtre.
		 *
		 * Cette fonction convertit une chaîne de caractères en un mode de fenêtre correspondant.
		 *
		 * @param _modeStr La chaîne de caractères représentant le mode de fenêtre à convertir.
		 * @return Le mode de fenêtre correspondant à la chaîne de caractères.
		 */
		WindowMode StrToWindowMode(const char* _modeStr);

		/**
		 * @brief Bascule entre le mode plein écran et le mode fenêtré.
		 *
		 * Cette méthode bascule entre le mode plein écran et le mode fenêtré de la fenêtre. Si le mode actuel est plein écran, la fenêtre bascule en mode fenêtré, et vice versa.
		 *
		 * @param _bForce Indique si le changement de mode doit être forcé.
		 */
		void ToggleFullscreen(const bool _bForce = false)
		{
			if (currentWindowMode == WindowMode::FULLSCREEN)
				//CHECK(lastNonFullscreenWindowMode == WindowMode::WINDOWED || lastNonFullscreenWindowMode == WindowMode::WINDOWED_FULLSCREEN);

				SetWindowMode(lastNonFullscreenWindowMode, _bForce);
			else SetWindowMode(WindowMode::FULLSCREEN, _bForce);
		}

		/**
		 * @brief Maximise la fenêtre.
		 *
		 * Cette méthode maximise la fenêtre à l'écran.
		 */
		void Maximize() const { glfwMaximizeWindow(window); }

		/**
		 * @brief Minimise la fenêtre.
		 *
		 * Cette méthode minimise la fenêtre.
		 */
		void Iconify() const { glfwIconifyWindow(window); }

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

	private:
		/**
		 * @brief Callback function for framebuffer resize events.
		 *
		 * This function is called when the framebuffer of the window is resized.
		 *
		 * @param _window The window that triggered the resize event.
		 * @param _width The new width of the framebuffer.
		 * @param _height The new height of the framebuffer.
		 */
		static void FrameBufferResizeCallBack(GLFWwindow* _window, int _width, int _height);
		/**
		 * @brief Génère le titre de la fenêtre en fonction de ses paramètres.
		 *
		 * Cette méthode génère le titre de la fenêtre en fonction de ses paramètres actuels, tel que la résolution, le mode de fenêtre, etc.
		 *
		 * @return Le titre de la fenêtre généré.
		 */
		std::string GenerateWindowTitle() const;

		/**
		 * @brief Initialise la fenêtre à partir de la configuration.
		 *
		 * Cette méthode initialise la fenêtre en utilisant les paramètres définis dans la configuration. Elle charge les paramètres tels que la résolution, le mode de fenêtre, etc.
		 *
		 * @return True si l'initialisation réussit, sinon false.
		 */
		bool InitFromConfig();

		/**
		 * @brief Sauvegarde les paramètres de la fenêtre dans la configuration.
		 *
		 * Cette méthode sauvegarde les paramètres actuels de la fenêtre dans la configuration, tels que la résolution, le mode de fenêtre, etc.
		 */
		void SaveToConfig();

		bool bFrameBufferResize = false; /**< Booléen indiquant si le framebuffer a été redimensionné. */

		SceneManager* sceneManager = nullptr; /**< Gestionnaire de scène associé à la fenêtre. */
		InputModule* inputModule = nullptr;

		std::string windowName; /**< Nom de la fenêtre. */

		std::string windowTitle; /**< Titre de la fenêtre. */

		GLFWwindow* window; /**< Pointeur vers la fenêtre GLFW. */

		glm::ivec2 size = {0, 0}; /**< Dimensions de la fenêtre. */

		glm::ivec2 startingPosition = {0, 0}; /**< Position de départ de la fenêtre. */

		glm::ivec2 position = {0, 0}; /**< Position actuelle de la fenêtre. */

		glm::ivec2 frameBufferSize = {0, 0}; /**< Dimensions du framebuffer. */

		bool bHasFocus = false; /**< Indique si la fenêtre a le focus. */

		bool bMoveConsoleToOtherMonitor = true;
		/**< Indique si la console doit être déplacée sur un autre moniteur lorsqu'elle est présente. */

		bool bAutoRestoreStateOnBootup = true;
		/**< Indique si la taille et la position doivent être restaurées lors du démarrage. */

		WindowMode currentWindowMode = WindowMode::_NONE; /**< Mode actuel de la fenêtre. */

		glm::ivec2 lastWindowedSize; /**< Dimensions de la fenêtre avant d'entrer en mode plein écran. */

		glm::ivec2 lastWindowedPos; /**< Position de la fenêtre avant d'entrer en mode plein écran. */

		WindowMode lastNonFullscreenWindowMode = WindowMode::_NONE;
		/**< Mode de fenêtre précédent avant d'entrer en mode plein écran. */

		bool bShowFPSInWindowTitle = true;
		/**< Indique si les FPS doivent être affichées dans le titre de la fenêtre. */

		bool bShowMSInWindowTitle = true;
		/**< Indique si les millisecondes doivent être affichées dans le titre de la fenêtre. */

		bool bMaximized = false; /**< Indique si la fenêtre est maximisée. */

		bool bIconified = false; /**< Indique si la fenêtre est réduite. */

		bool bVSyncEnabled = true; /**< Indique si la synchronisation verticale est activée. */

		float updateWindowTitleFrequency = 0.0f; /**< Fréquence de mise à jour du titre de la fenêtre. */

		float secondsSinceTitleUpdate = 0.0f;
		/**< Temps écoulé depuis la dernière mise à jour du titre de la fenêtre. */

		GlfwCursorMode cursorMode = NORMAL; /**< Mode du curseur GLFW. */
};
