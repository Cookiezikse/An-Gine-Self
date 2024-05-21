#include "Modules/RHIVulkanModule.h"

#include <fstream>
#include <iostream>
#include "GameObject/Components/Transform.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "Modules/ModuleManager.h"
#include "Modules/TimeModule.h"
#include "Scene/SceneManager.h"

RHIVulkanModule::RHIVulkanModule()
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}
void RHIVulkanModule:: Init()
{
	windowModule = moduleManager->GetModule<WindowModule>();
	p_lveDevice = new lve::LveDevice{ windowModule };
	p_lveRenderer = new lve::LveRenderer{ windowModule, *p_lveDevice };
}

void RHIVulkanModule::AddTextureToPool(const std::string& _filepath) {

	
	std::ifstream file(_filepath);
	if (!file.good()) { std::cout << "Failed to add texture, file path does not exist: " << _filepath << std::endl; }// return; }

	const auto global_set_layout = lve::LveDescriptorSetLayout::Builder(*p_lveDevice)
		.AddBinding(0, vk::DescriptorType::eUniformBuffer,
			vk::ShaderStageFlagBits::eAllGraphics)
		.AddBinding(1, vk::DescriptorType::eCombinedImageSampler,
			vk::ShaderStageFlagBits::eFragment)
		.Build();

	lve::LveTexture* NewTexture;
	if (!file.good()) { NewTexture = new lve::LveTexture(*p_lveDevice, "Textures/bugtexture.png"); }// return; }
	else { NewTexture = new lve::LveTexture(*p_lveDevice, _filepath); }

	ListTextures.push_back(vk::DescriptorImageInfo());
	ListTextures.back().sampler = NewTexture->getSampler();
	ListTextures.back().imageView = NewTexture->getImageView();
	ListTextures.back().imageLayout = NewTexture->getImageLayout();

	ListDescriptors.push_back(new std::vector<vk::DescriptorSet>());
	ListDescriptors.back()->resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	builder = new lve::LveDescriptorPool::Builder{ *p_lveDevice };
	builder->SetMaxSets(ListDescriptors.size() * lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder->Build();
	int i = 0;
	for (std::vector<vk::DescriptorSet>* descriptorSet : ListDescriptors)
	{
		for (size_t j = 0; j < descriptorSet->size(); j++)
		{
			int tex = i;
			if (i > ListTextures.size() - 1) {
				tex = 0;
			}

			auto buffer_info = uboBuffers[j]->DescriptorInfo();
			lve::LveDescriptorWriter(*global_set_layout, *globalPool)
				.WriteBuffer(0, &buffer_info)
				.WriteImage(1, &ListTextures[tex])
				.Build((*descriptorSet)[j]);
		}
		i++;
	}

}


void RHIVulkanModule::Start()
{
	builder = new lve::LveDescriptorPool::Builder{ *p_lveDevice };
	builder->SetMaxSets(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder->Build();

	camera = new lve::LveCamera{};
	uboBuffers.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<lve::LveBuffer>(
			*p_lveDevice,
			sizeof(lve::GlobalUbo),
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible);

		uboBuffers[i]->Map();
	}

	const auto global_set_layout = lve::LveDescriptorSetLayout::Builder(*p_lveDevice)
		.AddBinding(0, vk::DescriptorType::eUniformBuffer,
			vk::ShaderStageFlagBits::eAllGraphics)
		.AddBinding(1, vk::DescriptorType::eCombinedImageSampler,
			vk::ShaderStageFlagBits::eFragment)
		.Build();

	// Je laisse la construction du globalDesc comme �a pour montrer 
	texture1 = new lve::LveTexture(*p_lveDevice, "Textures/bugtexture2.png");

	vk::DescriptorImageInfo imageInfo{};
	imageInfo.sampler = texture1->getSampler();
	imageInfo.imageView = texture1->getImageView();
	imageInfo.imageLayout = texture1->getImageLayout();

	globalDescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < globalDescriptorSets.size(); i++)
	{
		auto buffer_info = uboBuffers[i]->DescriptorInfo();
		lve::LveDescriptorWriter(*global_set_layout, *globalPool)
			.WriteBuffer(0, &buffer_info)
			.WriteImage(1, &imageInfo)
			.Build(globalDescriptorSets[i]);
	}

	ListTextures.push_back(imageInfo);

	ListDescriptors.push_back(&globalDescriptorSets);

	AddTextureToPool("Textures/unnamed.png");
	AddTextureToPool("Textures/viking_room.png");
	AddTextureToPool("Textures/grass.jpg");
	AddTextureToPool("Textures/gras.truc"); // TEST	
	AddTextureToPool("Textures/noobtex.png");	
	AddTextureToPool("Textures/sand1.png");
	AddTextureToPool("Textures/sand2.png");
	AddTextureToPool("Textures/sand3.png");

	ListTexturesNames->push_back("bugtexture.png");
	ListTexturesNames->push_back("unnamed.png");
	ListTexturesNames->push_back("viking_room.png");
	ListTexturesNames->push_back("grass.jpg");
	ListTexturesNames->push_back("gras.truc");
	ListTexturesNames->push_back("noobtex.png");
	ListTexturesNames->push_back("sand1.png");
	ListTexturesNames->push_back("sand2.png");
	ListTexturesNames->push_back("sand3.png");

	simpleRenderSystem = new lve::SimpleRenderSystem{
		*p_lveDevice, p_lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};
	pointLightSystem = new lve::PointLightSystem{
		*p_lveDevice, p_lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};

	viewerObject = new GameObject(GameObject::CreateGameObject());
	viewerObject->GetTransform()->SetPosition(glm::vec3{ 0.f,0.f,-2.5f });

}

void RHIVulkanModule::FixedUpdate()
{
}

void RHIVulkanModule::Update()
{
	gameObjects = moduleManager->GetModule<SceneManager>()->GetCurrentScene()->GetAllGameObject();

	cameraController.MoveInPlaneXZ(windowModule->GetGlfwWindow(), TimeModule::GetDeltaTime(), *viewerObject);
	camera->SetViewYXZ(viewerObject->GetPosition(), viewerObject->GetRotation());

	const float aspect = p_lveRenderer->GetAspectRatio();
	camera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

	ubo.projection = camera->GetProjection();
	ubo.view = camera->GetView();

	pointLightSystem->Update(gameObjects, ubo);
}

void RHIVulkanModule::PreRender()
{
	currentCommandBuffer.reset(new vk::CommandBuffer(p_lveRenderer->BeginFrame()));
	if (currentCommandBuffer)
	{
		frameIndex = p_lveRenderer->GetFrameIndex();
		const float frame_time = TimeModule::GetDeltaTime();
		// update

		uboBuffers[frameIndex]->WriteToBuffer(&ubo);
		uboBuffers[frameIndex]->Flush();
		p_lveRenderer->BeginSwapChainRenderPass(*currentCommandBuffer);
	}
	
}

void RHIVulkanModule::Render()
{
	gameObjects = moduleManager->GetModule<SceneManager>()->GetCurrentScene()->GetAllGameObject();
	simpleRenderSystem->RenderGameObjects(gameObjects, *camera, *currentCommandBuffer, &ListDescriptors, frameIndex);       //render shadow casting objects
	pointLightSystem->Render(gameObjects, *camera, *currentCommandBuffer, globalDescriptorSets[frameIndex]);                 //render shadow casting objects
}

void RHIVulkanModule::RenderGui()
{
}

void RHIVulkanModule::PostRender()
{
	p_lveRenderer->EndSwapChainRenderPass(*currentCommandBuffer);
	p_lveRenderer->EndFrame(currentCommandBuffer.get());
}

void RHIVulkanModule::Release()
{
}

void RHIVulkanModule::Finalize()
{
}
