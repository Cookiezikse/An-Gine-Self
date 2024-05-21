#pragma once

#include <vulkan/vulkan.hpp>
#include "LveEngine/lve_device.h"

namespace lve
{
	/**
	 * @brief Classe représentant un tampon Vulkan.
	 */
	class LveBuffer
	{
		public:
			LveBuffer(
				LveDevice&              _device,
				vk::DeviceSize          _instanceSize,
				uint32_t                _instanceCount,
				vk::BufferUsageFlags    _usageFlags,
				vk::MemoryPropertyFlags _memoryPropertyFlags,
				vk::DeviceSize          _minOffsetAlignment = 1); /**< Constructeur de LveBuffer. */

			~LveBuffer(); /**< Destructeur de LveBuffer. */

			LveBuffer(const LveBuffer&)            = delete; /**< Constructeur de copie supprimé. */
			LveBuffer& operator=(const LveBuffer&) = delete; /**< Opérateur d'affectation supprimé. */


			/**
			 * @brief Mappe le tampon en mémoire.
			 *
			 * @param _size La taille du tampon à mapper. Par défaut, toute la taille du tampon est mappée.
			 * @param _offset Le décalage à partir du début du tampon pour commencer le mappage. Par défaut, le mappage commence au début du tampon.
			 *
			 * @return vk::Result indiquant le succès ou l'échec de l'opération de mappage.
			 */
			vk::Result Map(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0);

			/**
			 * @brief Désalloue le tampon mappé en mémoire.
			 */
			void Unmap();

			/**
			 * @brief Écrit les données dans le tampon.
			 *
			 * @param _data Pointeur vers les données à écrire dans le tampon.
			 * @param _size La taille des données à écrire dans le tampon. Par défaut, toute la taille des données est écrite.
			 * @param _offset Le décalage à partir du début du tampon où écrire les données. Par défaut, les données sont écrites à partir du début du tampon.
			 */
			void WriteToBuffer(const void*    _data, vk::DeviceSize _size = VK_WHOLE_SIZE,
			                   vk::DeviceSize _offset                     = 0) const;

			/**
			 * @brief Flushes the memory-mapped buffer range.
			 *
			 * @param _size La taille de la plage du tampon à flusher. Par défaut, toute la taille du tampon est flushée.
			 * @param _offset Le décalage à partir du début du tampon pour commencer le flush. Par défaut, le flush commence au début du tampon.
			 *
			 * @return vk::Result indiquant le succès ou l'échec de l'opération de flush.
			 */
			vk::Result Flush(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0) const;

			/**
			 * @brief Récupère les informations de tampon de descripteur.
			 *
			 * @param _size La taille des données à inclure dans les informations de descripteur. Par défaut, toute la taille des données est incluse.
			 * @param _offset Le décalage à partir du début du tampon pour inclure dans les informations de descripteur. Par défaut, les informations de descripteur incluent le début du tampon.
			 *
			 * @return vk::DescriptorBufferInfo contenant les informations de tampon de descripteur.
			 */
			vk::DescriptorBufferInfo DescriptorInfo(vk::DeviceSize _size   = VK_WHOLE_SIZE,
			                                        vk::DeviceSize _offset = 0) const;

			/**
			 * @brief Invalidate the memory-mapped buffer range.
			 *
			 * @param _size La taille de la plage du tampon à invalider. Par défaut, toute la taille du tampon est invalidée.
			 * @param _offset Le décalage à partir du début du tampon pour commencer l'invalidation. Par défaut, l'invalidation commence au début du tampon.
			 *
			 * @return vk::Result indiquant le succès ou l'échec de l'opération d'invalidation.
			 */
			vk::Result Invalidate(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0) const;

			/**
			 * @brief Écrit les données dans l'index spécifié du tampon.
			 *
			 * @param _data Pointeur vers les données à écrire dans le tampon.
			 * @param _index L'index dans le tampon où écrire les données.
			 */
			void WriteToIndex(void* _data, int _index);

			/**
			 * @brief Flushes the memory-mapped buffer range for the specified index.
			 *
			 * @param _index L'index du tampon pour lequel effectuer le flush.
			 *
			 * @return vk::Result indiquant le succès ou l'échec de l'opération de flush.
			 */
			vk::Result FlushIndex(int _index);

			/**
			 * @brief Récupère les informations de tampon de descripteur pour l'index spécifié.
			 *
			 * @param _index L'index du tampon pour lequel récupérer les informations de descripteur.
			 *
			 * @return vk::DescriptorBufferInfo contenant les informations de tampon de descripteur pour l'index spécifié.
			 */
			vk::DescriptorBufferInfo DescriptorInfoForIndex(int _index);

			/**
			 * @brief Invalide la plage du tampon pour l'index spécifié.
			 *
			 * @param _index L'index du tampon pour lequel effectuer l'invalidation.
			 *
			 * @return vk::Result indiquant le succès ou l'échec de l'opération d'invalidation.
			 */
			vk::Result InvalidateIndex(int _index);

			/**
			 * @brief Récupère le tampon Vulkan associé à l'objet LveBuffer.
			 *
			 * @return vk::Buffer contenant le tampon Vulkan.
			 */
			vk::Buffer GetBuffer() const { return buffer; }

			/**
			 * @brief Récupère le pointeur vers la mémoire mappée du tampon.
			 *
			 * @return Pointeur vers la mémoire mappée du tampon.
			 */
			void* GetMappedMemory() const { return mapped; }

			/**
			 * @brief Récupère le nombre d'instances dans le tampon.
			 *
			 * @return Le nombre d'instances dans le tampon.
			 */
			uint32_t GetInstanceCount() const { return instanceCount; }

			/**
			 * @brief Récupère la taille de chaque instance dans le tampon.
			 *
			 * @return La taille de chaque instance dans le tampon.
			 */
			vk::DeviceSize GetInstanceSize() const { return instanceSize; }

			/**
			 * @brief Récupère la taille d'alignement du tampon.
			 *
			 * @return La taille d'alignement du tampon.
			 */
			vk::DeviceSize GetAlignmentSize() const { return instanceSize; }

			/**
			 * @brief Récupère les indicateurs d'utilisation du tampon.
			 *
			 * @return vk::BufferUsageFlags contenant les indicateurs d'utilisation du tampon.
			 */
			vk::BufferUsageFlags GetUsageFlags() const { return usageFlags; }

			/**
			 * @brief Récupère les propriétés de la mémoire du tampon.
			 *
			 * @return vk::MemoryPropertyFlags contenant les propriétés de la mémoire du tampon.
			 */
			vk::MemoryPropertyFlags GetMemoryPropertyFlags() const { return memoryPropertyFlags; }

			/**
			 * @brief Récupère la taille totale du tampon.
			 *
			 * @return La taille totale du tampon.
			 */
			vk::DeviceSize GetBufferSize() const { return bufferSize; }

		private:
			/**
			 * @brief Obtient la taille d'alignement requise pour une taille d'instance spécifiée.
			 *
			 * @param _instanceSize La taille de l'instance.
			 * @param _minOffsetAlignment La taille d'alignement minimale.
			 * @return La taille d'alignement requise.
			 */
			static vk::DeviceSize GetAlignment(vk::DeviceSize _instanceSize, vk::DeviceSize _minOffsetAlignment);

			/** @brief Référence vers l'objet LveDevice associé à ce tampon. */
			LveDevice& lveDevice;

			/** @brief Pointeur vers la mémoire mappée du tampon. */
			void* mapped = nullptr;

			/** @brief Le tampon Vulkan. */
			vk::Buffer buffer = VK_NULL_HANDLE;

			/** @brief La mémoire du périphérique associée au tampon. */
			vk::DeviceMemory memory = VK_NULL_HANDLE;

			/** @brief La taille du tampon. */
			vk::DeviceSize bufferSize;

			/** @brief Le nombre d'instances dans le tampon. */
			uint32_t instanceCount;

			/** @brief La taille d'une instance dans le tampon. */
			vk::DeviceSize instanceSize;

			/** @brief La taille d'alignement pour le tampon. */
			vk::DeviceSize alignmentSize;

			/** @brief Les indicateurs d'utilisation du tampon. */
			vk::BufferUsageFlags usageFlags;

			/** @brief Les indicateurs de propriétés de mémoire du tampon. */
			vk::MemoryPropertyFlags memoryPropertyFlags;
	};
} // namespace lve
