#include "LveEngine/lve_renderer.h"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve
{
	LveRenderer::LveRenderer(WindowModule* _windowModule, LveDevice& _device) : windowModule{_windowModule}, lveDevice{_device}
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	LveRenderer::~LveRenderer() { FreeCommandBuffers(); }

	void LveRenderer::RecreateSwapChain()
	{
		auto extent = windowModule->GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = windowModule->GetExtent();
			glfwWaitEvents();
		}

		lveDevice.Device().waitIdle(); // Utilisation de vk::Device::waitIdle()

		if (lveSwapChain == nullptr)
		{
			lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
		}
		else
		{
			std::shared_ptr<LveSwapChain> oldSwapChain = std::move(lveSwapChain);
			lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*lveSwapChain))
				throw std::runtime_error(
					"Swap chain image(or depth) format has changed!");
		}
	}


	void LveRenderer::CreateCommandBuffers()
	{
		commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		vk::CommandBufferAllocateInfo alloc_info{};
		alloc_info.sType              = vk::StructureType::eCommandBufferAllocateInfo;
		alloc_info.level              = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandPool        = lveDevice.GetCommandPool();
		alloc_info.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (lveDevice.Device().allocateCommandBuffers(&alloc_info, commandBuffers.data()) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to allocate command buffers!");
	}


	void LveRenderer::FreeCommandBuffers()
	{
		lveDevice.Device().freeCommandBuffers(
			lveDevice.GetCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}


	vk::CommandBuffer LveRenderer::BeginFrame()
	{
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");

		const auto result = lveSwapChain->AcquireNextImage(&currentImageIndex);
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
			RecreateSwapChain();
			return nullptr;
		}
		if (result != vk::Result::eSuccess) throw std::runtime_error("Failed to acquire swap chain image!");

		isFrameStarted = true;

		const auto                 command_buffer = GetCurrentCommandBuffer();
		vk::CommandBufferBeginInfo begin_info{};
		begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;

		if (command_buffer.begin(&begin_info) != vk::Result::eSuccess)
			throw std::runtime_error(
				"failed to begin recording command buffer!");
		//return commandBuffer;
		return command_buffer;
	}


	void LveRenderer::EndFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

		const auto command_buffer = GetCurrentCommandBuffer();
		command_buffer.end();

		if (const auto result = lveSwapChain->SubmitCommandBuffers(&command_buffer, &currentImageIndex);
			result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
			windowModule->WasWindowResized())
		{
			windowModule->ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		isFrameStarted    = false;
		currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void LveRenderer::EndFrame(const vk::CommandBuffer* _commandBuffer)
	{
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

		_commandBuffer->end();

		if (const auto result = lveSwapChain->SubmitCommandBuffers(_commandBuffer, &currentImageIndex);
			result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
			windowModule->WasWindowResized())
		{
			windowModule->ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		isFrameStarted    = false;
		currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}


	void LveRenderer::BeginSwapChainRenderPass(const vk::CommandBuffer _commandBuffer) const
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			_commandBuffer == GetCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");

		vk::RenderPassBeginInfo render_pass_info{};
		render_pass_info.sType       = vk::StructureType::eRenderPassBeginInfo;
		render_pass_info.renderPass  = lveSwapChain->GetRenderPass();
		render_pass_info.framebuffer = lveSwapChain->GetFrameBuffer(currentImageIndex);

		render_pass_info.renderArea.offset = vk::Offset2D{0, 0};
		render_pass_info.renderArea.extent = lveSwapChain->GetSwapChainExtent();

		std::array<vk::ClearValue, 2> clear_values{};
		clear_values[0].color            = vk::ClearColorValue(std::array<float, 4>{0.01f, 0.01f, 0.01f, 1.0f});
		clear_values[1].depthStencil     = vk::ClearDepthStencilValue{1.0f, 0};
		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues    = clear_values.data();

		_commandBuffer.beginRenderPass(&render_pass_info, vk::SubpassContents::eInline);

		vk::Viewport viewport{};
		viewport.x        = 0.0f;
		viewport.y        = 0.0f;
		viewport.width    = static_cast<float>(lveSwapChain->GetSwapChainExtent().width);
		viewport.height   = static_cast<float>(lveSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		const vk::Rect2D scissor{{0, 0}, lveSwapChain->GetSwapChainExtent()};
		_commandBuffer.setViewport(0, 1, &viewport);
		_commandBuffer.setScissor(0, 1, &scissor);
	}


	void LveRenderer::EndSwapChainRenderPass(const vk::CommandBuffer _commandBuffer) const
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(_commandBuffer == GetCurrentCommandBuffer() &&
			"Can't end render pass on command buffer from a different frame");
		_commandBuffer.endRenderPass();
	}
} // namespace lve
