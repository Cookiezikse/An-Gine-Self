#include "LveEngine/lve_model.h"

#include "LveEngine/lve_utils.h"

//lives
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

//std
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#ifndef ENGINE_DIR
#define ENGINE_DIR ""
#endif


namespace std
{
	template <>
	struct hash<lve::LveModel::Vertex>
	{
		size_t operator()(const lve::LveModel::Vertex& _vertex) const
		{
			size_t seed = 0;
			lve::HashCombine(seed, _vertex.position, _vertex.color, _vertex.normal, _vertex.uv);
			return seed;
		}
	};
}

namespace lve
{
	/*struct Vertex {
		glm::vec2 position;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();
	};*/

	LveModel::LveModel(LveDevice& _lveDevice, const Builder& _builder) : lveDevice(_lveDevice)
	{
		CreateVertexBuffer(_builder.vertices);
		CreateIndexBuffer(_builder.indices);
	}

	LveModel::~LveModel()
	{
	}


	std::unique_ptr<LveModel> LveModel::CreateModelFromFile(LveDevice& _device, const std::string& _filepath, float _texMultiplier, glm::vec3 _color)
	{
		Builder builder{};
		builder.LoadModel(ENGINE_DIR + _filepath, _texMultiplier, _color);
		std::cout << "Vertex Count" << builder.vertices.size() << std::endl;

		return std::make_unique<LveModel>(_device, builder);
	}


	void LveModel::Bind(const vk::CommandBuffer _commandBuffer) const
	{
		const vk::Buffer         buffers[] = {vertexBuffer->GetBuffer()};
		constexpr vk::DeviceSize offsets[] = {0};
		_commandBuffer.bindVertexBuffers(0, 1, buffers, offsets);

		if (hasIndexBuffer) _commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
	}


	void LveModel::Draw(const vk::CommandBuffer _commandBuffer) const
	{
		if (hasIndexBuffer) _commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
		else _commandBuffer.draw(vertexCount, 1, 0, 0);
	}


	void LveModel::CreateVertexBuffer(const std::vector<Vertex>& _vertices)
	{
		vertexCount = static_cast<uint32_t>(_vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		const vk::DeviceSize buffer_size = sizeof(_vertices[0]) * vertexCount;
		uint32_t             vertex_size = sizeof(_vertices[0]);

		// Déclaration et initialisation d'un objet de type LveBuffer nommé stagingBuffer
		LveBuffer staging_buffer{
			// Paramètre représentant le périphérique graphique associé au tampon
			lveDevice,
			// Taille des données de vertex à stocker dans le tampon
			vertex_size,
			// Nombre total de vertices dans les données
			vertexCount,
			// Indique que le tampon sera utilisé comme une source lors des opérations de transfert de données
			vk::BufferUsageFlagBits::eTransferSrc,
			// Propriétés de la mémoire dans laquelle le tampon sera alloué :
			// - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : La mémoire est visible pour le CPU
			// - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Les écritures CPU seront visibles par le GPU sans nécessiter d'opérations de synchronisation explicites
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};

		staging_buffer.Map();
		staging_buffer.WriteToBuffer(_vertices.data());

		// Allocation dynamique d'un objet de type unique_ptr pointant vers un LveBuffer et initialisation avec make_unique
		vertexBuffer = std::make_unique<LveBuffer>(
			// Paramètre représentant le périphérique graphique associé au tampon
			lveDevice,
			// Taille des données de vertex à stocker dans le tampon
			vertex_size,
			// Nombre total de vertices dans les données
			vertexCount,
			// Combinaison de drapeaux d'utilisation du tampon :
			// - VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : Le tampon est utilisé comme tampon de vertex
			// - VK_BUFFER_USAGE_TRANSFER_DST_BIT : Le tampon peut être utilisé comme destination lors des opérations de transfert de données
			// - VK_BUFFER_USAGE_INDEX_BUFFER_BIT : Le tampon est utilisé comme tampon d'index
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst |
			vk::BufferUsageFlagBits::eIndexBuffer,
			// Propriétés de la mémoire dans laquelle le tampon sera alloué :
			// - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : La mémoire est locale au périphérique et n'est pas visible pour le CPU
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		lveDevice.CopyBuffer(staging_buffer.GetBuffer(), vertexBuffer->GetBuffer(), buffer_size);
	}


	void LveModel::CreateIndexBuffer(const std::vector<uint32_t>& _indices)
	{
		// Détermine le nombre d'indices et vérifie s'il y en a au moins un
		indexCount     = static_cast<uint32_t>(_indices.size());
		hasIndexBuffer = indexCount > 0;

		// Si aucun indice n'est présent, la fonction se termine
		if (!hasIndexBuffer) return;

		// Calcul de la taille du tampon en bytes
		const vk::DeviceSize buffer_size = sizeof(_indices[0]) * indexCount;
		uint32_t             index_size  = sizeof(_indices[0]);


		// Création d'un objet de type LveBuffer nommé stagingBuffer en utilisant une initialisation directe
		// Création du tampon de transfert pour les données d'indices
		LveBuffer stagingBuffer{
			// Paramètre représentant le périphérique graphique associé au tampon
			lveDevice,
			// Taille des données d'index à stocker dans le tampon
			index_size,
			// Nombre total d'indices dans les données
			indexCount,
			vk::BufferUsageFlagBits::eTransferSrc,
			// Indique que le tampon sera utilisé comme une source lors des opérations de transfert de données
			// Propriétés de la mémoire dans laquelle le tampon sera alloué :
			// - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : La mémoire est visible pour le CPU
			// - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : Les écritures CPU seront visibles par le GPU sans nécessiter d'opérations de synchronisation explicites
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		};

		// Mappage du tampon de transfert et écriture des données
		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(_indices.data());

		// Allocation dynamique d'un objet unique_ptr pointant vers un LveBuffer et initialisation avec make_unique
		// Création du tampon d'indices sur le GPU
		indexBuffer = std::make_unique<LveBuffer>(
			// Paramètre représentant le périphérique graphique associé au tampon
			lveDevice,
			// Taille des données d'index à stocker dans le tampon
			index_size,
			// Nombre total d'indices dans les données
			indexCount,
			// Combinaison de drapeaux d'utilisation du tampon :
			// - VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : Le tampon est utilisé comme tampon de vertex
			// - VK_BUFFER_USAGE_TRANSFER_DST_BIT : Le tampon peut être utilisé comme destination lors des opérations de transfert de données
			// - VK_BUFFER_USAGE_INDEX_BUFFER_BIT : Le tampon est utilisé comme tampon d'index
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst |
			vk::BufferUsageFlagBits::eIndexBuffer,
			// Propriétés de la mémoire dans laquelle le tampon sera alloué :
			// - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : La mémoire est locale au périphérique et n'est pas visible pour le CPU
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		// Copie des données de l'indice du tampon de transfert vers le tampon d'indices du GPU
		lveDevice.CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), buffer_size);
	}


	std::vector<vk::VertexInputBindingDescription> LveModel::Vertex::GetBindingDescriptions()
	{
		std::vector<vk::VertexInputBindingDescription> binding_descriptions(1);
		binding_descriptions[0].setBinding(0);
		binding_descriptions[0].setStride(sizeof(Vertex));
		binding_descriptions[0].setInputRate(vk::VertexInputRate::eVertex);
		return binding_descriptions;
	}

	std::vector<vk::VertexInputAttributeDescription> LveModel::Vertex::GetAttributeDescriptions()
	{
		std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;

		attribute_descriptions.push_back({
			0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(Vertex, position))
		});
		attribute_descriptions.push_back({
			1, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(Vertex, color))
		});
		attribute_descriptions.push_back({
			2, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(Vertex, normal))
		});
		attribute_descriptions.push_back({
			3, 0, vk::Format::eR32G32Sfloat, static_cast<uint32_t>(offsetof(Vertex, uv))
		});

		return attribute_descriptions;
	}

	void LveModel::Builder::LoadModel(const std::string& _filepath, float _texMultiplier = 1, glm::vec3 _color)
	{
		tinyobj::attrib_t                attrib;
		std::vector<tinyobj::shape_t>    shapes;
		std::vector<tinyobj::material_t> materials;
		std::string                      warn, err;

		if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, _filepath.c_str()))
			throw
				std::runtime_error(warn + err);

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> unique_vertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};
					
					if (_color.r == -1 && _color.g == -1 && _color.b == -1) {
						vertex.color = {
							attrib.colors[3 * index.vertex_index + 0],
							attrib.colors[3 * index.vertex_index + 1],
							attrib.colors[3 * index.vertex_index + 2],
						};
					}
					else {
						vertex.color = _color;
					}
				}

				if (index.normal_index >= 0)
					vertex.normal = {
						
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};

				if (index.texcoord_index >= 0)
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0] * _texMultiplier,
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1] * _texMultiplier,
					};

				if (!unique_vertices.contains(vertex))
				{
					unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(unique_vertices[vertex]);
			}
		}
	}
} //namespace lve
