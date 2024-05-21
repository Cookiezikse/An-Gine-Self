#include "Systems/point_light_system.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <vulkan/vulkan.hpp>

// std
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>

#include "GameObject/GameObject.h"
#include "GameObject/Components/Light.h"
#include "GameObject/Components/Transform.h"
#include "Modules/TimeModule.h"


namespace lve
{
	PointLightSystem::PointLightSystem(LveDevice&              _device, vk::RenderPass _renderPass,
	                                   vk::DescriptorSetLayout _globalSetLayout) : lveDevice(_device)
	{
		CreatePipelineLayout(_globalSetLayout);
		CreatePipeline(_renderPass);
	}

	PointLightSystem::~PointLightSystem() { lveDevice.Device().destroyPipelineLayout(pipelineLayout, nullptr); }

	void PointLightSystem::CreatePipelineLayout(vk::DescriptorSetLayout _globalSetLayout)
	{
		vk::PushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
		pushConstantRange.offset     = 0;
		pushConstantRange.size       = sizeof(PointLightPushConstants);

		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{_globalSetLayout};

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType                  = vk::StructureType::ePipelineLayoutCreateInfo;
		pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts            = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
		if (lveDevice.Device().createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to create pipeline layout!");
	}

	void PointLightSystem::	CreatePipeline(vk::RenderPass _renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		LvePipeline::EnableAlphaBlending(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass     = _renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline                   = std::make_unique<LvePipeline>(
			lveDevice,
			"Shaders/point_light.vert.spv",
			"Shaders/point_light.frag.spv",
			pipelineConfig);
	}

	void PointLightSystem::Update(const std::vector<GameObject*>& _gameObjects, GlobalUbo& _ubo)
	{
		const auto rotate_light = rotate(glm::mat4(1.f), 0.5f * TimeModule::GetDeltaTime(), {0.f, -1.f, 0.f});
		int  light_index  = 0;
		for (auto& game_object : _gameObjects)
		{
			const auto light_component = game_object->GetComponent<Light>();
			if (light_component == nullptr) continue;

			assert(light_index < MAX_LIGHTS && "Point lights exceed maximum specified");

			// copy light to ubo
			_ubo.pointLights[light_index].position = glm::vec4(game_object->GetTransform()->GetPosition(), 1.f);
			_ubo.pointLights[light_index].color    = glm::vec4(game_object->GetColor(), light_component->lightIntensity);

			light_index ++;
		}
		_ubo.numLights = light_index;
	}

	void PointLightSystem::Render(const std::vector<GameObject*>& _gameObjects, const LveCamera& _camera, const vk::CommandBuffer _commandBuffer, const vk::DescriptorSet _globalDescriptorSet) const
	{
		lvePipeline->Bind(_commandBuffer);

		// Liaison de l'ensemble de descripteurs global
		_commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipelineLayout,
			0,
			_globalDescriptorSet,
			nullptr);

		// Itération à travers les lumières triées
		for (const auto& game_object : _gameObjects)
		{
			if (const auto light_component = game_object->GetComponent<Light>(); light_component == nullptr) continue;
			PointLightPushConstants push{};
			push.position = glm::vec4(game_object->GetTransform()->GetPosition(), 1.f);
			push.color    = glm::vec4(game_object->GetColor(), game_object->GetComponent<Light>()->lightIntensity);
			push.radius   = game_object->GetTransform()->GetScale().x;

			// Mise à jour des push constants
			_commandBuffer.pushConstants<PointLightPushConstants>(
				pipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				push);

			// Dessin d'une primitive pour représenter la lumière
			_commandBuffer.draw(6, 1, 0, 0);
		}
	}
} // namespace lve
