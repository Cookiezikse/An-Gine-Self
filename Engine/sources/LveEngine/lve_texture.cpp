#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <iostream>

#include "LveEngine/lve_texture.h"
#include "LveEngine/lve_buffer.h"

bool fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}

namespace lve {
	LveTexture::LveTexture(LveDevice& _device, const std::string& filepath) : lveDevice(_device) {
		int channels;
		int bytesPerPixel;

		std::string filePath = filepath;
		if (fileExists(filePath)) {
			std::cout << "File exists at path: " << filePath << std::endl;
		}
		else {
			std::cout << "File does not exist at path: " << filePath << std::endl;
		}

		stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, 4);

		mipLevels = std::floor(std::log2(std::max(width, height))) + 1;

		LveBuffer stagingBuffer(lveDevice, 4,
			static_cast<uint32_t>(width * height),
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(data);

		imageFormat = vk::Format::eR8G8B8A8Srgb;

		vk::ImageCreateInfo imageInfo{};
		imageInfo.sType = vk::StructureType::eImageCreateInfo;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.format = imageFormat;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.setExtent({ static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 });
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;

		lveDevice.CreateImageWithInfo(imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, image, imageMemory);

		transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

		lveDevice.CopyBufferToImage(stagingBuffer.GetBuffer(), image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1);

		generateMipmaps();

		imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		vk::SamplerCreateInfo samplerInfo{};
		samplerInfo.sType = vk::StructureType::eSamplerCreateInfo;
		samplerInfo.magFilter = vk::Filter::eLinear;
		samplerInfo.minFilter = vk::Filter::eLinear;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.compareOp = vk::CompareOp::eNever;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.maxAnisotropy = 4.0;
		samplerInfo.anisotropyEnable = vk::True;
		samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;

		lveDevice.Device().createSampler(&samplerInfo, nullptr, &sampler);

		vk::ImageViewCreateInfo imageViewInfo{};
		imageViewInfo.sType = vk::StructureType::eImageViewCreateInfo;
		imageViewInfo.viewType = vk::ImageViewType::e2D;
		imageViewInfo.format = imageFormat;
		imageViewInfo.components = vk::ComponentMapping();
		imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;
		imageViewInfo.subresourceRange.levelCount = mipLevels;
		imageViewInfo.image = image;

		lveDevice.Device().createImageView(&imageViewInfo, nullptr, &imageView);

		stbi_image_free(data);

	}

	LveTexture::~LveTexture() {
		std::cout << "Destroyed !!";
		
		lveDevice.Device().destroyImage(image, nullptr);
		lveDevice.Device().freeMemory(imageMemory, nullptr);
		lveDevice.Device().destroyImageView(imageView, nullptr);
		lveDevice.Device().destroySampler(sampler, nullptr);
	}

	void LveTexture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
		vk::CommandBuffer commandBuffer = lveDevice.BeginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier{};
		barrier.sType = vk::StructureType::eImageMemoryBarrier;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}

		else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else {
			throw std::runtime_error("unsuppported layout transition!");
		}

		commandBuffer.pipelineBarrier(
			sourceStage,                      // Source pipeline stage
			destinationStage,                 // Destination pipeline stage
			vk::DependencyFlags{},            // Dependency flags
			nullptr,                          // Memory barriers
			nullptr,                          // Buffer memory barriers
			barrier                          // Image memory barriers
		);

		lveDevice.EndSingleTimeCommands(commandBuffer);
	}

	void LveTexture::generateMipmaps() {
		vk::FormatProperties formatProperties;
		lveDevice.GetPhysicalDevice().getFormatProperties(imageFormat, &formatProperties);
		
		if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		vk::CommandBuffer commandBuffer = lveDevice.BeginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier{};
		barrier.sType = vk::StructureType::eImageMemoryBarrier;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;

		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			commandBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,                      // srcStageMask
				vk::PipelineStageFlagBits::eTransfer,                      // dstStageMask
				vk::DependencyFlags{},                                     // dependencyFlags
				nullptr,                                                   // memoryBarriers
				nullptr,                                                   // bufferMemoryBarriers
				barrier,                                                   // imageMemoryBarriers
				{}                                                         // dispatch
			);

			vk::ImageBlit blit{};
			blit.srcOffsets[0].x = 0;
			blit.srcOffsets[0].y = 0;
			blit.srcOffsets[0].z = 0;
			blit.srcOffsets[1].x = mipWidth;
			blit.srcOffsets[1].y = mipHeight;
			blit.srcOffsets[1].z = 1;
			blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0].x = 0;
			blit.dstOffsets[0].y = 0;
			blit.dstOffsets[0].z = 0;
			blit.dstOffsets[1].x = mipWidth > 1 ? mipWidth / 2 : 1;
			blit.dstOffsets[1].y = mipHeight > 1 ? mipHeight / 2 : 1;
			blit.dstOffsets[1].z = 1;
			blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			commandBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,                      // srcStageMask
				vk::PipelineStageFlagBits::eFragmentShader,                      // dstStageMask
				vk::DependencyFlags{},                                     // dependencyFlags
				nullptr,                                                   // memoryBarriers
				nullptr,                                                   // bufferMemoryBarriers
				barrier,                                                   // imageMemoryBarriers
				{}                                                         // dispatch
			);
			
			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;

		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,                      // srcStageMask
			vk::PipelineStageFlagBits::eFragmentShader,                      // dstStageMask
			vk::DependencyFlags{},                                     // dependencyFlags
			nullptr,                                                   // memoryBarriers
			nullptr,                                                   // bufferMemoryBarriers
			barrier,                                                   // imageMemoryBarriers
			{}                                                         // dispatch
		);

		lveDevice.EndSingleTimeCommands(commandBuffer);

	}

}