#include "LveEngine/lve_device.h"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

#include "Modules/WindowModule.h"

namespace lve
{
	// local callback functions
	/**
	 * @brief Fonction de rappel de débogage pour la gestion des messages de validation Vulkan.
	 *
	 * Cette fonction est appelée par les couches de validation Vulkan pour gérer les messages de débogage.
	 * Elle affiche les messages de débogage dans la console d'erreur standard.
	 *
	 * @param messageSeverity La sévérité du message de débogage.
	 * @param messageType Le type de message de débogage.
	 * @param pCallbackData Les données de rappel du messager de débogage.
	 * @param pUserData Des données utilisateur optionnelles.
	 * @return VK_FALSE indiquant que le traitement du message est terminé, et aucune action supplémentaire n'est nécessaire.
	 */
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
		vk::DebugUtilsMessageTypeFlagsEXT             messageType,
		const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*                                         pUserData)
	{
		// Affiche le message de débogage dans la console d'erreur standard
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		// Indique que le traitement du message est terminé, et qu'aucune action supplémentaire n'est nécessaire
		return VK_FALSE;
	}


	/**
	 * @brief Crée un gestionnaire de messager de débogage Vulkan en utilisant l'extension VK_EXT_debug_utils.
	 *
	 * Cette fonction crée un gestionnaire de messager de débogage Vulkan en utilisant l'extension VK_EXT_debug_utils.
	 * Elle utilise la fonction vkCreateDebugUtilsMessengerEXT, si elle est disponible.
	 *
	 * @param instance L'instance Vulkan.
	 * @param pCreateInfo Les informations de création du gestionnaire de messager de débogage.
	 * @param pAllocator L'allocation de rappels, permettant la personnalisation de la gestion de la mémoire.
	 * @param pDebugMessenger Un pointeur vers l'objet de gestionnaire de messager de débogage à créer.
	 * @return VK_SUCCESS si le gestionnaire de messager de débogage a été créé avec succès, VK_ERROR_EXTENSION_NOT_PRESENT si l'extension n'est pas prise en charge.
	 */
	vk::Result CreateDebugUtilsMessengerEXT(vk::Instance                                instance,
	                                        const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	                                        const vk::AllocationCallbacks*              pAllocator,
	                                        vk::DebugUtilsMessengerEXT*                 pDebugMessenger)
	{
		const PFN_vkVoidFunction function = instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
		const auto messenger_ext = reinterpret_cast<PFN_vkGetInstanceProcAddr>(function);
		const vk::DispatchLoaderDynamic dispatch(instance, messenger_ext);
		const vk::Result result = instance.createDebugUtilsMessengerEXT(pCreateInfo, pAllocator, pDebugMessenger,
		                                                                dispatch);
		return result;
	}


	/**
	 * @brief Détruit un gestionnaire de messager de débogage Vulkan créé avec l'extension VK_EXT_debug_utils.
	 *
	 * Cette fonction détruit un gestionnaire de messager de débogage Vulkan créé avec l'extension VK_EXT_debug_utils.
	 * Elle utilise la fonction vkDestroyDebugUtilsMessengerEXT, si elle est disponible.
	 *
	 * @param _instance L'instance Vulkan.
	 * @param _debugMessenger L'objet de gestionnaire de messager de débogage à détruire.
	 * @param _pAllocator L'allocation de rappels, permettant la personnalisation de la gestion de la mémoire.
	 */
	void DestroyDebugUtilsMessengerEXT(
		const vk::Instance               _instance,
		const vk::DebugUtilsMessengerEXT _debugMessenger,
		const vk::AllocationCallbacks*   _pAllocator)
	{
		// Récupère le pointeur de fonction pour vkDestroyDebugUtilsMessengerEXT

		// Vérifie si la fonction est disponible
		if (const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT")); func != nullptr)
			// Appelle la fonction vkDestroyDebugUtilsMessengerEXT pour détruire le gestionnaire de messager de débogage
			func(_instance, static_cast<VkDebugUtilsMessengerEXT>(_debugMessenger),
			     reinterpret_cast<const VkAllocationCallbacks*>(_pAllocator));
	}


	// class member functions

	// Constructeur de la classe LveDevice
	LveDevice::LveDevice(WindowModule* _windowModule) : windowModule{ _windowModule }
	{
		CreateInstance();      // Crée une instance Vulkan
		SetupDebugMessenger(); // Configure le messager de débogage Vulkan
		CreateSurface();       // Crée la surface de la fenêtre Vulkan
		PickPhysicalDevice();  // Sélectionne le périphérique physique adapté
		CreateLogicalDevice(); // Crée le périphérique logique Vulkan
		CreateCommandPool();   // Crée le pool de commandes Vulkan
	}

	// Destructeur de la classe LveDevice
	LveDevice::~LveDevice()
	{
		// Détruit le pool de commandes Vulkan
		device_.destroyCommandPool(commandPool, nullptr);

		// Détruit le périphérique logique Vulkan
		device_.destroy(nullptr);

		// Si les couches de validation sont activées, détruit le messager de débogage Vulkan
		if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		// Détruit la surface Vulkan associée à la fenêtre
		instance.destroySurfaceKHR(surface_, nullptr);

		// Détruit l'instance Vulkan
		instance.destroy(nullptr);
	}


	/**
	 * @brief Crée une instance Vulkan pour l'application.
	 *
	 * Cette fonction configure et crée une instance Vulkan, qui est la première étape dans l'utilisation de l'API Vulkan.
	 * Elle configure les informations d'application, les extensions requises, les couches de validation, et crée l'instance Vulkan correspondante.
	 *
	 * @throws std::runtime_error si la création de l'instance échoue ou si les couches de validation sont activées mais non disponibles.
	 */
	vk::Instance LveDevice::CreateInstance()
	{
		// Vérifie si les couches de validation sont activées et si elles sont supportées
		if (enableValidationLayers && !CheckValidationLayerSupport())
			throw std::runtime_error(
				"validation layers requested, but not available!");

		// Configuration des informations d'application pour l'instance Vulkan
		constexpr vk::ApplicationInfo app_info(
			"An-Gine App",
			VK_MAKE_VERSION(1, 0, 0),
			"An-Gine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_0
		);

		// Obtention des extensions Vulkan requises pour l'application
		const auto extensions = GetRequiredExtensions();

		// Configuration de la création de l'instance Vulkan
		vk::InstanceCreateInfo create_info(
			{},
			&app_info, // Initialisation directe avec l'adresse de appInfo
			0, nullptr, // Aucune couche de validation
			static_cast<uint32_t>(extensions.size()), extensions.data() // Extensions activées
		);

		// Configuration du messager de débogage Vulkan si les couches de validation sont activées
		vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
		if (enableValidationLayers)
		{
			create_info.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
			create_info.ppEnabledLayerNames = validationLayers.data();

			PopulateDebugMessengerCreateInfo(debug_create_info);
			create_info.pNext = &debug_create_info;
		}
		else
		{
			create_info.enabledLayerCount = 0;
			create_info.pNext             = nullptr;
		}
		instance = createInstance(create_info);
		// Vérification des extensions GLFW requises pour l'instance Vulkan
		HasGlfwRequiredInstanceExtensions();

		return instance;
	}

	/**
	 * @brief Sélectionne le périphérique physique adapté pour l'application.
	 *
	 * Cette fonction récupère la liste des périphériques physiques Vulkan disponibles sur le système,
	 * puis elle itère à travers chaque périphérique pour trouver celui qui convient le mieux à l'application.
	 * Le périphérique sélectionné est stocké dans la variable physicalDevice.
	 *
	 * @throws Une exception si aucun périphérique physique approprié n'est trouvé.
	 */
	void LveDevice::PickPhysicalDevice()
	{
		// Récupère la liste des périphériques physiques Vulkan disponibles sur le système
		std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

		// Vérifie si des périphériques Vulkan ont été trouvés
		if (devices.empty()) throw std::runtime_error("failed to find GPUs with Vulkan support!");

		// Affiche le nombre de périphériques Vulkan trouvés
		std::cout << "Device count: " << devices.size() << std::endl;

		// Parcourt chaque périphérique pour trouver celui qui convient le mieux à l'application
		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}

		// Vérifie si un périphérique adapté a été trouvé
		if (!physicalDevice) throw std::runtime_error("failed to find a suitable GPU!");
		// Récupère les propriétés du périphérique sélectionné et les affiche
		vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
		std::cout << "Physical device: " << properties.deviceName << std::endl;
	}


	/**
	* @brief Crée un périphérique logique Vulkan.
	*
	* Cette fonction crée un périphérique logique Vulkan à partir du périphérique physique sélectionné.
	* Elle configure les files de commandes pour les opérations graphiques et de présentation, ainsi que les fonctionnalités du périphérique.
	*
	* @throws Une exception si la création du périphérique logique échoue.
	*/
	void LveDevice::CreateLogicalDevice()
	{
		// Recherche des familles de files de commandes pour le périphérique physique sélectionné
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

		// Configuration des files de commandes pour les opérations graphiques et de présentation
		std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t>                     unique_queue_families = {indices.graphicsFamily, indices.presentFamily};
		float                                  queue_priority        = 1.0f;
		for (uint32_t queue_family : unique_queue_families)
		{
			vk::DeviceQueueCreateInfo queue_create_info(
				{},
				queue_family,
				1,
				&queue_priority
			);
			queue_create_infos.push_back(queue_create_info);
		}

		// Configuration des fonctionnalités du périphérique
		vk::PhysicalDeviceFeatures device_features;
		device_features.samplerAnisotropy = VK_TRUE;

		// Configuration de la création du périphérique logique
		vk::DeviceCreateInfo create_info;
		create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pQueueCreateInfos       = queue_create_infos.data();
		create_info.pEnabledFeatures        = &device_features;
		create_info.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
		create_info.ppEnabledExtensionNames = deviceExtensions.data();

		// Configuration des couches de validation si elles sont activées
		if (enableValidationLayers)
		{
			create_info.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
			create_info.ppEnabledLayerNames = validationLayers.data();
		}

		// Création du périphérique logique Vulkan
		device_ = physicalDevice.createDevice(create_info);

		// Obtention des files de commandes graphiques et de présentation du périphérique logique
		graphicsQueue_ = device_.getQueue(indices.graphicsFamily, 0);
		presentQueue_  = device_.getQueue(indices.presentFamily, 0);
	}


	/**
	 * @brief Crée un pool de commandes Vulkan.
	 *
	 * Cette fonction crée un pool de commandes Vulkan pour le périphérique logique actuel.
	 * Le pool de commandes est associé à la famille de files de commandes graphiques trouvée pour ce périphérique.
	 * Les tampons de commandes créés à partir de ce pool auront les drapeaux VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	 * et VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, ce qui permet des opérations transitoires et de réinitialiser
	 * les tampons de commandes.
	 *
	 * @throw std::runtime_error si la création du pool de commandes échoue.
	 */
	void LveDevice::CreateCommandPool()
	{
		// Recherche les indices des familles de files de commandes supportées par le périphérique physique actuel.
		const QueueFamilyIndices queue_family_indices = FindPhysicalQueueFamilies();

		// Initialise la structure d'informations du pool de commandes Vulkan.
		const vk::CommandPoolCreateInfo pool_info(
			vk::CommandPoolCreateFlags(
				vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
			queue_family_indices.graphicsFamily
		);

		// Crée le pool de commandes Vulkan en utilisant les informations fournies.
		try
		{
			commandPool = device_.createCommandPool(pool_info);
		}
		catch (const vk::SystemError& e)
		{
			// Lance une exception si la création du pool de commandes échoue.
			throw std::runtime_error("failed to create command pool!");
		}
	}


	/**
	 * @brief Crée une surface Vulkan associée à la fenêtre de l'application.
	 *
	 * Cette fonction utilise la fenêtre de l'application pour créer une surface Vulkan, qui est nécessaire
	 * pour interagir avec la fenêtre et effectuer le rendu graphique.
	 *
	 * @throw std::runtime_error si la création de la surface échoue.
	 */
	void LveDevice::CreateSurface() { windowModule->CreateWindowSurface(instance, &surface_); }

	/**
	 * @brief Vérifie si le périphérique physique Vulkan spécifié convient aux besoins de l'application.
	 *
	 * Cette fonction examine les propriétés et les capacités du périphérique pour déterminer s'il convient à l'application.
	 *
	 * @param _device
	 * @param device Le périphérique physique Vulkan à évaluer.
	 * @return true si le périphérique convient, sinon false.
	 */
	bool LveDevice::IsDeviceSuitable(vk::PhysicalDevice _device)
	{
		// Recherche des indices des familles de files de commandes supportées par le périphérique physique.
		QueueFamilyIndices indices = FindQueueFamilies(_device);

		// Vérification de la pris000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e en charge des extensions de périphérique nécessaires.
		bool extensions_supported = CheckDeviceExtensionSupport(_device);

		// Vérification de l'adéquation de la chaîne d'échange.
		bool swapChainAdequate = false;
		if (extensions_supported)
		{
			// Interrogation du périphérique pour obtenir les détails de support de la chaîne d'échange.
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		// Récupération des fonctionnalités supportées par le périphérique.
		vk::PhysicalDeviceFeatures supportedFeatures = _device.getFeatures();

		// Vérification des fonctionnalités requises.
		return indices.IsComplete() && extensions_supported && swapChainAdequate &&
		       supportedFeatures.samplerAnisotropy;
	}

	/**
	 * @brief Remplit une structure de données pour la création d'un gestionnaire de débogage Vulkan.
	 *
	 * Cette fonction initialise une structure de données pour la création d'un gestionnaire de débogage Vulkan avec les informations nécessaires.
	 *
	 * @param createInfo La structure de données à remplir pour la création du gestionnaire de débogage.
	 */
	void LveDevice::PopulateDebugMessengerCreateInfo(
		vk::DebugUtilsMessengerCreateInfoEXT& _createInfo)
	{
		// Spécifie le type de la structure.
		_createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;

		// Spécifie les niveaux de sévérité des messages à intercepter.
		_createInfo.setMessageSeverity(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

		// Spécifie les types de messages à intercepter.
		_createInfo.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

		// Spécifie la fonction de rappel à appeler pour chaque message de débogage.
		_createInfo.setPfnUserCallback(reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback));

		// Spécifie des données utilisateur facultatives.
		_createInfo.setPUserData(nullptr); // Optionnel
	}


	/**
	 * @brief Configure le gestionnaire de débogage Vulkan.
	 *
	 * Cette fonction configure le gestionnaire de débogage Vulkan si les couches de validation sont activées.
	 *
	 * Elle utilise la fonction populateDebugMessengerCreateInfo() pour remplir les informations nécessaires pour
	 * la création du gestionnaire de débogage.
	 *
	 * @throw std::runtime_error si la configuration du gestionnaire de débogage échoue.
	 */
	vk::DebugUtilsMessengerEXT LveDevice::SetupDebugMessenger()
	{
		// Vérifie si les couches de validation sont activées.
		if (!enableValidationLayers) return VK_NULL_HANDLE;

		// Initialise une structure pour la création du gestionnaire de débogage.
		vk::DebugUtilsMessengerCreateInfoEXT create_info;
		PopulateDebugMessengerCreateInfo(create_info);

		if (const auto dispatcher = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
			instance.createDebugUtilsMessengerEXT(&create_info, nullptr, &debugMessenger, dispatcher) !=
			vk::Result::eSuccess)
			throw std::runtime_error("failed to set up debug messenger!");

		return debugMessenger;
	}


	/**
	 * @brief Vérifie la prise en charge des couches de validation Vulkan.
	 *
	 * Cette fonction vérifie si toutes les couches de validation spécifiées dans la variable `validationLayers`
	 * sont prises en charge par Vulkan sur le système.
	 *
	 * @return true si toutes les couches de validation sont prises en charge, sinon false.
	 */
	bool LveDevice::CheckValidationLayerSupport() const
	{
		// Récupère les propriétés de toutes les couches de validation disponibles.
		const auto available_layers = vk::enumerateInstanceLayerProperties();

		// Parcourt toutes les couches de validation requises.
		for (const char* layer_name : validationLayers)
		{
			bool layer_found = false;

			// Vérifie si la couche de validation requise est présente parmi les couches disponibles.
			for (const auto& layer_properties : available_layers)
			{
				if (strcmp(layer_name, layer_properties.layerName) == 0)
				{
					layer_found = true;
					break;
				}
			}

			// Si la couche de validation requise n'est pas trouvée, retourne false.
			if (!layer_found) return false;
		}

		// Si toutes les couches de validation requises sont trouvées, retourne true.
		return true;
	}


	/**
	 * @brief Récupère les extensions Vulkan requises pour l'instance Vulkan.
	 *
	 * Cette fonction récupère les extensions Vulkan requises pour l'instance Vulkan en utilisant la bibliothèque GLFW.
	 * Elle inclut les extensions requises par GLFW et, si les couches de validation sont activées, l'extension de
	 * gestionnaire de débogage Vulkan.
	 *
	 * @return Un vecteur de pointeurs vers les noms des extensions Vulkan requises.
	 */
	std::vector<const char*> LveDevice::GetRequiredExtensions() const
	{
		// Récupère le nombre d'extensions requises par GLFW.
		uint32_t     glfw_extension_count = 0;
		const char** glfw_extensions      = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		// Convertit les noms des extensions GLFW en un vecteur de pointeurs vers les noms des extensions Vulkan.
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		// Ajoute l'extension de gestionnaire de débogage Vulkan si les couches de validation sont activées.
		if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		// Retourne le vecteur contenant les noms des extensions Vulkan requises.
		return extensions;
	}


	/**
	  * @brief Vérifie la disponibilité des extensions Vulkan requises par GLFW.
	  *
	  * Cette fonction vérifie si toutes les extensions Vulkan requises par GLFW sont disponibles sur le système.
	  * Elle récupère d'abord toutes les extensions Vulkan disponibles, puis compare avec les extensions requises
	  * obtenues à partir de la fonction getRequiredExtensions(). Si une extension requise est manquante, une exception
	  * est levée.
	  *
	  * @throw std::runtime_error si une extension requise par GLFW est manquante.
	  */
	void LveDevice::HasGlfwRequiredInstanceExtensions() const
	{
		// Récupère le nombre d'extensions Vulkan disponibles.
		const auto available_extensions = vk::enumerateInstanceExtensionProperties();

		// Affiche les extensions Vulkan disponibles et les stocke dans un ensemble pour une recherche rapide.
		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : available_extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		// Affiche les extensions Vulkan requises par GLFW et vérifie leur disponibilité.
		std::cout << "required extensions:" << std::endl;
		for (const auto required_extensions = GetRequiredExtensions(); const auto& required : required_extensions)
		{
			std::cout << "\t" << required << std::endl;
			if (!available.contains(required))
				// Lance une exception si une extension requise est manquante.
				throw std::runtime_error("Missing required glfw extension");
		}
	}


	/**
	 * @brief Vérifie la prise en charge des extensions de périphérique Vulkan par le périphérique physique spécifié.
	 *
	 * Cette fonction vérifie si toutes les extensions de périphérique Vulkan requises par l'application sont prises en charge
	 * par le périphérique physique Vulkan spécifié.
	 *
	 * @param _device
	 * @param device Le périphérique physique Vulkan à vérifier.
	 * @return true si toutes les extensions de périphérique requises sont prises en charge, sinon false.
	 */
	bool LveDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice _device) const
	{
		// Récupère les propriétés de toutes les extensions de périphérique Vulkan disponibles pour le périphérique spécifié.
		const auto available_extensions = _device.enumerateDeviceExtensionProperties(nullptr);

		// Crée un ensemble contenant toutes les extensions de périphérique requises par l'application.
		std::set<std::string> required_extensions(deviceExtensions.begin(), deviceExtensions.end());

		// Parcourt toutes les extensions de périphérique disponibles.
		for (const auto& extension : available_extensions)
		{
			// Supprime les extensions disponibles de l'ensemble des extensions requises.
			required_extensions.erase(extension.extensionName);
		}

		// Si l'ensemble des extensions requises est vide, cela signifie que toutes les extensions requises sont prises en charge.
		return required_extensions.empty();
	}


	/**
	 * @brief Recherche et retourne les indices des files d'attente de périphériques pour le périphérique physique spécifié.
	 *
	 * Cette fonction recherche les indices des files d'attente de périphériques pour les opérations de rendu graphique et de présentation
	 * pour le périphérique physique Vulkan spécifié.
	 *
	 * @param _device Le périphérique physique Vulkan pour lequel les files d'attente doivent être recherchées.
	 * @return Une structure QueueFamilyIndices contenant les indices des files d'attente pour les opérations de rendu graphique et de présentation.
	 */
	QueueFamilyIndices LveDevice::FindQueueFamilies(const vk::PhysicalDevice _device) const
	{
		// Initialise la structure QueueFamilyIndices pour stocker les indices des files d'attente.
		QueueFamilyIndices indices;

		// Récupère le nombre de familles de files d'attente de périphériques disponibles pour le périphérique spécifié.
		const auto queue_families = _device.getQueueFamilyProperties();

		// Parcourt toutes les familles de files d'attente pour trouver celles qui prennent en charge les opérations de rendu graphique et de présentation.
		int i = 0;
		for (const auto& queue_family : queue_families)
		{
			// Vérifie si la famille de files d'attente prend en charge les opérations de rendu graphique.
			if (queue_family.queueCount > 0 && queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphicsFamily         = i;
				indices.graphicsFamilyHasValue = true;
			}
			// Vérifie si la famille de files d'attente prend en charge la présentation sur la surface associée.
			if (const bool presentSupport = _device.getSurfaceSupportKHR(i, surface_);
				queue_family.queueCount > 0 && presentSupport)
			{
				indices.presentFamily         = i;
				indices.presentFamilyHasValue = true;
			}
			// Si les indices requis ont été trouvés, arrête la recherche.
			if (indices.IsComplete()) break;

			i++;
		}

		// Retourne les indices des files d'attente trouvées.
		return indices;
	}


	/**
	 * @brief Interroge et retourne les détails de prise en charge de la chaîne d'échange de swap (SwapChain) pour le périphérique physique spécifié.
	 *
	 * Cette fonction interroge le périphérique physique Vulkan spécifié pour obtenir les détails de prise en charge de la chaîne d'échange de swap, tels que
	 * les capacités de surface, les formats de surface pris en charge et les modes de présentation pris en charge.
	 *
	 * @param _device Le périphérique physique Vulkan pour lequel les détails de prise en charge de la chaîne d'échange de swap doivent être interrogés.
	 * @return Une structure SwapChainSupportDetails contenant les détails de prise en charge de la chaîne d'échange de swap.
	 */
	SwapChainSupportDetails LveDevice::QuerySwapChainSupport(const vk::PhysicalDevice _device) const
	{
		// Initialise la structure SwapChainSupportDetails pour stocker les détails de prise en charge de la chaîne d'échange de swap.
		SwapChainSupportDetails details;

		// Interroge le périphérique physique Vulkan pour obtenir les capacités de la surface associée.
		details.capabilities = _device.getSurfaceCapabilitiesKHR(surface_);

		// Récupère les formats de surface pris en charge par le périphérique.
		details.formats = _device.getSurfaceFormatsKHR(surface_);

		// Récupère les modes de présentation pris en charge par le périphérique.
		details.presentModes = _device.getSurfacePresentModesKHR(surface_);

		// Retourne les détails de prise en charge de la chaîne d'échange de swap.
		return details;
	}


	/**
	 * @brief Recherche un format d'image supporté parmi une liste de formats candidats.
	 *
	 * Cette fonction recherche un format d'image supporté parmi une liste de formats candidats,
	 * en tenant compte du mode de tiling spécifié (linéaire ou optimal) et des fonctionnalités requises.
	 *
	 * @param _candidates La liste des formats candidats à vérifier.
	 * @param _tiling Le mode de tiling de l'image (VK_IMAGE_TILING_LINEAR ou VK_IMAGE_TILING_OPTIMAL).
	 * @param _features Les fonctionnalités requises pour le format.
	 * @return Le format d'image supporté trouvé.
	 * @throws std::runtime_error si aucun format d'image supporté n'est trouvé.
	 */
	vk::Format LveDevice::FindSupportedFormat(
		const std::vector<vk::Format>& _candidates, const vk::ImageTiling _tiling,
		const vk::FormatFeatureFlags   _features) const
	{
		// Parcourt chaque format d'image candidat
		for (const vk::Format format : _candidates)
		{
			vk::FormatProperties props;
			// Obtient les propriétés du format
			props = physicalDevice.getFormatProperties(format);

			// Vérifie si le format est adapté au mode de tiling spécifié (linéaire ou optimal)
			if (_tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & _features) == _features)
				// Si le format est adapté au mode linéaire et prend en charge toutes les fonctionnalités requises, le retourne
				return format;
			if (_tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & _features) == _features)
				// Si le format est adapté au mode optimal et prend en charge toutes les fonctionnalités requises, le retourne
				return format;
		}
		// Si aucun format d'image trouvé ne répond aux critères spécifiés, lance une exception
		throw std::runtime_error("failed to find supported format!");
	}


	/**
	 * @brief Recherche un type de mémoire adapté pour les allocations de mémoire graphique.
	 *
	 * Cette fonction recherche un type de mémoire adapté pour les allocations de mémoire graphique,
	 * en tenant compte du filtre de type spécifié et des propriétés de mémoire requises.
	 *
	 * @param _typeFilter Le filtre de type de mémoire spécifié.
	 * @param _properties Les propriétés de mémoire requises.
	 * @return L'index du type de mémoire adapté trouvé.
	 * @throws std::runtime_error si aucun type de mémoire adapté n'est trouvé.
	 */
	uint32_t LveDevice::FindMemoryType(const uint32_t _typeFilter, const vk::MemoryPropertyFlags _properties) const
	{
		const vk::PhysicalDeviceMemoryProperties mem_properties = physicalDevice.getMemoryProperties();

		// Parcourt chaque type de mémoire
		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			// Vérifie si le type de mémoire est compatible avec le filtre spécifié et possède toutes les propriétés requises
			if ((_typeFilter & (1 << i)) &&
			    (mem_properties.memoryTypes[i].propertyFlags & _properties) == _properties)
				// Si le type de mémoire convient, retourne son index
				return i;
		}
		// Si aucun type de mémoire adapté n'est trouvé, lance une exception
		throw std::runtime_error("failed to find suitable memory type!");
	}


	/**
	 * @brief Crée un tampon Vulkan avec la taille, l'utilisation et les propriétés spécifiées.
	 *
	 * Cette fonction crée un tampon Vulkan avec la taille, l'utilisation et les propriétés spécifiées,
	 * et associe la mémoire appropriée au tampon.
	 *
	 * @param _size La taille du tampon en octets.
	 * @param _usage Les indicateurs d'utilisation du tampon.
	 * @param _properties Les propriétés de mémoire du tampon.
	 * @param _buffer Une référence à l'objet tampon à créer.
	 * @param _bufferMemory Une référence à la mémoire allouée pour le tampon.
	 * @throws std::runtime_error en cas d'échec de la création ou de l'allocation de mémoire pour le tampon.
	 */
	void LveDevice::CreateBuffer(
		const vk::DeviceSize          _size,
		const vk::BufferUsageFlags    _usage,
		const vk::MemoryPropertyFlags _properties,
		vk::Buffer&                   _buffer,
		vk::DeviceMemory&             _bufferMemory) const
	{
		// Définit les informations du tampon
		const vk::BufferCreateInfo buffer_info(
			{},
			_size,
			_usage,
			vk::SharingMode::eExclusive
		);

		// Crée le tampon
		_buffer = device_.createBuffer(buffer_info);

		// Obtient les exigences de mémoire pour le tampon
		const vk::MemoryRequirements mem_requirements = device_.getBufferMemoryRequirements(_buffer);

		// Alloue la mémoire pour le tampon
		const vk::MemoryAllocateInfo alloc_info(
			mem_requirements.size,
			FindMemoryType(mem_requirements.memoryTypeBits, _properties)
		);

		_bufferMemory = device_.allocateMemory(alloc_info);

		// Associe la mémoire allouée au tampon
		device_.bindBufferMemory(_buffer, _bufferMemory, 0);
	}


	/**
	 * @brief Démarre une série de commandes Vulkan pour une utilisation unique.
	 *
	 * Cette fonction alloue et démarre un tampon de commandes Vulkan pour une utilisation unique.
	 * Le tampon de commandes retourné peut être utilisé pour exécuter des commandes Vulkan
	 * qui doivent être exécutées une seule fois.
	 *
	 * @return Le tampon de commandes Vulkan alloué et démarré.
	 * @throws std::runtime_error en cas d'échec de l'allocation du tampon de commandes.
	 */
	vk::CommandBuffer LveDevice::BeginSingleTimeCommands() const
	{
		// Alloue un tampon de commandes
		const vk::CommandBufferAllocateInfo alloc_info(
			commandPool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

		const vk::CommandBuffer command_buffer = device_.allocateCommandBuffers(alloc_info)[0];

		// Démarre l'enregistrement des commandes dans le tampon
		constexpr vk::CommandBufferBeginInfo begin_info(
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		);

		command_buffer.begin(begin_info);

		return command_buffer;
	}


	/**
	 * @brief Termine une série de commandes Vulkan pour une utilisation unique.
	 *
	 * Cette fonction termine l'enregistrement des commandes dans le tampon de commandes spécifié,
	 * soumet les commandes à la file de commandes graphiques pour exécution, attend la fin de l'exécution
	 * des commandes, puis libère le tampon de commandes.
	 *
	 * @param _commandBuffer Le tampon de commandes Vulkan à terminer et à soumettre.
	 * @throws std::runtime_error en cas d'échec de soumission des commandes ou d'attente de la fin de l'exécution.
	 */
	void LveDevice::EndSingleTimeCommands(const vk::CommandBuffer _commandBuffer) const
	{
		// Termine l'enregistrement des commandes dans le tampon de commandes
		_commandBuffer.end();

		// Soumet les commandes à la file de commandes graphiques pour exécution
		vk::SubmitInfo submit_info;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers    = &_commandBuffer;

		graphicsQueue_.submit(submit_info, nullptr);

		// Attend la fin de l'exécution des commandes
		graphicsQueue_.waitIdle();

		// Libère le tampon de commandes
		device_.freeCommandBuffers(commandPool, 1, &_commandBuffer);
	}


	/**
	 * @brief Copie les données d'un tampon source vers un tampon de destination.
	 *
	 * Cette fonction utilise un tampon de commandes temporaire pour copier les données
	 * du tampon source vers le tampon de destination. Elle garantit que l'opération de
	 * copie se produit de manière synchrone, c'est-à-dire que le tampon de commandes temporaire
	 * est soumis à la file de commandes graphiques, attend que toutes les opérations en cours
	 * sur cette file soient terminées, puis est libéré.
	 *
	 * @param _srcBuffer Le tampon source à partir duquel copier les données.
	 * @param _dstBuffer Le tampon de destination vers lequel copier les données.
	 * @param _size La taille des données à copier, en octets.
	 * @throws std::runtime_error en cas d'échec de l'allocation ou de l'enregistrement des commandes.
	 */
	void LveDevice::CopyBuffer(const vk::Buffer     _srcBuffer, const vk::Buffer _dstBuffer,
	                           const vk::DeviceSize _size) const
	{
		// Démarre l'enregistrement des commandes dans un tampon de commandes temporaire
		const vk::CommandBuffer command_buffer = BeginSingleTimeCommands();

		// Définit la région de copie
		vk::BufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Facultatif
		copyRegion.dstOffset = 0; // Facultatif
		copyRegion.size      = _size;

		// Effectue la copie des données entre les tampons
		command_buffer.copyBuffer(_srcBuffer, _dstBuffer, 1, &copyRegion);

		// Termine l'enregistrement des commandes et soumet le tampon de commandes à la file de commandes graphiques
		EndSingleTimeCommands(command_buffer);
	}


	/**
	 * @brief Copie les données d'un tampon vers une image Vulkan.
	 *
	 * Cette fonction utilise un tampon de commandes temporaire pour copier les données
	 * du tampon vers l'image Vulkan. Elle garantit que l'opération de copie se produit de manière
	 * synchrone, c'est-à-dire que le tampon de commandes temporaire est soumis à la file de commandes
	 * graphiques, attend que toutes les opérations en cours sur cette file soient terminées, puis est libéré.
	 *
	 * @param _buffer Le tampon contenant les données à copier vers l'image.
	 * @param _image L'image Vulkan de destination vers laquelle copier les données.
	 * @param _width La largeur de l'image en pixels.
	 * @param _height La hauteur de l'image en pixels.
	 * @param _layerCount Le nombre de couches de l'image.
	 * @throws std::runtime_error en cas d'échec de l'allocation ou de l'enregistrement des commandes.
	 */
	void LveDevice::CopyBufferToImage(
		const vk::Buffer _buffer, const vk::Image _image, const uint32_t _width, const uint32_t _height,
		const uint32_t   _layerCount) const
	{
		// Démarre l'enregistrement des commandes dans un tampon de commandes temporaire
		const vk::CommandBuffer command_buffer = BeginSingleTimeCommands();

		// Configure la région de copie
		vk::BufferImageCopy region{};
		region.bufferOffset      = 0;
		region.bufferRowLength   = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel       = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount     = _layerCount;

		region.imageOffset = vk::Offset3D{0, 0, 0};
		region.imageExtent = vk::Extent3D{_width, _height, 1};

		// Copie les données du tampon vers l'image Vulkan
		command_buffer.copyBufferToImage(
			_buffer,
			_image,
			vk::ImageLayout::eTransferDstOptimal,
			1,
			&region);

		// Termine l'enregistrement des commandes et soumet le tampon de commandes à la file de commandes graphiques
		EndSingleTimeCommands(command_buffer);
	}


	/**
	 * @brief Crée une image Vulkan avec les informations spécifiées.
	 *
	 * Cette fonction crée une image Vulkan en utilisant les informations fournies dans
	 * l'objet VkImageCreateInfo. Elle alloue également la mémoire nécessaire pour cette
	 * image et associe cette mémoire à l'image créée.
	 *
	 * @param _imageInfo Les informations nécessaires à la création de l'image.
	 * @param _properties Les propriétés de la mémoire de l'image.
	 * @param _image Référence où l'objet image créé sera stocké.
	 * @param _imageMemory Référence où la mémoire de l'image sera stockée.
	 * @throws std::runtime_error en cas d'échec de la création de l'image ou de l'allocation de mémoire.
	 */
	void LveDevice::CreateImageWithInfo(
		const vk::ImageCreateInfo&    _imageInfo,
		const vk::MemoryPropertyFlags _properties,
		vk::Image&                    _image,
		vk::DeviceMemory&             _imageMemory) const
	{
		// Crée l'image Vulkan
		try
		{
			_image = device_.createImage(_imageInfo);
		}
		catch (const vk::SystemError& e)
		{
			throw std::runtime_error("failed to create image!");
		}

		// Récupère les exigences de mémoire de l'image
		const vk::MemoryRequirements mem_requirements = device_.getImageMemoryRequirements(_image);

		// Alloue la mémoire pour l'image
		vk::MemoryAllocateInfo alloc_info{};
		alloc_info.setAllocationSize(mem_requirements.size);
		alloc_info.setMemoryTypeIndex(FindMemoryType(mem_requirements.memoryTypeBits, _properties));

		try
		{
			_imageMemory = device_.allocateMemory(alloc_info);
		}
		catch (const vk::SystemError& e)
		{
			// Libère l'image si l'allocation de mémoire échoue
			device_.destroyImage(_image);
			throw std::runtime_error("failed to allocate image memory!");
		}

		// Associe la mémoire allouée à l'image
		try
		{
			device_.bindImageMemory(_image, _imageMemory, 0);
		}
		catch (const vk::SystemError& e)
		{
			// Libère l'image et la mémoire si la liaison échoue
			device_.destroyImage(_image);
			device_.freeMemory(_imageMemory);
			throw std::runtime_error("failed to bind image memory!");
		}
	}
} // namespace lve
