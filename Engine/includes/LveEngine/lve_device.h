#pragma once

#include "LveEngine/lve_window.h"

#include <vulkan/vulkan.hpp> // Inclure les en-têtes Vulkan C++

// std lib headers
#include <string>
#include <vector>

class WindowModule;

namespace lve
{
	// Structure pour stocker les détails de support de la chaîne d'échange
	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR        capabilities; // Capacités de la surface Vulkan
		std::vector<vk::SurfaceFormatKHR> formats;      // Formats de surface Vulkan pris en charge
		std::vector<vk::PresentModeKHR>   presentModes; // Modes de présentation Vulkan pris en charge
	};

	// Structure pour stocker les indices des files d'attente requises
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily; // Indice de la famille de files d'attente graphiques
		uint32_t presentFamily;  // Indice de la famille de files d'attente de présentation
		bool     graphicsFamilyHasValue = false;
		// Indique si l'indice de la famille de files d'attente graphiques est défini
		bool presentFamilyHasValue = false;
		// Indique si l'indice de la famille de files d'attente de présentation est défini

		// Vérifie si les indices des files d'attente requis sont complets
		bool IsComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	/**
	 * @brief Classe représentant un périphérique Vulkan.
	 */
	class LveDevice
	{
		public:
			#ifdef NDEBUG
        const bool enableValidationLayers = false;
			#else
			const bool enableValidationLayers = true;
			#endif

			/**
			* @brief Constructeur de la classe LveDevice.
			*
			* Initialise un périphérique Vulkan en prenant une référence à une fenêtre Vulkan LveWindow en tant que paramètre.
			*
			* @param _windowModule
			*/
			explicit LveDevice(WindowModule* _windowModule);

			/**
			 * @brief Destructeur de la classe LveDevice.
			 *
			 * Nettoie les ressources associées au périphérique Vulkan lorsqu'il est détruit.
			 */
			~LveDevice();

			// Not copyable or movable
			/**
			* @brief Constructeur de copie supprimé.
			*
			* Empêche la création d'une nouvelle instance de LveDevice en copiant une instance existante.
			*/
			LveDevice(const LveDevice&) = delete;

			/**
			* @brief Opérateur d'affectation de copie supprimé.
			*
			* Empêche la copie des membres d'une instance de LveDevice vers une autre instance existante.
			*/
			LveDevice& operator=(const LveDevice&) = delete;

			/**
			* @brief Constructeur de déplacement supprimé.
			*
			* Empêche la création d'une nouvelle instance de LveDevice en déplaçant une instance existante.
			*/
			LveDevice(LveDevice&&) = delete;

			/**
			* @brief Opérateur d'affectation de déplacement supprimé.
			*
			* Empêche le déplacement des membres d'une instance de LveDevice vers une autre instance existante.
			*/
			LveDevice& operator=(LveDevice&&) = delete;

			/**
			* @brief Récupère le pool de commandes Vulkan associé au périphérique.
			*
			* @return Le pool de commandes Vulkan associé au périphérique.
			*/
			vk::CommandPool GetCommandPool() const { return commandPool; }

			/**
			 * @brief Récupère l'objet de périphérique Vulkan.
			 *
			 * @return L'objet de périphérique Vulkan.
			 */
			vk::Device Device() const { return device_; }

			// MAURAD
			vk::Instance       GetInstance() const { return instance; }
			vk::PhysicalDevice GetPhysicalDevice() const { return physicalDevice; }

			/**
			 * @brief Récupère la surface Vulkan associée au périphérique.
			 *
			 * @return La surface Vulkan associée au périphérique.
			 */
			vk::SurfaceKHR Surface() const { return surface_; }

			/**
			 * @brief Récupère la file de commandes graphiques du périphérique.
			 *
			 * @return La file de commandes graphiques du périphérique.
			 */
			vk::Queue GraphicsQueue() const { return graphicsQueue_; }

			/**
			 * @brief Récupère la file de commandes de présentation du périphérique.
			 *
			 * @return La file de commandes de présentation du périphérique.
			 */
			vk::Queue PresentQueue() const { return presentQueue_; }


			/**
			 * @brief Récupère les détails de prise en charge de la chaîne de balisage pour le périphérique.
			 *
			 * @return Les détails de prise en charge de la chaîne de balisage pour le périphérique.
			 */
			SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(physicalDevice); }

			/**
			 * @brief Trouve un type de mémoire approprié pour une utilisation spécifiée.
			 *
			 * @param _typeFilter Le type de filtre de mémoire.
			 * @param _properties Les propriétés de la mémoire.
			 * @return Le type de mémoire approprié.
			 */
			uint32_t FindMemoryType(uint32_t _typeFilter, vk::MemoryPropertyFlags _properties) const;

			/**
			 * @brief Trouve les familles de files de commandes physiques disponibles sur le périphérique.
			 *
			 * @return Les familles de files de commandes physiques disponibles sur le périphérique.
			 */
			QueueFamilyIndices FindPhysicalQueueFamilies() const { return FindQueueFamilies(physicalDevice); }

			/**
			 * @brief Trouve un format d'image supporté par le périphérique.
			 *
			 * @param _candidates Les formats d'image candidats.
			 * @param _tiling L'inclinaison de l'image.
			 * @param _features Les fonctionnalités du format de l'image.
			 * @return Le format d'image supporté par le périphérique.
			 */
			vk::Format FindSupportedFormat(
				const std::vector<vk::Format>& _candidates, vk::ImageTiling _tiling,
				vk::FormatFeatureFlags         _features) const;


			// Buffer Helper Functions
			/**
			 * @brief Crée un tampon Vulkan avec les paramètres spécifiés.
			 *
			 * @param _size La taille du tampon.
			 * @param _usage Les indicateurs d'utilisation du tampon.
			 * @param _properties Les propriétés de la mémoire du tampon.
			 * @param _buffer Référence à l'objet tampon Vulkan créé.
			 * @param _bufferMemory Référence à l'objet mémoire du tampon Vulkan créé.
			 */
			void CreateBuffer(
				vk::DeviceSize          _size,
				vk::BufferUsageFlags    _usage,
				vk::MemoryPropertyFlags _properties,
				vk::Buffer&             _buffer,
				vk::DeviceMemory&       _bufferMemory) const;

			/**
			 * @brief Démarre une séquence de commandes Vulkan temporaires.
			 *
			 * Cette fonction alloue un tampon de commandes Vulkan et le démarre pour une utilisation unique.
			 *
			 * @return Le tampon de commandes Vulkan alloué.
			 */
			vk::CommandBuffer BeginSingleTimeCommands() const;

			/**
			 * @brief Termine une séquence de commandes Vulkan temporaires.
			 *
			 * Cette fonction termine l'exécution d'une séquence de commandes Vulkan temporaires et libère les ressources associées.
			 *
			 * @param _commandBuffer Le tampon de commandes Vulkan à terminer.
			 */
			void EndSingleTimeCommands(vk::CommandBuffer _commandBuffer) const;

			/**
			 * @brief Copie les données d'un tampon source vers un tampon de destination.
			 *
			 * @param _srcBuffer Le tampon source à partir duquel copier les données.
			 * @param _dstBuffer Le tampon de destination où copier les données.
			 * @param _size La taille des données à copier.
			 */
			void CopyBuffer(vk::Buffer _srcBuffer, vk::Buffer _dstBuffer, vk::DeviceSize _size) const;

			/**
			 * @brief Copie les données d'un tampon vers une image Vulkan.
			 *
			 * Cette fonction copie les données d'un tampon Vulkan vers une image Vulkan en spécifiant la largeur, la hauteur et le nombre de couches de l'image.
			 *
			 * @param _buffer Le tampon contenant les données à copier.
			 * @param _image L'image Vulkan de destination.
			 * @param _width La largeur de l'image.
			 * @param _height La hauteur de l'image.
			 * @param _layerCount Le nombre de couches de l'image.
			 */
			void CopyBufferToImage(
				vk::Buffer _buffer, vk::Image _image, uint32_t _width, uint32_t _height, uint32_t _layerCount) const;

			/**
			 * @brief Crée une image Vulkan avec les informations fournies.
			 *
			 * Cette fonction crée une image Vulkan en utilisant les informations spécifiées dans la structure VkImageCreateInfo.
			 *
			 * @param _imageInfo Les informations de création de l'image.
			 * @param _properties Les propriétés de la mémoire de l'image.
			 * @param _image Référence à l'objet image Vulkan créé.
			 * @param _imageMemory Référence à l'objet mémoire de l'image Vulkan créé.
			 */
			void CreateImageWithInfo(
				const vk::ImageCreateInfo& _imageInfo,
				vk::MemoryPropertyFlags    _properties,
				vk::Image&                 _image,
				vk::DeviceMemory&          _imageMemory) const;

			/**
			 * @brief Propriétés du périphérique physique Vulkan associé.
			 *
			 * Cette structure contient les propriétés du périphérique physique Vulkan associé.
			 */
			vk::PhysicalDeviceProperties properties;

			/**
		  * @brief Crée une instance Vulkan.
		  *
		  * Cette fonction initialise une instance Vulkan, qui représente la connexion entre l'application et l'API Vulkan.
		  */
			vk::Instance CreateInstance();

			/**
			* @brief Configure le gestionnaire de débogage.
			*
			* Cette fonction configure un gestionnaire de débogage Vulkan pour recevoir les messages de validation et de débogage de l'API Vulkan.
			*/
			vk::DebugUtilsMessengerEXT SetupDebugMessenger();
			vk::DebugUtilsMessengerEXT GetDebugMessenger() const { return debugMessenger; }

		private:
			/**
			 * @brief Crée une surface Vulkan.
			 *
			 * Cette fonction crée une surface Vulkan à partir de la fenêtre Vulkan associée.
			 */
			void CreateSurface();

			/**
			 * @brief Sélectionne le périphérique physique Vulkan approprié.
			 *
			 * Cette fonction sélectionne le périphérique physique Vulkan approprié parmi ceux disponibles sur le système.
			 */
			void PickPhysicalDevice();

			/**
			 * @brief Crée le périphérique logique Vulkan.
			 *
			 * Cette fonction crée le périphérique logique Vulkan qui sera utilisé pour interagir avec le matériel graphique.
			 */
			void CreateLogicalDevice();

			/**
			 * @brief Crée le pool de commandes Vulkan.
			 *
			 * Cette fonction crée un pool de commandes Vulkan, qui est utilisé pour allouer des tampons de commandes pour le périphérique logique.
			 */
			void CreateCommandPool();


			// helper functions
			/**
			 * @brief Vérifie si le périphérique physique Vulkan spécifié convient aux besoins de l'application.
			 *
			 * Cette fonction examine les propriétés et les capacités du périphérique pour déterminer s'il convient à l'application.
			 *
			 * @param _device Le périphérique physique Vulkan à évaluer.
			 * @return true si le périphérique convient, sinon false.
			 */
			bool IsDeviceSuitable(vk::PhysicalDevice _device);

			/**
			 * @brief Récupère les extensions Vulkan requises par l'application.
			 *
			 * Cette fonction retourne un vecteur contenant les noms des extensions Vulkan nécessaires à l'application.
			 *
			 * @return Un vecteur de pointeurs de chaînes C représentant les noms des extensions requises.
			 */
			std::vector<const char*> GetRequiredExtensions() const;

			/**
			 * @brief Vérifie la prise en charge des couches de validation Vulkan.
			 *
			 * Cette fonction vérifie si les couches de validation Vulkan spécifiées sont prises en charge par l'instance Vulkan.
			 *
			 * @return true si les couches de validation sont prises en charge, sinon false.
			 */
			bool CheckValidationLayerSupport() const;

			/**
			 * @brief Recherche les familles de files de commandes supportées par le périphérique physique Vulkan spécifié.
			 *
			 * Cette fonction identifie les indices des familles de files de commandes graphiques et de présentation supportées par le périphérique.
			 *
			 * @param _device Le périphérique physique Vulkan à évaluer.
			 * @return Une structure contenant les indices des familles de files de commandes supportées.
			 */
			QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice _device) const;

			/**
			 * @brief Remplit une structure de données pour la création d'un gestionnaire de débogage Vulkan.
			 *
			 * Cette fonction initialise une structure de données pour la création d'un gestionnaire de débogage Vulkan avec les informations nécessaires.
			 *
			 * @param _createInfo La structure de données à remplir pour la création du gestionnaire de débogage.
			 */
			void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& _createInfo);

			/**
			 * @brief Vérifie si l'instance Vulkan possède les extensions requises par GLFW.
			 *
			 * Cette fonction vérifie si l'instance Vulkan possède les extensions requises par GLFW pour créer une surface Vulkan.
			 */
			void HasGlfwRequiredInstanceExtensions() const;

			/**
			 * @brief Vérifie si le périphérique physique Vulkan spécifié prend en charge les extensions de périphérique nécessaires.
			 *
			 * Cette fonction vérifie si le périphérique physique Vulkan spécifié prend en charge les extensions de périphérique requises par l'application.
			 *
			 * @param _device Le périphérique physique Vulkan à évaluer.
			 * @return true si les extensions de périphérique sont prises en charge, sinon false.
			 */
			bool CheckDeviceExtensionSupport(vk::PhysicalDevice _device) const;

			/**
			 * @brief Interroge le périphérique physique Vulkan spécifié pour obtenir les détails de support de la chaîne d'échange.
			 *
			 * Cette fonction interroge le périphérique physique Vulkan spécifié pour obtenir les détails de support de la chaîne d'échange, tels que les capacités, les formats de surface supportés et les modes de présentation supportés.
			 *
			 * @param _device Le périphérique physique Vulkan à interroger.
			 * @return Une structure contenant les détails de support de la chaîne d'échange.
			 */
			SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice _device) const;


			vk::Instance instance; /**< L'instance Vulkan utilisée par l'application. */

			vk::DebugUtilsMessengerEXT debugMessenger;
			/**< Le gestionnaire de débogage Vulkan pour la gestion des messages de validation et de débogage. */

			vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
			/**< Le périphérique physique Vulkan utilisé par l'application, initialisé à VK_NULL_HANDLE par défaut. */

			WindowModule* windowModule = nullptr; /**< Une référence à la fenêtre Vulkan utilisée par l'application. */

			vk::CommandPool commandPool;
			/**< Le pool de commandes Vulkan utilisé pour allouer les tampons de commandes. */

			vk::Device device_; /**< Le périphérique logique Vulkan utilisé par l'application. */

			vk::SurfaceKHR surface_; /**< La surface Vulkan associée à la fenêtre Vulkan utilisée par l'application. */

			vk::Queue graphicsQueue_;
			/**< La file de commandes pour les opérations graphiques sur le périphérique logique. */

			vk::Queue presentQueue_;
			/**< La file de commandes pour les opérations de présentation sur le périphérique logique. */

			const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
			/**< Les couches de validation Vulkan activées par défaut. */

			const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};
			/**< Les extensions de périphérique Vulkan utilisées par l'application, avec l'extension de la chaîne d'échange Vulkan activée par défaut. */
	};
} // namespace lve
