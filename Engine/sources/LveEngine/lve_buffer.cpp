#include "LveEngine/lve_buffer.h"
/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */


// std
#include <cassert>
#include <cstring>

namespace lve
{
	/**
	 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
	 *
	 * @param _instanceSize
	 * @param _minOffsetAlignment
	 * @param instanceSize The size of an instance
	 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
	 * minUniformBufferOffsetAlignment)
	 *
	 * @return vk::Result of the buffer mapping call
	 */
	vk::DeviceSize LveBuffer::GetAlignment(const vk::DeviceSize _instanceSize, const vk::DeviceSize _minOffsetAlignment)
	{
		if (_minOffsetAlignment > 0) return (_instanceSize + _minOffsetAlignment - 1) & ~(_minOffsetAlignment - 1);
		return _instanceSize;
	}

	LveBuffer::LveBuffer(
		LveDevice&                    _device,
		const vk::DeviceSize          _instanceSize,
		const uint32_t                _instanceCount,
		const vk::BufferUsageFlags    _usageFlags,
		const vk::MemoryPropertyFlags _memoryPropertyFlags,
		const vk::DeviceSize          _minOffsetAlignment) : lveDevice{_device},
		                                            instanceCount{_instanceCount},
		                                            instanceSize{_instanceSize},
		                                            usageFlags{_usageFlags},
		                                            memoryPropertyFlags{_memoryPropertyFlags}
	{
		alignmentSize = GetAlignment(_instanceSize, _minOffsetAlignment);
		bufferSize    = alignmentSize * instanceCount;
		_device.CreateBuffer(bufferSize, usageFlags, _memoryPropertyFlags, buffer, memory);
	}

	LveBuffer::~LveBuffer()
	{
		Unmap();
		lveDevice.Device().destroyBuffer(buffer, nullptr);
		lveDevice.Device().freeMemory(memory, nullptr);
	}

	/**
	 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	 *
	 * @param _size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
	 * buffer range.
	 * @param _offset (Optional) Byte offset from beginning
	 *
	 * @return vk::Result of the buffer mapping call
	 */
	vk::Result LveBuffer::Map(const vk::DeviceSize _size, const vk::DeviceSize _offset)
	{
		assert(buffer && memory && "Called map on buffer before create");
		return lveDevice.Device().mapMemory(memory, _offset, _size, vk::MemoryMapFlags(), &mapped);
	}

	/**
	 * Unmap a mapped memory range
	 *
	 * @note Does not return a result as vkUnmapMemory can't fail
	 */
	void LveBuffer::Unmap()
	{
		if (mapped)
		{
			lveDevice.Device().unmapMemory(memory);
			mapped = nullptr;
		}
	}

	/**
	 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
	 *
	 * @param _data Pointer to the data to copy
	 * @param _size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
	 * range.
	 * @param _offset (Optional) Byte offset from beginning of mapped region
	 *
	 */
	void LveBuffer::WriteToBuffer(const void* _data, const vk::DeviceSize _size, const vk::DeviceSize _offset) const
	{
		assert(mapped && "Cannot copy to unmapped buffer");

		if (_size == VK_WHOLE_SIZE)
		{
			memcpy(mapped, _data, bufferSize);
		}
		else
		{
			auto mem_offset = static_cast<char*>(mapped);
			mem_offset += _offset;
			memcpy(mem_offset, _data, _size);
		}
	}

	/**
	 * Flush a memory range of the buffer to make it visible to the device
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param _size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
	 * complete buffer range.
	 * @param _offset (Optional) Byte offset from beginning
	 *
	 * @return vk::Result of the flush call
	 */
	vk::Result LveBuffer::Flush(const vk::DeviceSize _size, const vk::DeviceSize _offset) const
	{
		vk::MappedMemoryRange mapped_range{};
		mapped_range.memory = memory;
		mapped_range.offset = _offset;
		mapped_range.size   = _size;
		return lveDevice.Device().flushMappedMemoryRanges(1, &mapped_range);
	}

	/**
	 * Invalidate a memory range of the buffer to make it visible to the host
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param _size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
	 * the complete buffer range.
	 * @param _offset (Optional) Byte offset from beginning
	 *
	 * @return vk::Result of the invalidate call
	 */
	vk::Result LveBuffer::Invalidate(const vk::DeviceSize _size, const vk::DeviceSize _offset) const
	{
		vk::MappedMemoryRange mapped_range{};
		mapped_range.memory = memory;
		mapped_range.offset = _offset;
		mapped_range.size   = _size;
		return lveDevice.Device().invalidateMappedMemoryRanges(1, &mapped_range);
	}

	/**
	 * Create a buffer info descriptor
	 *
	 * @param _size (Optional) Size of the memory range of the descriptor
	 * @param _offset (Optional) Byte offset from beginning
	 *
	 * @return vk::DescriptorBufferInfo of specified offset and range
	 */
	vk::DescriptorBufferInfo LveBuffer::DescriptorInfo(const vk::DeviceSize _size, const vk::DeviceSize _offset) const
	{
		return vk::DescriptorBufferInfo{
			buffer,
			_offset,
			_size,
		};
	}

	/**
	 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
	 *
	 * @param _data Pointer to the data to copy
	 * @param _index Used in offset calculation
	 *
	 */
	void LveBuffer::WriteToIndex(void* _data, const int _index)
	{
		WriteToBuffer(_data, instanceSize, _index * alignmentSize);
	}

	/**
	 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
	 *
	 * @param _index Used in offset calculation
	 *
	 */
	vk::Result LveBuffer::FlushIndex(const int _index) { return Flush(alignmentSize, _index * alignmentSize); }

	/**
	 * Create a buffer info descriptor
	 *
	 * @param _index Specifies the region given by index * alignmentSize
	 *
	 * @return vk::DescriptorBufferInfo for instance at index
	 */
	vk::DescriptorBufferInfo LveBuffer::DescriptorInfoForIndex(const int _index)
	{
		return DescriptorInfo(alignmentSize, _index * alignmentSize);
	}

	/**
	 * Invalidate a memory range of the buffer to make it visible to the host
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param _index Specifies the region to invalidate: index * alignmentSize
	 *
	 * @return vk::Result of the invalidate call
	 */
	vk::Result LveBuffer::InvalidateIndex(const int _index)
	{
		return Invalidate(alignmentSize, _index * alignmentSize);
	}
} // namespace lve
