#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve
{
	class LveWindow
	{
		public:
			/**
			 * @brief Enumération des types de curseurs GLFW.
			 *
			 * Cette énumération représente les différents types de curseurs GLFW disponibles.
			 * Ces types peuvent être utilisés pour spécifier le style du curseur à afficher.
			 */
			enum GlfwCursorType
			{
				ARROW,
				/**< Curseur flèche. */
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
				NOT_ALLOWED /**< Curseur non autorisé. */
			};

			/**
			 * @brief Enumération des modes de curseurs GLFW.
			 *
			 * Cette énumération représente les différents modes de curseurs GLFW.
			 * Ces modes définissent le comportement du curseur sur l'écran.
			 */
			enum GlfwCursorMode
			{
				DISABLED,
				/**< Curseur désactivé. */
				HIDDEN,
				/**< Curseur caché. */
				NORMAL,
				/**< Curseur normal (visible). */
				CAPTURED /**< Curseur capturé (exclusif à la fenêtre). */
			};

			/**
			 * @brief Constructeur de la classe LveWindow.
			 *
			 * Initialise une instance de LveWindow avec les dimensions spécifiées et le nom de la fenêtre.
			 *
			 * @param _width La largeur de la fenêtre.
			 * @param _height La hauteur de la fenêtre.
			 * @param _name Le nom de la fenêtre.
			 * @return Rien.
			 */
			LveWindow(int _width, int _height, const std::string& _name);

			/**
			 * @brief Destructeur de la classe LveWindow.
			 *
			 * Détruit la fenêtre GLFW.
			 */
			~LveWindow();

			/**
			 * @brief Constructeur de copie supprimé.
			 *
			 * La copie d'objets de type LveWindow est explicitement interdite pour éviter les problèmes de gestion
			 * des ressources associées à la fenêtre.
			 */
			LveWindow(const LveWindow&) = delete;

			/**
			 * @brief Opérateur d'affectation par copie supprimé.
			 *
			 * L'affectation par copie d'objets de type LveWindow est explicitement interdite pour éviter les problèmes
			 * de gestion des ressources associées à la fenêtre.
			 *
			 * @return Une référence vers l'objet LveWindow après affectation.
			 */
			LveWindow& operator=(const LveWindow&) = delete;

			/**
			 * @brief Vérifie si la fenêtre doit être fermée.
			 *
			 * Cette fonction renvoie true si la fenêtre doit être fermée, false sinon.
			 *
			 * @return true si la fenêtre doit être fermée, false sinon.
			 */
			bool ShouldClose() const { return glfwWindowShouldClose(window); }

			/**
			 * @brief Obtient les dimensions de la fenêtre.
			 *
			 * Cette fonction retourne les dimensions de la fenêtre sous forme d'une structure vk::Extent2D.
			 *
			 * @return Les dimensions de la fenêtre sous forme d'une structure vk::Extent2D.
			 */
			vk::Extent2D GetExtent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

			/**
			 * @brief Vérifie si la fenêtre a été redimensionnée.
			 *
			 * Cette fonction vérifie si la fenêtre a été redimensionnée depuis la dernière vérification.
			 *
			 * @return true si la fenêtre a été redimensionnée, sinon false.
			 */

			bool WasWindowResized() const { return frameBufferResize; }

			/**
			 * @brief Réinitialise le drapeau de redimensionnement de la fenêtre.
			 *
			 * Cette fonction réinitialise le drapeau indiquant que la fenêtre a été redimensionnée.
			 */
			void ResetWindowResizedFlag() { frameBufferResize = false; }

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
			 * @param _color
			 * @param color La couleur du cube. La valeur par défaut est 255.
			 */
			void CubeCursorWindow(int _color = 255) const;

			/**
			 * @brief Définit le curseur standard de la fenêtre.
			 *
			 * Cette fonction définit le curseur standard de la fenêtre à un type spécifié.
			 *
			 * @param _cursorType
			 * @param CursorType Le type de curseur à définir.
			 */
			void StandardCursorWindow(GlfwCursorType _cursorType) const;

			/**
			 * @brief Définit le mode de curseur d'entrée pour la fenêtre.
			 *
			 * Cette fonction définit le mode de curseur d'entrée pour la fenêtre à un mode spécifié.
			 *
			 * @param _mode
			 * @param mode Le mode de curseur d'entrée à définir.
			 */
			void SetInputCursorMode(GlfwCursorMode _mode) const;

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
			 * @brief Initialise la fenêtre GLFW.
			 *
			 * Cette fonction initialise GLFW, configure la fenêtre pour être non redimensionnable et crée une fenêtre GLFW avec la taille et le nom spécifiés.
			 */
			void InitWindow();

			int  width;                     // Largeur de la fenêtre
			int  height;                    // Hauteur de la fenêtre
			bool frameBufferResize = false; // Booléen indiquant si le framebuffer a été redimensionné.

			std::string windowName; // Nom de la fenêtre
			GLFWwindow* window;     // Fenêtre GLFW
	};
} // namespace lve
