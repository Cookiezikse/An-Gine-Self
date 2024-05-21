#include "Systems/simple_render_system.h"

#include "LveEngine/lve_descriptors.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <iostream>

// std
#include <cassert>
#include <stdexcept>
#include "GameObject/Components/Transform.h"
#include "GameObject/GameObject.h"

namespace lve
{
	SimpleRenderSystem::SimpleRenderSystem(LveDevice&              _device, vk::RenderPass _renderPass,
	                                       vk::DescriptorSetLayout _globalSetLayout) : lveDevice(_device)
	{
		CreatePipelineLayout(_globalSetLayout);
		CreatePipeline(_renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() { lveDevice.Device().destroyPipelineLayout(pipelineLayout, nullptr); }

	void SimpleRenderSystem::CreatePipelineLayout(vk::DescriptorSetLayout _globalSetLayout)
	{
		vk::PushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
		pushConstantRange.offset     = 0;
		pushConstantRange.size       = sizeof(SimplePushConstantData);

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


	void SimpleRenderSystem::CreatePipeline(vk::RenderPass _renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass     = _renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline                   = std::make_unique<LvePipeline>(
			lveDevice,
			"Shaders/simple_shader.vert.spv",
			"Shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void SimpleRenderSystem::RenderGameObjects(const std::vector<GameObject*>& _gameObjects, const LveCamera& _camera, const vk::CommandBuffer _commandBuffer, std::vector<std::vector<vk::DescriptorSet>*>* _DescriptorSetsAll, int _frameIndex)  const
	{
		// Liaison du pipeline
		lvePipeline->Bind(_commandBuffer);

		// Liaison de l'ensemble de descripteurs global
		//_commandBuffer.bindDescriptorSets(
		//	vk::PipelineBindPoint::eGraphics,
		//	pipelineLayout,
		//	0,
		//	_globalDescriptorSet,
		//	nullptr);

		for (const auto& game_object : _gameObjects)
		{
			if (game_object->GetModel() == nullptr || !game_object->GetVisible()) continue;

			if (game_object->GetTexture() > _DescriptorSetsAll->size() - 1)
			{
				_commandBuffer.bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,
					pipelineLayout,
					0,
					(*_DescriptorSetsAll)[0]->at(_frameIndex),
					nullptr);
			}
			else
			{
				_commandBuffer.bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,
					pipelineLayout,
					0,
					(*_DescriptorSetsAll)[game_object->GetTexture()]->at(_frameIndex),
					nullptr);
			}



			SimplePushConstantData push{};
			push.modelMatrix  = game_object->GetTransform()->Mat4();
			push.normalMatrix = game_object->GetTransform()->NormalMatrix();

			// Mise � jour des push constants
			_commandBuffer.pushConstants<SimplePushConstantData>(
				pipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				push);

			// Liaison du mod�le et dessin
			game_object->GetModel()->Bind(_commandBuffer);
			game_object->GetModel()->Draw(_commandBuffer);
		}
	}
} // namespace lve
