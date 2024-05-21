#pragma once

#include "Camera/lve_camera.h"

//lib
#include <vulkan/vulkan.hpp>

namespace lve
{
	#define MAX_LIGHTS 10

	/**
	 * @brief Représente une lumière ponctuelle dans l'espace 3D.
	 */
	struct PointLight
	{
		glm::vec4 position{}; /**< Position de la lumière. */
		glm::vec4 color{};    /**< Couleur de la lumière. */
	};

	/**
	 * @brief Représente les données uniformes globales utilisées dans le pipeline de rendu.
	 */
	struct GlobalUbo
	{
		glm::mat4  projection{1.f};                        /**< Matrice de projection. */
		glm::mat4  view{1.f};                              /**< Matrice de vue. */
		glm::mat4  inverseView{1.f};                       /**< Matrice de vue inverse. */
		glm::vec4  ambientLightColor{1.f, 1.f, 1.f, .02f}; /**< Couleur de lumière ambiante. */
		PointLight pointLights[MAX_LIGHTS];                /**< Tableau des lumières ponctuelles. */
		int        numLights;                              /**< Nombre de lumières ponctuelles. */
	};

	/**
	 * @brief Contient les informations de trame utilisées lors du rendu.
	 */
	struct FrameInfo
	{
		int                 frameIndex;          /**< Index de la trame. */
		float               frameTime;           /**< Temps de la trame. */
		vk::CommandBuffer   commandBuffer;       /**< Tampon de commandes Vulkan. */
		LveCamera&          camera;              /**< Caméra utilisée pour le rendu. */
		vk::DescriptorSet   globalDescriptorSet; /**< Ensemble de descripteurs global. */
		vk::DescriptorSet   tex1DescriptorSet;
		vk::DescriptorSet   tex2DescriptorSet;
	};
} // namespace lve
