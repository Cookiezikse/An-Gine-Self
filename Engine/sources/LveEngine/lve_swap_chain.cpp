#include "LveEngine/lve_swap_chain.h"

// std
#include <array>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>


namespace lve
{
	LveSwapChain::LveSwapChain(LveDevice& _deviceRef, const vk::Extent2D _windowExtent) : lveDevice{_deviceRef},
		windowExtent{_windowExtent}
	{
		Init();
	}

	LveSwapChain::LveSwapChain(LveDevice&                           _deviceRef, const vk::Extent2D _extent,
	                           const std::shared_ptr<LveSwapChain>& _previous) : lveDevice{_deviceRef},
		windowExtent{_extent},
		oldSwapChain{_previous}
	{
		Init();
		oldSwapChain = nullptr;
	}

	void LveSwapChain::Init()
	{
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResources();
		CreateFrameBuffers();
		CreateSyncObjects();
	}


	LveSwapChain::~LveSwapChain()
	{
		// Nettoie les image views de la chaîne de swap
		for (const auto image_view : swapChainImageViews)
		{
			lveDevice.Device().destroyImageView(image_view, nullptr);
		}
		swapChainImageViews.clear();

		// Détruit la chaîne de swap
		if (swapChain != nullptr)
		{
			lveDevice.Device().destroySwapchainKHR(swapChain, nullptr);
			swapChain = nullptr;
		}

		// Nettoie les images de profondeur, leurs vues et la mémoire associée
		for (int i = 0; i < depthImages.size(); i++)
		{
			lveDevice.Device().destroyImageView(depthImageViews[i], nullptr);
			lveDevice.Device().destroyImage(depthImages[i], nullptr);
			lveDevice.Device().freeMemory(depthImageMemorys[i], nullptr);
		}

		// Nettoie les framebuffers de la chaîne de swap
		for (const auto frame_buffer : swapChainFramebuffers)
		{
			lveDevice.Device().destroyFramebuffer(frame_buffer, nullptr);
		}

		// Détruit le render pass
		lveDevice.Device().destroyRenderPass(renderPass, nullptr);

		// Nettoie les objets de synchronisation
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			lveDevice.Device().destroySemaphore(renderFinishedSemaphores[i], nullptr);
			lveDevice.Device().destroySemaphore(imageAvailableSemaphores[i], nullptr);
			lveDevice.Device().destroyFence(inFlightFences[i], nullptr);
		}
	}

	vk::Result LveSwapChain::AcquireNextImage(uint32_t* _imageIndex) const
	{
		// Attend que les barrières de synchronisation soient satisfaites
		if (const vk::Result result = lveDevice.Device().waitForFences(1, &inFlightFences[currentFrame], VK_TRUE,
		                                                               std::numeric_limits<uint64_t>::max());
			result != vk::Result::eSuccess)
			std::cout << "ALERTE" << std::endl;

		// Acquiert l'index de l'image suivante dans la chaîne de swap
		// en utilisant l'extension KHR_swapchain
		const vk::Result result = lveDevice.Device().acquireNextImageKHR(
			swapChain,                              // La chaîne de swap
			std::numeric_limits<uint64_t>::max(),   // Durée d'attente infinie
			imageAvailableSemaphores[currentFrame], // Sémaphore non signalé (attendre qu'il soit signalé avant)
			nullptr,                                // Pas de gestion de l'objet de barrière de blocage
			_imageIndex);                           // L'index de l'image acquise

		return result;
	}

	vk::Result LveSwapChain::SubmitCommandBuffers(const vk::CommandBuffer* buffers, uint32_t* imageIndex)
	{
		// Attend la fin des commandes en cours d'exécution pour l'image en cours d'acquisition
		if (imagesInFlight[*imageIndex] != nullptr)
			if (const vk::Result result = lveDevice.Device().waitForFences(1, &imagesInFlight[*imageIndex], VK_TRUE,
			                                                               std::numeric_limits<uint64_t>::max());
				result != vk::Result::eSuccess)
			{
				// Gérer l'erreur ou afficher un message d'erreur
			}
		imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

		// Configuration de la soumission des commandes
		vk::SubmitInfo submit_info;
		submit_info.sType = vk::StructureType::eSubmitInfo;

		const vk::Semaphore wait_semaphores[] = {imageAvailableSemaphores[currentFrame]};
		// Déclaration d'un tableau de type VkPipelineStageFlags
		constexpr vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		// Initialisation du tableau avec un seul élément contenant le drapeau VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT

		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores    = wait_semaphores;
		submit_info.pWaitDstStageMask  = wait_stages;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers    = buffers;

		const vk::Semaphore signal_semaphores[] = {renderFinishedSemaphores[currentFrame]};
		submit_info.signalSemaphoreCount        = 1;
		submit_info.pSignalSemaphores           = signal_semaphores;

		// Soumet les commandes à la file de commandes graphiques
		lveDevice.Device().resetFences(1, &inFlightFences[currentFrame]);
		if (lveDevice.GraphicsQueue().submit(1, &submit_info, inFlightFences[currentFrame]) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to submit draw command buffer!");

		// Configuration de la présentation
		vk::PresentInfoKHR present_info;
		present_info.sType = vk::StructureType::ePresentInfoKHR;

		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores    = signal_semaphores;

		const vk::SwapchainKHR swap_chains[] = {swapChain};
		present_info.swapchainCount          = 1;
		present_info.pSwapchains             = swap_chains;

		present_info.pImageIndices = imageIndex;

		// Présente l'image
		const vk::Result result = lveDevice.PresentQueue().presentKHR(&present_info);

		// Passe au prochain frame
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}


	void LveSwapChain::CreateSwapChain()
	{
		// Obtient les détails de prise en charge de la chaîne de swap
		const SwapChainSupportDetails swap_chain_support = lveDevice.GetSwapChainSupport();

		// Choix du format de surface de la chaîne de swap
		const vk::SurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);

		// Choix du mode de présentation de la chaîne de swap
		const vk::PresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.presentModes);

		// Choix de l'étendue de la chaîne de swap
		const vk::Extent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);

		// Détermine le nombre d'images dans la chaîne de swap
		uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
		if (swap_chain_support.capabilities.maxImageCount > 0 &&
		    image_count > swap_chain_support.capabilities.maxImageCount)
			image_count = swap_chain_support.capabilities.maxImageCount;

		// Configuration de la création de la chaîne de swap
		vk::SwapchainCreateInfoKHR create_info = {};
		create_info.sType                      = vk::StructureType::eSwapchainCreateInfoKHR;
		create_info.setSurface(lveDevice.Surface());

		create_info.setMinImageCount(image_count);
		create_info.setImageFormat(surface_format.format);
		create_info.setImageColorSpace(surface_format.colorSpace);
		create_info.setImageExtent(extent);
		create_info.setImageArrayLayers(1);
		create_info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		const QueueFamilyIndices indices                = lveDevice.FindPhysicalQueueFamilies();
		const uint32_t           queue_family_indices[] = {indices.graphicsFamily, indices.presentFamily};

		if (indices.graphicsFamily != indices.presentFamily)
		{
			// Si les familles de files d'attente sont différentes, partagez les images
			create_info.setImageSharingMode(vk::SharingMode::eConcurrent);
			create_info.setQueueFamilyIndexCount(2);
			create_info.setPQueueFamilyIndices(queue_family_indices);
		}
		else
		{
			// Si les familles de files d'attente sont les mêmes, utilisez exclusivement
			create_info.setImageSharingMode(vk::SharingMode::eExclusive);
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices   = nullptr;
		}

		create_info.setPreTransform(swap_chain_support.capabilities.currentTransform);
		create_info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

		create_info.setPresentMode(present_mode);
		create_info.setClipped(VK_TRUE);

		create_info.setOldSwapchain(oldSwapChain == nullptr ? nullptr : oldSwapChain->swapChain);

		// Crée la chaîne de swap
		swapChain = lveDevice.Device().createSwapchainKHR(create_info);

		// Obtient les images de la chaîne de swap
		// Récupérer le nombre d'images de swapchain
		swapChainImages = lveDevice.Device().getSwapchainImagesKHR(swapChain);
		swapChainImages.resize(image_count);

		if (lveDevice.Device().getSwapchainImagesKHR(swapChain, &image_count, swapChainImages.data()) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to create swap chain!");

		swapChainImageFormat = surface_format.format;
		swapChainExtent      = extent;
	}

	void LveSwapChain::CreateImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			vk::ImageViewCreateInfo view_info;
			view_info.sType                           = vk::StructureType::eImageViewCreateInfo;
			view_info.image                           = swapChainImages[i];
			view_info.viewType                        = vk::ImageViewType::e2D;
			view_info.format                          = swapChainImageFormat;
			view_info.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
			view_info.subresourceRange.baseMipLevel   = 0;
			view_info.subresourceRange.levelCount     = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount     = 1;

			if (lveDevice.Device().createImageView(&view_info, nullptr, &swapChainImageViews[i]) !=
			    vk::Result::eSuccess)
				throw std::runtime_error("failed!");
		}
	}


	void LveSwapChain::CreateRenderPass()
	{
		vk::AttachmentDescription depth_attachment{};
		depth_attachment.format         = FindDepthFormat();
		depth_attachment.samples        = vk::SampleCountFlagBits::e1;
		depth_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		depth_attachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		depth_attachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference depth_attachment_ref{};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentDescription color_attachment{};
		color_attachment.format         = GetSwapChainImageFormat();
		color_attachment.samples        = vk::SampleCountFlagBits::e1;
		color_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		color_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference color_attachment_ref{};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout     = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount    = 1;
		subpass.pColorAttachments       = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		vk::SubpassDependency dependency{};
		dependency.dstSubpass    = 0;
		dependency.dstAccessMask =
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		dependency.dstStageMask =
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.srcSubpass    = vk::SubpassExternal;
		dependency.srcAccessMask = vk::AccessFlags{};
		dependency.srcStageMask  =
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;

		const std::array<vk::AttachmentDescription, 2> attachments = {color_attachment, depth_attachment};
		vk::RenderPassCreateInfo                       render_pass_info{};
		render_pass_info.sType           = vk::StructureType::eRenderPassCreateInfo;
		render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		render_pass_info.pAttachments    = attachments.data();
		render_pass_info.subpassCount    = 1;
		render_pass_info.pSubpasses      = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies   = &dependency;

		if (lveDevice.Device().createRenderPass(&render_pass_info, nullptr, &renderPass) != vk::Result::eSuccess)
			throw
				std::runtime_error("failed to create render pass!");
	}


	void LveSwapChain::CreateFrameBuffers()
	{
		swapChainFramebuffers.resize(ImageCount());

		for (size_t i = 0; i < ImageCount(); i++)
		{
			std::array<vk::ImageView, 2> attachments = {swapChainImageViews[i], depthImageViews[i]};

			const vk::Extent2D        swap_chain_extent = GetSwapChainExtent();
			vk::FramebufferCreateInfo frame_buffer_info;
			frame_buffer_info.sType = vk::StructureType::eFramebufferCreateInfo;
			frame_buffer_info.setRenderPass(renderPass);
			frame_buffer_info.setAttachmentCount(attachments.size());
			frame_buffer_info.setPAttachments(attachments.data());
			frame_buffer_info.setWidth(swap_chain_extent.width);
			frame_buffer_info.setHeight(swap_chain_extent.height);
			frame_buffer_info.setLayers(1);

			if (lveDevice.Device().createFramebuffer(&frame_buffer_info, nullptr, &swapChainFramebuffers[i]) !=
			    vk::Result::eSuccess)
				throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void LveSwapChain::CreateDepthResources()
	{
		const vk::Format depth_format  = FindDepthFormat();
		swapChainDepthFormat           = depth_format;
		vk::Extent2D swap_chain_extent = GetSwapChainExtent();

		depthImages.resize(ImageCount());
		depthImageMemorys.resize(ImageCount());
		depthImageViews.resize(ImageCount());

		for (int i = 0; i < depthImages.size(); i++)
		{
			vk::ImageCreateInfo image_info;
			image_info.sType = vk::StructureType::eImageCreateInfo;
			image_info.setImageType(vk::ImageType::e2D);
			image_info.setExtent({swap_chain_extent.width, swap_chain_extent.height, 1});
			image_info.setMipLevels(1);
			image_info.setArrayLayers(1);
			image_info.setFormat(depth_format);
			image_info.setTiling(vk::ImageTiling::eOptimal);
			image_info.setInitialLayout(vk::ImageLayout::eUndefined);
			image_info.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
			image_info.setSamples(vk::SampleCountFlagBits::e1);
			image_info.setSharingMode(vk::SharingMode::eExclusive);

			lveDevice.CreateImageWithInfo(
				image_info,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				depthImages[i],
				depthImageMemorys[i]);

			vk::ImageViewCreateInfo viewInfo;
			viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
			viewInfo.setImage(depthImages[i]);
			viewInfo.setViewType(vk::ImageViewType::e2D);
			viewInfo.setFormat(depth_format);
			viewInfo.setSubresourceRange({vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

			if (lveDevice.Device().createImageView(&viewInfo, nullptr, &depthImageViews[i]) !=
			    vk::Result::eSuccess)
				throw std::runtime_error("failed to create texture image view!");
		}
	}


	void LveSwapChain::CreateSyncObjects()
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(ImageCount(), vk::Fence{});

		vk::SemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = vk::StructureType::eSemaphoreCreateInfo;

		vk::FenceCreateInfo fence_info{};
		fence_info.sType = vk::StructureType::eFenceCreateInfo;
		fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (lveDevice.Device().createSemaphore(&semaphore_info, nullptr, &imageAvailableSemaphores[i]) !=
			    vk::Result::eSuccess ||
			    lveDevice.Device().createSemaphore(&semaphore_info, nullptr, &renderFinishedSemaphores[i]) !=
			    vk::Result::eSuccess ||
			    lveDevice.Device().createFence(&fence_info, nullptr, &inFlightFences[i]) != vk::Result::eSuccess)
				throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	vk::SurfaceFormatKHR LveSwapChain::ChooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR>& _availableFormats)
	{
		for (const auto& available_format : _availableFormats)
		{
			if (available_format.format == vk::Format::eB8G8R8A8Srgb &&
			    available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return available_format;
		}

		return _availableFormats[0];
	}


	vk::PresentModeKHR LveSwapChain::ChooseSwapPresentMode(
		const std::vector<vk::PresentModeKHR>& _availablePresentModes)
	{
		/* for (const auto& availablePresentMode : availablePresentModes) {
		     if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
		         std::cout << "Present mode: Mailbox" << std::endl;
		         return availablePresentMode;
		     }
		 }*/

		for (const auto& available_present_mode : _availablePresentModes)
		{
			if (available_present_mode == vk::PresentModeKHR::eImmediate)
			{
				std::cout << "Present mode: Immediate" << std::endl;
				return available_present_mode;
			}
		}

		std::cout << "Present mode: V-Sync" << std::endl;
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D LveSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& _capabilities) const
	{
		if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) return _capabilities.
			currentExtent;
		vk::Extent2D actual_extent = windowExtent;
		actual_extent.width        = std::max(
			_capabilities.minImageExtent.width,
			std::min(_capabilities.maxImageExtent.width, actual_extent.width));
		actual_extent.height = std::max(
			_capabilities.minImageExtent.height,
			std::min(_capabilities.maxImageExtent.height, actual_extent.height));

		return actual_extent;
	}

	vk::Format LveSwapChain::FindDepthFormat() const
	{
		return lveDevice.FindSupportedFormat(
			{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}
} // namespace lve
