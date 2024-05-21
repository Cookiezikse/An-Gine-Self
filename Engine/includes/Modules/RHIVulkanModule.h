#pragma once
// vulkan_rhi.h
#pragma once

#include <vulkan/vulkan.hpp>

#include "Inputs/keyboard_movement_controller.h"
#include "rhi.h"

#include "LveEngine/lve_descriptors.h"
#include "LveEngine/lve_model.h"
#include "LveEngine/lve_pipeline.h"
#include "LveEngine/lve_renderer.h"
#include "LveEngine/lve_texture.h"
#include "Modules/WindowModule.h"

#include "Systems/point_light_system.h"
#include "Systems/simple_render_system.h"

/**
 * @brief Classe représentant un module RHI (Render Hardware Interface) utilisant Vulkan.
 *
 * Cette classe hérite de la classe RHIModule et implémente les fonctionnalités spécifiques à Vulkan pour le rendu graphique.
 */
class RHIVulkanModule final : public RHIModule
{
	public:

		/**
		 * @brief Constructeur de la classe RHIVulkanModule.
		 *
		 * Ce constructeur initialise un nouvel objet RHIVulkanModule.
		 */
		RHIVulkanModule();

		/**
		 * @brief Destructeur de la classe RHIVulkanModule.
		 *
		 * Ce destructeur libère les ressources associées à l'objet RHIVulkanModule.
		 */
		~RHIVulkanModule() override;


		/**
		 * @brief Obtient le tampon de commandes actuel.
		 *
		 * Cette méthode retourne le tampon de commandes Vulkan actuellement utilisé par le module RHI.
		 *
		 * @return Un pointeur vers le tampon de commandes Vulkan actuellement utilisé.
		 */
		[[nodiscard]] vk::CommandBuffer* GetCurrentCommandBuffer() const { return currentCommandBuffer.get(); }

		void AddTextureToPool(const std::string& _filepath);
		
#pragma region Event

		/**
		* @brief Initialise le module.
		*/
		void Init() override;

		/**
		 * @brief D�marre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise � jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met � jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pr�-rendu du module.
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
		 * @brief Lib�re les ressources utilis�es par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;

#pragma endregion

		std::vector<std::vector<vk::DescriptorSet>*> GetListDescriptors() { return ListDescriptors; }

		std::vector<std::string>* GetListTexturesNames() { return ListTexturesNames; }

		std::vector<vk::DescriptorImageInfo> GetListTextures() { return ListTextures; }

		void AddListTexturesNames(std::string _newFile) { ListTexturesNames->push_back(_newFile); }

	private:
		/**< Tampon de commandes Vulkan actuellement utilisé. */
		std::unique_ptr<vk::CommandBuffer> currentCommandBuffer;

		/**< Système de rendu simple. */
		lve::SimpleRenderSystem* simpleRenderSystem;

		/**< Système de lumières ponctuelles. */
		lve::PointLightSystem* pointLightSystem;

		/**< Constructeur de pool de descripteurs de Vulkan. */
		lve::LveDescriptorPool::Builder* builder;

		/**< Disposition de l'ensemble de descripteurs global. */
		std::unique_ptr<lve::LveDescriptorSetLayout, std::default_delete<lve::LveDescriptorSetLayout>>* globalSetLayout;

		/**< Texture 1. */
		lve::LveTexture* texture1;

		/**< Texture 2. */
		lve::LveTexture* texture2;

		/**< Texture 3. */
		lve::LveTexture* texture3;

		/**< Caméra utilisée pour la visualisation. */
		lve::LveCamera* camera;

		/**< Contrôleur de mouvement de la caméra. */
		lve::KeyboardMovementController cameraController{};

		/**< Objet de visualisation. */
		GameObject* viewerObject;

		/**< Ensembles de descripteurs globaux. */
		std::vector<vk::DescriptorSet> globalDescriptorSets;

		/**< Ensembles de descripteurs pour la texture 1. */
		std::vector<vk::DescriptorSet> tex1DescriptorSets;

		/**< Ensembles de descripteurs pour la texture 2. */
		std::vector<vk::DescriptorSet> tex2DescriptorSets;

		/**< Tampons UBO. */
		std::vector<std::unique_ptr<lve::LveBuffer>> uboBuffers;

		/**< Objets de jeu. */
		std::vector<GameObject*> gameObjects;

		/**< Pool de descripteurs global. */
		std::unique_ptr<lve::LveDescriptorPool> globalPool{};

		/**< Indice de trame. */
		int frameIndex;

		/**< Données UBO globales. */
		lve::GlobalUbo ubo{};

		/**< Module de fenêtre. */
		WindowModule* windowModule = nullptr;

		/**< Chaîne de swaps LVE. */
		lve::LveSwapChain* p_lveSwapChain;

		/**< Modèle LVE. */
		lve::LveModel* p_lveModel;

		/**< Pipeline LVE. */
		lve::LvePipeline* p_lvePipeline;

		/**< Tampon LVE. */
		lve::LveBuffer* p_lveBuffer;

		/**< Pool de descripteurs LVE. */
		lve::LveDescriptorPool* p_lveDescriptorPool;

		/**< Disposition de l'ensemble de descripteurs LVE. */
		lve::LveDescriptorSetLayout* p_lveDescriptorSetLayout;

		/**< Écrivain de descripteurs LVE. */
		lve::LveDescriptorWriter* p_lveDescriptorWriter;

		std::vector<vk::DescriptorImageInfo> ListTextures;

		std::vector<std::vector<vk::DescriptorSet>*> ListDescriptors;

		std::vector<std::string>* ListTexturesNames = new std::vector<std::string>();

};
