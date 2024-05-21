#pragma once

#include "LveEngine/lve_device.h"

#include <vulkan/vulkan.hpp>

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve
{
	/**
	 * @brief Classe représentant un agencement de jeu de descripteurs Vulkan.
	 */
	class LveDescriptorSetLayout
	{
		public:
			class Builder
			{
				public:
					/**
					 * @brief Constructeur de la classe Builder.
					 *
					 * @param _lveDevice Une référence vers l'objet LveDevice utilisé pour la création de la mise en page de l'ensemble de descripteurs.
					 */
					explicit Builder(LveDevice& _lveDevice) : lveDevice{_lveDevice}
					{
					}

					/**
					 * @brief Ajoute un binding à la mise en page de l'ensemble de descripteurs.
					 *
					 * @param _binding L'index du binding.
					 * @param _descriptorType Le type de descripteur.
					 * @param _stageFlags Les indicateurs d'étape de shader.
					 * @param _count Le nombre d'éléments dans le binding.
					 * @return Une référence vers l'objet Builder actuel.
					 */
					Builder& AddBinding(uint32_t             _binding, vk::DescriptorType _descriptorType,
					                    vk::ShaderStageFlags _stageFlags, uint32_t        _count = 1);

					/**
					 * @brief Construit et retourne un ensemble de descripteurs avec la mise en page spécifiée.
					 *
					 * @return Un pointeur unique vers l'objet LveDescriptorSetLayout nouvellement créé.
					 */
					std::unique_ptr<LveDescriptorSetLayout> Build() const;

				private:
					LveDevice& lveDevice;
					/**< Une référence vers l'objet LveDevice utilisé pour la création de la mise en page de l'ensemble de descripteurs. */
					std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings{};
					/**< Les bindings ajoutés à la mise en page de l'ensemble de descripteurs. */
			};


			/**
			 * @brief Constructeur de la classe LveDescriptorSetLayout.
			 *
			 * @param _lveDevice Une référence vers l'objet LveDevice utilisé pour la création de l'ensemble de descripteurs.
			 * @param _bindings Les bindings de la mise en page de l'ensemble de descripteurs.
			 */
			LveDescriptorSetLayout(LveDevice&                                                   _lveDevice,
			                       std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> _bindings);

			/**
			 * @brief Destructeur de la classe LveDescriptorSetLayout.
			 */
			~LveDescriptorSetLayout();

			/**
			 * @brief Constructeur de copie supprimé.
			 */
			LveDescriptorSetLayout(const LveDescriptorSetLayout&) = delete;

			/**
			 * @brief Opérateur d'affectation supprimé.
			 */
			LveDescriptorSetLayout& operator=(const LveDescriptorSetLayout&) = delete;


			/**
			 * @brief Obtient la mise en page de l'ensemble de descripteurs.
			 *
			 * @return La mise en page de l'ensemble de descripteurs.
			 */
			vk::DescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

		private:
			LveDevice& lveDevice; /**< Référence à l'objet LveDevice associé. */
			vk::DescriptorSetLayout descriptorSetLayout; /**< Mise en page de l'ensemble de descripteurs Vulkan. */
			std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;
			/**< Map des bindings de l'ensemble de descripteurs. */

			friend class LveDescriptorWriter; /**< Déclaration d'amitié avec la classe LveDescriptorWriter. */
	};

	class LveDescriptorPool
	{
		public:
			class Builder
			{
				public:
					/**
		 * @brief Constructeur de la classe Builder.
		 *
		 * Initialise un nouvel objet Builder avec une référence à l'objet LveDevice spécifié.
		 *
		 * @param _lveDevice Référence à l'objet LveDevice associé à ce builder.
		 */
					explicit Builder(LveDevice& _lveDevice) : lveDevice{_lveDevice}
					{
					}

					/**
					 * @brief Ajoute une taille de pool de descripteurs au builder.
					 *
					 * Cette fonction ajoute une taille de pool de descripteurs au builder.
					 * Elle spécifie le type de descripteur et le nombre de descripteurs à allouer dans le pool.
					 *
					 * @param _descriptorType Le type de descripteur à ajouter.
					 * @param _count Le nombre de descripteurs à allouer dans le pool pour le type spécifié.
					 * @return Une référence à cet objet Builder pour permettre les appels de méthode en chaîne.
					 */
					Builder& AddPoolSize(vk::DescriptorType _descriptorType, uint32_t _count);

					/**
					 * @brief Définit les indicateurs de création du pool de descripteurs.
					 *
					 * Cette fonction définit les indicateurs de création du pool de descripteurs.
					 * Elle spécifie les options de création pour le pool de descripteurs.
					 *
					 * @param _flags Les indicateurs de création du pool de descripteurs.
					 * @return Une référence à cet objet Builder pour permettre les appels de méthode en chaîne.
					 */
					Builder& SetPoolFlags(vk::DescriptorPoolCreateFlags _flags);

					/**
					 * @brief Définit le nombre maximal d'ensembles de descripteurs pouvant être alloués à partir du pool.
					 *
					 * Cette fonction définit le nombre maximal d'ensembles de descripteurs pouvant être alloués à partir du pool.
					 * Elle spécifie le nombre maximal d'ensembles de descripteurs qui peuvent être alloués à partir du pool de descripteurs.
					 *
					 * @param _count Le nombre maximal d'ensembles de descripteurs pouvant être alloués à partir du pool.
					 * @return Une référence à cet objet Builder pour permettre les appels de méthode en chaîne.
					 */
					Builder& SetMaxSets(uint32_t _count);

					/**
					 * @brief Construit et retourne un objet LveDescriptorPool.
					 *
					 * Cette fonction construit et retourne un objet LveDescriptorPool avec les configurations spécifiées précédemment.
					 *
					 * @return Un unique_ptr pointant vers l'objet LveDescriptorPool nouvellement créé.
					 */
					[[nodiscard]] std::unique_ptr<LveDescriptorPool> Build() const;

				private:
					LveDevice& lveDevice; /**< Référence à l'objet LveDevice associé à ce pool de descripteurs. */
					uint32_t   maxSets;
					/**< Nombre maximal d'ensembles de descripteurs pouvant être alloués à partir du pool. */
					vk::DescriptorPoolCreateFlags poolFlags; /**< Indicateurs de création du pool de descripteurs. */
					std::vector<vk::DescriptorPoolSize> poolSizes;
					/**< Vecteur contenant les tailles de pool de descripteurs. */
			};

			/**
			 * @brief Constructeur de LveDescriptorPool.
			 *
			 * @param _lveDevice Référence à l'objet LveDevice associé à ce pool de descripteurs.
			 * @param _maxSets Nombre maximal d'ensembles de descripteurs pouvant être alloués à partir du pool.
			 * @param _poolFlags Indicateurs de création du pool de descripteurs.
			 * @param _poolSizes Vecteur contenant les tailles de pool de descripteurs.
			 */
			LveDescriptorPool(
				LveDevice&                                 _lveDevice,
				uint32_t                                   _maxSets,
				vk::DescriptorPoolCreateFlags              _poolFlags,
				const std::vector<vk::DescriptorPoolSize>& _poolSizes);

			/**
			 * @brief Destructeur de LveDescriptorPool.
			 */
			~LveDescriptorPool();

			LveDescriptorPool(const LveDescriptorPool&)            = delete; /**< Constructeur de copie supprimé. */
			LveDescriptorPool& operator=(const LveDescriptorPool&) = delete; /**< Opérateur d'affectation supprimé. */


			/**
			 * @brief Alloue un ensemble de descripteurs à partir du pool.
			 *
			 * @param _descriptorSetLayout Layout de l'ensemble de descripteurs à allouer.
			 * @param _descriptor Référence où stocker l'ensemble de descripteurs alloué.
			 * @return true si l'allocation réussit, false sinon.
			 */
			bool AllocateDescriptor(
				vk::DescriptorSetLayout _descriptorSetLayout, vk::DescriptorSet& _descriptor) const;

			/**
			 * @brief Libère un ensemble de descripteurs.
			 *
			 * @param _descriptors Vecteur contenant les ensembles de descripteurs à libérer.
			 */
			void FreeDescriptors(const std::vector<vk::DescriptorSet>& _descriptors) const;

			/**
			 * @brief Réinitialise le pool de descripteurs.
			 */
			void ResetPool() const;

			vk::DescriptorPool GetDescriptorPool() { return descriptorPool; }

		private:
			LveDevice&         lveDevice;      /**< Référence vers l'appareil logique associé. */
			vk::DescriptorPool descriptorPool; /**< Pool de descripteurs Vulkan. */
			friend class LveDescriptorWriter;
			/**< Permet à la classe LveDescriptorWriter d'accéder aux membres privés. */
	};

	class LveDescriptorWriter
	{
		public:
			/**
			 * @brief Constructeur de LveDescriptorWriter.
			 * @param _setLayout Référence vers l'ensemble de descripteurs.
			 * @param _pool Référence vers le pool de descripteurs.
			 */
			LveDescriptorWriter(LveDescriptorSetLayout& _setLayout, LveDescriptorPool& _pool);

			/**
			 * @brief Écriture d'informations de tampon dans le descripteur.
			 * @param _binding Indice de liaison du descripteur.
			 * @param _bufferInfo Pointeur vers les informations du tampon.
			 * @return Une référence vers l'objet LveDescriptorWriter.
			 */
			LveDescriptorWriter& WriteBuffer(uint32_t _binding, const vk::DescriptorBufferInfo* _bufferInfo);

			/**
			 * @brief Écriture d'informations d'image dans le descripteur.
			 * @param _binding Indice de liaison du descripteur.
			 * @param _imageInfo Pointeur vers les informations de l'image.
			 * @return Une référence vers l'objet LveDescriptorWriter.
			 */
			LveDescriptorWriter& WriteImage(uint32_t _binding, const vk::DescriptorImageInfo* _imageInfo);


			/**
			  * @brief Construit un ensemble de descripteurs à partir des écritures précédentes.
			  * @param _set Référence vers l'ensemble de descripteurs à construire.
			  * @return Booléen indiquant si la construction a réussi.
			  */
			bool Build(vk::DescriptorSet& _set);

			/**
			 * @brief Écrit à nouveau les informations de descripteur dans un ensemble de descripteurs existant.
			 * @param _set Référence vers l'ensemble de descripteurs à modifier.
			 */
			void Overwrite(const vk::DescriptorSet& _set);

		private:
			/**
			 * @brief Référence vers l'ensemble de descripteurs utilisé pour définir la mise en page des descripteurs.
			 */
			LveDescriptorSetLayout& setLayout;

			/**
			 * @brief Référence vers le pool de descripteurs utilisé pour l'allocation des ensembles de descripteurs.
			 */
			LveDescriptorPool& pool;

			/**
			 * @brief Vecteur contenant les opérations d'écriture de descripteurs à appliquer lors de la construction des ensembles de descripteurs.
			 */
			std::vector<vk::WriteDescriptorSet> writes;
	};
} // namespace lve
