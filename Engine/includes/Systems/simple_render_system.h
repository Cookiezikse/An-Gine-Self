#pragma once

#include <vulkan/vulkan.hpp>

#include "LveEngine/lve_device.h"
#include "LveEngine/lve_pipeline.h"
#include "Camera/lve_camera.h"

//std
#include <memory>
#include <vector>

class GameObject;

namespace lve
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{1.f};
		glm::mat4 normalMatrix{1.f};
	};

	/**
	 * @brief Classe représentant la première application utilisant Vulkan.
	 *
	 * Cette classe gère l'exécution de la première application Vulkan, incluant la création de la fenêtre.
	 */
	class SimpleRenderSystem
	{
		public:
			SimpleRenderSystem(LveDevice&              _device, vk::RenderPass _renderPass,
			                   vk::DescriptorSetLayout _globalSetLayout);
			~SimpleRenderSystem();

			SimpleRenderSystem(const SimpleRenderSystem&)           = delete;
			SimpleRenderSystem operator=(const SimpleRenderSystem&) = delete;

			/**
			 * @brief Rend les objets de jeu avec le système de rendu simple.
			 *
			 * Cette méthode rend les objets de jeu avec le système de rendu simple en utilisant le pipeline graphique
			 * et le layout du pipeline spécifiés dans le cadre d'une Frame.
			 *
			 * @param _gameObjects
			 * @param _camera
			 * @param _commandBuffer
			 * @param _globalDescriptorSet
			 * @param _tex1DescriptorSet
			 * @param _tex2DescriptorSet
			 */
			void RenderGameObjects(const std::vector<GameObject*>& _gameObjects, const LveCamera& _camera, const vk::CommandBuffer _commandBuffer, std::vector<std::vector<vk::DescriptorSet>*>* _DescriptorSetsAll, int _frameIndex)  const;

		private:
			/**
			 * @brief Crée le layout du pipeline pour le système de rendu simple.
			 *
			 * Cette méthode crée le layout du pipeline pour le système de rendu simple en spécifiant le layout du descripteur global
			 * ainsi que la plage des constantes poussées utilisées par les shaders.
			 *
			 * @param _globalSetLayout Le layout du descripteur global.
			 */
			void CreatePipelineLayout(vk::DescriptorSetLayout _globalSetLayout);

			/**
			 * @brief Crée le pipeline pour le système de rendu simple.
			 *
			 * Cette méthode crée le pipeline pour le système de rendu simple en utilisant les shaders spécifiés et le layout de pipeline
			 * préalablement créé. Le rendu se fait selon le render pass spécifié.
			 *
			 * @param _renderPass Le render pass utilisé pour le rendu.
			 */
			void CreatePipeline(vk::RenderPass _renderPass);

			LveDevice& lveDevice;


			std::unique_ptr<LvePipeline> lvePipeline;
			vk::PipelineLayout           pipelineLayout;
	};
} // namespace lve
