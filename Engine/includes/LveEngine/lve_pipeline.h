#pragma once

#include "LveEngine/lve_device.h"

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace lve
{
	/**
	 * @brief Structure contenant les informations de configuration pour la création d'un pipeline.
	 */
	struct PipelineConfigInfo
	{
		PipelineConfigInfo()                                     = default; /**< Constructeur par défaut. */
		PipelineConfigInfo(const PipelineConfigInfo&)            = delete;  /**< Constructeur de copie supprimé. */
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;  /**< Opérateur d'affectation supprimé. */

		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
		/**< Descriptions des attributs de vertex. */
		std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
		/**< Descriptions des liaisons d'entrée de vertex. */
		vk::PipelineViewportStateCreateInfo      viewportInfo; /**< Informations sur le viewport du pipeline. */
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		/**< Informations sur l'assemblage d'entrée du pipeline. */
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		/**< Informations sur la rasterisation du pipeline. */
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		/**< Informations sur le multi-échantillonnage du pipeline. */
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		/**< Attachement de mélange de couleur du pipeline. */
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		/**< Informations sur le mélange de couleur du pipeline. */
		vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
		/**< Informations sur la profondeur et le stencil du pipeline. */
		std::vector<vk::DynamicState>      dynamicStateEnables; /**< États dynamiques activés du pipeline. */
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo; /**< Informations sur l'état dynamique du pipeline. */
		vk::PipelineLayout                 pipelineLayout = nullptr; /**< Layout du pipeline. */
		vk::RenderPass                     renderPass     = nullptr; /**< Passe de rendu associée au pipeline. */
		uint32_t                           subpass        = 0; /**< Index de la sous-passe du pipeline. */
	};

	/**
	 * @brief Classe représentant un pipeline Vulkan.
	 */
	class LvePipeline
	{
		public:
			/**
			 * @brief Constructeur de la classe LvePipeline.
			 *
			 * Initialise un pipeline graphique en créant les shaders à partir des fichiers spécifiés.
			 *
			 * @param _vertFilepath Le chemin d'accès au fichier contenant le code source du shader de vertex.
			 * @param _fragFilepath Le chemin d'accès au fichier contenant le code source du shader de fragment.
			 */
			LvePipeline(LveDevice& _device, const std::string& _vertFilepath, const std::string& _fragFilepath,
			            const PipelineConfigInfo& _configInfo);

			/**
			 * @brief Destructeur de la classe LvePipeline.
			 */
			~LvePipeline();

			/**
			 * @brief Constructeur de copie supprimé.
			 */
			LvePipeline(const LvePipeline&) = delete;

			/**
			 * @brief Opérateur d'affectation supprimé.
			 */
			LvePipeline& operator=(const LvePipeline&) = delete;


			/**
			 * @brief Lie le pipeline graphique au tampon de commandes spécifié.
			 *
			 * Cette méthode lie le pipeline graphique associé à ce LvePipeline à un tampon de commandes spécifié.
			 *
			 * @param _commandBuffer Le tampon de commandes auquel le pipeline graphique doit être lié.
			 */
			void Bind(vk::CommandBuffer _commandBuffer) const;

			/**
			 * @brief Configure les paramètres par défaut du pipeline.
			 *
			 * Cette méthode configure les paramètres par défaut du pipeline en remplissant la structure PipelineConfigInfo fournie.
			 *
			 * @param _configInfo Une référence vers la structure PipelineConfigInfo à remplir avec les paramètres par défaut du pipeline.
			 */
			static void DefaultPipelineConfigInfo(PipelineConfigInfo& _configInfo);

			/**
			 * @brief Active le mélange alpha dans la configuration de pipeline spécifiée.
			 *
			 * @param _configInfo Les informations de configuration de pipeline.
			 */
			static void EnableAlphaBlending(PipelineConfigInfo& _configInfo);

		private:
			/**
			* @brief Lit le contenu d'un fichier binaire et le retourne sous forme de vecteur de caractères.
			*
			* Cette fonction ouvre un fichier binaire, lit son contenu dans un vecteur de caractères et le retourne.
			*
			* @param _filepath Le chemin d'accès au fichier à lire.
			* @return Un vecteur de caractères contenant le contenu du fichier.
			* @throws std::runtime_error si le fichier ne peut pas être ouvert.
			*/
			static std::vector<char> ReadFile(const std::string& _filepath);

			/**
			 * @brief Crée un pipeline graphique à partir des fichiers de shader spécifiés et des informations de configuration de pipeline.
			 *
			 * Cette fonction charge les shaders à partir des fichiers spécifiés, configure le pipeline graphique en utilisant les informations
			 * de configuration fournies, et crée le pipeline graphique correspondant.
			 *
			 * @param _vertFilepath Le chemin d'accès au fichier du shader de sommet.
			 * @param _fragFilepath Le chemin d'accès au fichier du shader de fragment.
			 * @param _configInfo Les informations de configuration de pipeline.
			 *
			 * @note Cette fonction doit être appelée après l'initialisation de la fenêtre et du périphérique Vulkan.
			 */
			void CreateGraphicsPipeline(const std::string&        _vertFilepath, const std::string& _fragFilepath,
			                            const PipelineConfigInfo& _configInfo);


			/**
			 * @brief Crée un module de shader à partir du code du shader spécifié.
			 *
			 * Cette méthode crée un module de shader Vulkan à partir du code du shader spécifié.
			 *
			 * @param _code Le code du shader sous forme d'un vecteur de caractères.
			 * @param _shaderModule Un pointeur vers l'emplacement où stocker le module de shader créé.
			 *
			 * @throw std::runtime_error si la création du module de shader échoue.
			 */
			void CreateShaderModule(const std::vector<char>& _code, vk::ShaderModule* _shaderModule) const;


			/**
			 * @brief Référence vers le périphérique Vulkan associé à ce pipeline.
			 */
			LveDevice& lveDevice;

			/**
			 * @brief Le pipeline graphique Vulkan.
			 */
			vk::Pipeline graphicsPipeline;

			/**
			 * @brief Le module de shader de sommet Vulkan.
			 */
			vk::ShaderModule vertShaderModule;

			/**
			 * @brief Le module de shader de fragment Vulkan.
			 */
			vk::ShaderModule fragShaderModule;
	};
} //namespace lve
