#pragma once

#include "LveEngine/lve_device.h"

// vulkan headers
#include <vulkan/vulkan.hpp>

// std lib headers
#include <memory>
#include <vector>

namespace lve
{
	/**
	* @brief Classe représentant une chaîne d'échange Vulkan.
	*/
	class LveSwapChain
	{
		public:
			static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

			/**
			 * @brief Constructeur prenant une référence à un objet LveDevice et une étendue windowExtent.
			 *
			 * @param _deviceRef Référence à un objet LveDevice.
			 * @param _windowExtent Étendue de la fenêtre.
			 */
			LveSwapChain(LveDevice& _deviceRef, vk::Extent2D _windowExtent);

			/**
			 * @brief Constructeur prenant une référence à un objet LveDevice, une étendue extent
			 *        et un pointeur partagé vers une précédente chaîne d'échanges.
			 *
			 * @param _deviceRef Référence à un objet LveDevice.
			 * @param _extent Étendue de la chaîne d'échanges.
			 * @param _previous Pointeur partagé vers une précédente chaîne d'échanges.
			 */
			LveSwapChain(LveDevice& _deviceRef, vk::Extent2D _extent, const std::shared_ptr<LveSwapChain>& _previous);

			/**
			 * @brief Destructeur.
			 */
			~LveSwapChain();

			/**
			 * @brief Initialise la chaîne d'échanges.
			 */
			void Init();

			/**
			 * @brief Constructeur de recopie supprimé.
			 */
			LveSwapChain(const LveSwapChain&) = delete;

			/**
			 * @brief Opérateur d'assignation par recopie supprimé.
			 */
			LveSwapChain& operator=(const LveSwapChain&) = delete;

			/**
			 * @brief Obtient le framebuffer correspondant à l'indice spécifié.
			 *
			 * Cette fonction retourne le framebuffer de la swap chain qui correspond à l'indice spécifié.
			 *
			 * @param _index L'indice du framebuffer à récupérer.
			 * @return vk::Framebuffer Le framebuffer correspondant à l'indice spécifié.
			 */
			vk::Framebuffer GetFrameBuffer(const int _index) const { return swapChainFramebuffers[_index]; }

			/**
			 * @brief Obtient le passe de rendu.
			 *
			 * Cette fonction retourne le passe de rendu associé à la swap chain.
			 *
			 * @return vk::RenderPass Le passe de rendu associé à la swap chain.
			 */
			vk::RenderPass GetRenderPass() const { return renderPass; }

			/**
			 * @brief Obtient la vue d'image correspondant à l'indice spécifié.
			 *
			 * Cette fonction retourne la vue d'image de la swap chain qui correspond à l'indice spécifié.
			 *
			 * @param _index L'indice de la vue d'image à récupérer.
			 * @return vk::ImageView La vue d'image correspondant à l'indice spécifié.
			 */
			vk::ImageView GetImageView(const int _index) const { return swapChainImageViews[_index]; }

			/**
			 * @brief Obtient le nombre d'images dans la swap chain.
			 *
			 * Cette fonction retourne le nombre d'images actuellement présentes dans la swap chain.
			 *
			 * @return size_t Le nombre d'images dans la swap chain.
			 */
			size_t ImageCount() const { return swapChainImages.size(); }

			/**
			 * @brief Obtient le format d'image de la swap chain.
			 *
			 * Cette fonction retourne le format d'image utilisé par la swap chain.
			 *
			 * @return vk::Format Le format d'image de la swap chain.
			 */
			vk::Format GetSwapChainImageFormat() const { return swapChainImageFormat; }

			/**
			 * @brief Obtient l'étendue de la swap chain.
			 *
			 * Cette fonction retourne l'étendue (largeur et hauteur) de la swap chain.
			 *
			 * @return vk::Extent2D L'étendue de la swap chain.
			 */
			vk::Extent2D GetSwapChainExtent() const { return swapChainExtent; }

			/**
			 * @brief Obtient la largeur de la swap chain.
			 *
			 * Cette fonction retourne la largeur de la swap chain.
			 *
			 * @return uint32_t La largeur de la swap chain.
			 */
			uint32_t Width() const { return swapChainExtent.width; }

			/**
			 * @brief Obtient la hauteur de la swap chain.
			 *
			 * Cette fonction retourne la hauteur de la swap chain.
			 *
			 * @return uint32_t La hauteur de la swap chain.
			 */
			uint32_t Height() const { return swapChainExtent.height; }

			/**
			 * @brief Calcule le rapport d'aspect de l'étendue de la swap chain.
			 *
			 * Cette fonction calcule le rapport d'aspect (largeur/hauteur) de l'étendue de la swap chain.
			 *
			 * @return float Le rapport d'aspect de l'étendue de la swap chain.
			 */
			float ExtentAspectRatio() const
			{
				return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
			}

			/**
			 * @brief Recherche le format de profondeur adapté.
			 *
			 * Cette fonction recherche le format de profondeur adapté parmi une liste de formats donnée, en tenant compte des caractéristiques spécifiées.
			 *
			 * @return vk::Format Le format de profondeur adapté.
			 */
			vk::Format FindDepthFormat() const;

			/**
			 * @brief Acquiert l'index de l'image suivante dans la chaîne de swaps.
			 *
			 * Cette fonction acquiert l'index de l'image suivante dans la chaîne de swaps.
			 *
			 * @param _imageIndex Pointeur vers la variable où stocker l'index de l'image acquise.
			 * @return VkResult Le résultat de l'opération.
			 */
			vk::Result AcquireNextImage(uint32_t* _imageIndex) const;

			/**
			 * @brief Soumet les command buffers pour exécution et présente le résultat.
			 *
			 * Cette fonction soumet les command buffers spécifiés pour exécution, attend leur achèvement, puis présente le résultat à l'écran.
			 *
			 * @param _buffers Tableau des command buffers à soumettre.
			 * @param _imageIndex Pointeur vers l'index de l'image à présenter.
			 * @return VkResult Le résultat de l'opération.
			 */
			vk::Result SubmitCommandBuffers(const vk::CommandBuffer* _buffers, uint32_t* _imageIndex);

			/**
			 * @brief Compare les formats de profondeur et d'image avec une autre LveSwapChain.
			 *
			 * Cette fonction compare les formats de profondeur et d'image de cette LveSwapChain avec ceux d'une autre LveSwapChain spécifiée.
			 *
			 * @param _swapChain La LveSwapChain avec laquelle comparer les formats.
			 * @return bool True si les formats de profondeur et d'image sont identiques, sinon False.
			 */
			bool CompareSwapFormats(const LveSwapChain& _swapChain) const
			{
				return _swapChain.swapChainDepthFormat == swapChainDepthFormat &&
				       _swapChain.swapChainImageFormat == swapChainImageFormat;
			}

		private:
			/**
			 * @brief Crée la chaîne de swaps.
			 *
			 * Cette fonction crée la chaîne de swaps, qui est utilisée pour la présentation des images à l'écran.
			 * La création de la chaîne de swaps implique la sélection des formats de surface, des modes de présentation et des dimensions appropriés, ainsi que la configuration de divers autres paramètres liés à la présentation.
			 *
			 * @throws std::runtime_error si la création de la chaîne de swaps échoue.
			 */
			void CreateSwapChain();

			/**
			 * @brief Crée les vues d'image de la chaîne de swaps.
			 *
			 * Cette fonction crée les vues d'image pour chaque image de la chaîne de swaps.
			 * Chaque vue d'image est associée à une image de la chaîne de swaps et spécifie comment cette image doit être interprétée lorsqu'elle est utilisée comme texture.
			 *
			 * @throws std::runtime_error si la création d'une vue d'image échoue.
			 */
			void CreateImageViews();

			/**
			* @brief Crée les ressources de profondeur pour la chaîne de swaps.
			*
			* Cette fonction crée les ressources de profondeur nécessaires pour chaque image de la chaîne de swaps.
			* Ces ressources de profondeur sont utilisées pour le rendu des scènes en 3D afin de déterminer la profondeur des fragments et gérer la détection des collisions.
			*
			* @throws std::runtime_error si la création d'une ressource de profondeur échoue.
			*/
			void CreateDepthResources();

			/**
			 * @brief Crée la passe de rendu.
			 *
			 * Cette fonction crée la passe de rendu utilisée pour définir comment les données de géométrie sont traitées lors du rendu d'une image.
			 * La passe de rendu définit les attachements de couleur et de profondeur, ainsi que les sous-passes qui décrivent les opérations de rendu à exécuter.
			 *
			 * @throws std::runtime_error si la création de la passe de rendu échoue.
			 */
			void CreateRenderPass();

			/**
			 * @brief Crée les tampons de trame.
			 *
			 * Cette fonction crée les tampons de trame qui sont utilisés pour stocker les informations de rendu associées à chaque image de la chaîne de swaps.
			 * Chaque tampon de trame est associé à une image de la chaîne de swaps et est configuré avec les vues d'image et les profondeurs appropriées pour le rendu.
			 *
			 * @throws std::runtime_error si la création d'un tampon de trame échoue.
			 */
			void CreateFrameBuffers();

			/**
			 * @brief Crée les objets de synchronisation.
			 *
			 * Cette fonction crée les objets de synchronisation utilisés pour synchroniser l'exécution des opérations de rendu avec la présentation des images.
			 * Ces objets comprennent des sémaphores pour la synchronisation entre les différentes étapes du pipeline graphique, ainsi que des clôtures de barrière pour la synchronisation entre le CPU et le GPU.
			 *
			 * @throws std::runtime_error si la création d'un objet de synchronisation échoue.
			 */
			void CreateSyncObjects();

			// Helper functions

			/**
			 * @brief Choisissez le format de surface de la chaîne de swaps.
			 *
			 * Cette fonction choisit le format de surface de la chaîne de swaps en parcourant les formats disponibles et en recherchant un format spécifique (B8G8R8A8_SRGB) avec un espace colorimétrique compatible (VK_COLOR_SPACE_SRGB_NONLINEAR_KHR).
			 * Si un tel format est trouvé, il est renvoyé. Sinon, le premier format disponible est renvoyé par défaut.
			 *
			 * @param _availableFormats Les formats de surface disponibles.
			 * @return Le format de surface choisi.
			 */
			static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(
				const std::vector<vk::SurfaceFormatKHR>& _availableFormats);

			/**
			 * @brief Choisissez le mode de présentation de la chaîne de swaps.
			 *
			 * Cette fonction choisit le mode de présentation de la chaîne de swaps en parcourant les modes de présentation disponibles et en recherchant un mode spécifique (VK_PRESENT_MODE_IMMEDIATE_KHR) qui offre une présentation immédiate sans attente de synchronisation verticale (V-Sync).
			 * Si un tel mode est trouvé, il est renvoyé. Sinon, le mode de présentation FIFO (VK_PRESENT_MODE_FIFO_KHR) avec synchronisation verticale est renvoyé par défaut.
			 *
			 * @param _availablePresentModes Les modes de présentation disponibles.
			 * @return Le mode de présentation choisi.
			 */
			static vk::PresentModeKHR ChooseSwapPresentMode(
				const std::vector<vk::PresentModeKHR>& _availablePresentModes);

			/**
			 * @brief Choisissez l'étendue de la chaîne de swaps.
			 *
			 * Cette fonction choisit l'étendue de la chaîne de swaps en se basant sur les capacités de la surface de rendu.
			 * Si l'étendue actuelle est déjà définie dans les capacités de la surface, elle est retournée directement.
			 * Sinon, une étendue appropriée est calculée en fonction de la taille de la fenêtre et des contraintes minimales et maximales spécifiées dans les capacités de la surface.
			 *
			 * @param _capabilities Les capacités de la surface de rendu.
			 * @return L'étendue de la chaîne de swaps choisie.
			 */
			vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& _capabilities) const;

			vk::Format   swapChainImageFormat; /**< Format des images de la chaîne d'échanges. */
			vk::Format   swapChainDepthFormat; /**< Format de profondeur de la chaîne d'échanges. */
			vk::Extent2D swapChainExtent;      /**< Étendue de la chaîne d'échanges. */

			std::vector<vk::Framebuffer> swapChainFramebuffers;
			/**< Tableau des tampons de trame de la chaîne d'échanges. */
			vk::RenderPass renderPass; /**< Passe de rendu de la chaîne d'échanges. */

			std::vector<vk::Image> depthImages; /**< Tableau des images de profondeur. */
			std::vector<vk::DeviceMemory> depthImageMemorys; /**< Tableau de la mémoire des images de profondeur. */
			std::vector<vk::ImageView> depthImageViews; /**< Tableau des vues des images de profondeur. */
			std::vector<vk::Image> swapChainImages; /**< Tableau des images de la chaîne d'échanges. */
			std::vector<vk::ImageView> swapChainImageViews; /**< Tableau des vues des images de la chaîne d'échanges. */

			LveDevice&   lveDevice;    /**< Référence à l'objet LveDevice associé. */
			vk::Extent2D windowExtent; /**< Étendue de la fenêtre. */

			vk::SwapchainKHR              swapChain;    /**< Chaîne d'échanges Vulkan. */
			std::shared_ptr<LveSwapChain> oldSwapChain; /**< Ancienne chaîne d'échanges. */

			std::vector<vk::Semaphore> imageAvailableSemaphores; /**< Sémaphores disponibles pour les images. */
			std::vector<vk::Semaphore> renderFinishedSemaphores; /**< Sémaphores indiquant la fin du rendu. */
			std::vector<vk::Fence>     inFlightFences; /**< Barrières d'attente pour la synchronisation. */
			std::vector<vk::Fence>     imagesInFlight; /**< Barrières pour les images en cours de traitement. */
			size_t                     currentFrame = 0; /**< Indice du cadre actuel. */
	};
} // namespace lve
