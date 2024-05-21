#pragma once
#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Modules/Module.h"

// TODO : Vérifier si c pertinent
struct HUDComponent
{
	float       x,     y;      // Position de l'élément HUD
	float       width, height; // Largeur et hauteur de l'élément HUD
	VkImage     texture;       // Image Vulkan de l'élément HUD
	VkImageView imageView;     // Vue de l'image Vulkan
	VkSampler   sampler;       // Échantillonneur Vulkan
};

class HUDModule : public Module
{
	public:
		HUDModule(VkDevice _device, VkRenderPass _renderPass, VkQueue _graphicsQueue, VkCommandPool _commandPool);
		~HUDModule();

		void        AddComponent(float _x, float _y, float _width, float _height, const std::string& _texturePath);
		static void RemoveComponent();
		void        CleanupComponents();
		void        Render(VkCommandBuffer _commandBuffer);

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

		static constexpr int HEIGHT = 600;
		static constexpr int WIDTH  = 800;

	private:
		VkDevice                  device;
		VkRenderPass              renderPass;
		VkQueue                   graphicsQueue;
		VkCommandPool             commandPool;
		std::vector<HUDComponent> components;

		std::vector<char> ReadFile(const std::string& _filename);
		// Méthodes pour créer et gérer les ressources Vulkan
		/*VkImage createTextureImage(const std::string& texturePath);*/
		VkImageView             CreateTextureImageView(VkImage _image);
		[[nodiscard]] VkSampler CreateTextureSampler() const;
		void                    TransitionImageLayout(VkImage       _image, VkFormat _format, VkImageLayout _oldLayout,
		                                              VkImageLayout _newLayout);

		void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties,
		                  VkBuffer& _buffer, VkDeviceMemory& _bufferMemory) const;
		void CreateImage(VkDeviceSize          _size, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage,
		                 VkMemoryPropertyFlags _properties, VkImage& _image, VkDeviceMemory& _imageMemory) const;

		void CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height) const;

		VkImageView CreateImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags);
};

#endif // HUD_MANAGER_H
