#pragma once

#include <vulkan/vulkan.hpp>
#include "LveEngine/lve_buffer.h"
#include "LveEngine/lve_device.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

//std
#include <memory>
#include <string>
#include <vector>

namespace lve
{
	/**
	 * @brief Classe représentant un modèle Vulkan.
	 */
	class LveModel
	{
		public:

			/**
			 * @brief Structure représentant un vertex dans l'espace 3D.
			 */
			struct Vertex
			{
				glm::vec3 position{}; /**< Position du vertex dans l'espace. */
				glm::vec3 color{};    /**< Couleur du vertex. */
				glm::vec3 normal{};   /**< Normale du vertex. */
				glm::vec2 uv{};       /**< Coordonnées de texture du vertex. */
				unsigned int texMultiplier = 1;

				/**
				 * @brief Obtient les descriptions de liaison des entrées de vertex.
				 *
				 * @return Un vecteur contenant les descriptions de liaison des entrées de vertex.
				 */
				static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();

				/**
				 * @brief Obtient les descriptions d'attribut des entrées de vertex.
				 *
				 * @return Un vecteur contenant les descriptions d'attribut des entrées de vertex.
				 */
				static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();

				/**
				 * @brief Opérateur de comparaison d'égalité.
				 *
				 * @param _other L'autre vertex à comparer.
				 * @return true si les deux vertices sont égaux, sinon false.
				 */
				bool operator==(const Vertex& _other) const
				{
					return position == _other.position && color == _other.color && normal == _other.normal && uv ==
					       _other.uv;
				}
			};


			/**
			 * @brief Structure responsable de la construction d'un modèle 3D.
			 */
			struct Builder
			{
				std::vector<Vertex>   vertices{}; /**< Vecteur contenant les vertices du modèle. */
				std::vector<uint32_t> indices{};  /**< Vecteur contenant les indices des vertices. */

				/**
				 * @brief Charge un modèle à partir d'un fichier au format OBJ.
				 *
				 * Cette fonction charge un modèle à partir d'un fichier au format OBJ.
				 * Elle utilise la bibliothèque tinyobjloader pour lire le fichier et extraire les attributs des vertices tels que la position, la couleur, la normale et les coordonnées de texture.
				 * Les vertices sont stockés dans le vecteur 'vertices' de l'objet LveModel::Builder, tandis que les indices des vertices sont stockés dans le vecteur 'indices'.
				 * Les vertices sont comparés pour détecter les doublons et les indices sont ajustés en conséquence pour assurer l'unicité des vertices.
				 *
				 * @param _filepath Le chemin du fichier du modèle.
				 * @throws std::runtime_error Si une erreur survient lors du chargement du modèle.
				 */
				void LoadModel(const std::string& _filepath, float _texMultiplier, glm::vec3 _color =  {-1,-1,-1});
			};


			/**
			 * @brief Constructeur de la classe LveModel.
			 *
			 * Construit un modèle 3D à partir des données fournies par le builder.
			 *
			 * @param _lveDevice Référence vers l'objet LveDevice utilisé pour la création du modèle.
			 * @param _builder Référence vers l'objet Builder contenant les données du modèle.
			 */
			LveModel(LveDevice& _lveDevice, const Builder& _builder);

			/**
			 * @brief Destructeur de la classe LveModel.
			 */
			~LveModel();

			/**
			 * @brief Constructeur de copie supprimé.
			 *
			 * Le constructeur de copie de la classe LveModel est supprimé pour empêcher la duplication d'instances de modèle.
			 */
			LveModel(const LveModel&) = delete;

			/**
			 * @brief Opérateur d'affectation supprimé.
			 *
			 * L'opérateur d'affectation de la classe LveModel est supprimé pour empêcher l'affectation d'une instance de modèle à une autre.
			 */
			LveModel operator=(const LveModel&) = delete;


			/**
			 * @brief Crée un modèle à partir d'un fichier.
			 *
			 * Cette fonction statique crée un modèle à partir d'un fichier en utilisant un objet LveDevice et un chemin de fichier donnés.
			 * Elle charge d'abord le modèle à l'aide du constructeur Builder, puis affiche le nombre de vertices chargés.
			 * Enfin, elle crée un modèle unique à partir de l'objet LveDevice et du constructeur Builder, puis le retourne.
			 *
			 * @param _device L'objet LveDevice utilisé pour créer le modèle.
			 * @param _filepath Le chemin du fichier à partir duquel charger le modèle.
			 * @return Un pointeur unique vers le modèle créé.
			 */
			static std::unique_ptr<LveModel> CreateModelFromFile(LveDevice& _device, const std::string& _filepath, float _texMultiplier = 1, glm::vec3 _color = {-1,-1,-1});

			/**
			 * @brief Lie les tampons de vertex et d'index au tampon de commande spécifié.
			 *
			 * Cette fonction lie les tampons de vertex et d'index au tampon de commande spécifié pour le rendu ultérieur.
			 * Si un tampon d'index est présent, il est également lié au tampon de commande avec le type d'index spécifié.
			 *
			 * @param _commandBuffer Le tampon de commande auquel lier les tampons de vertex et d'index.
			 */
			void Bind(vk::CommandBuffer _commandBuffer) const;

			/**
			 * @brief Dessine les primitives géométriques à l'aide du tampon de commande spécifié.
			 *
			 * Cette fonction dessine les primitives géométriques à l'aide du tampon de commande spécifié.
			 * Si un tampon d'index est présent, la fonction utilise vkCmdDrawIndexed pour dessiner les primitives indexées. Sinon, elle utilise vkCmdDraw pour dessiner les primitives non indexées.
			 *
			 * @param _commandBuffer Le tampon de commande utilisé pour dessiner les primitives géométriques.
			 */
			void Draw(vk::CommandBuffer _commandBuffer) const;

			std::string GetFilename() const { return filename; }
			void SetFileName(const std::string& _filename) { filename = _filename; }

		private:
			/**
			 * @brief Crée le tampon de vertex.
			 *
			 * Cette fonction crée le tampon de vertex à partir des données de vertex fournies.
			 * Elle calcule la taille du tampon en fonction du nombre de vertices et de leur taille individuelle, puis crée un tampon de transfert de mémoire visible par l'hôte pour copier les données de vertex.
			 * Les données de vertex sont ensuite copiées dans le tampon de transfert, puis transférées vers un tampon de mémoire local du périphérique pour une utilisation efficace pendant le rendu.
			 *
			 * @param _vertices Les données de vertex à utiliser pour créer le tampon.
			 */
			void CreateVertexBuffer(const std::vector<Vertex>& _vertices);


			/**
			 * @brief Crée le tampon d'index.
			 *
			 * Cette fonction crée le tampon d'index à partir des indices fournis.
			 * Elle détermine d'abord si des indices sont présents. Si aucun indice n'est présent, la fonction se termine.
			 * Sinon, elle calcule la taille du tampon en fonction du nombre d'indices et de leur taille individuelle, puis crée un tampon de transfert de mémoire visible par l'hôte pour copier les données d'indices.
			 * Les données d'indices sont ensuite copiées dans le tampon de transfert, puis transférées vers un tampon de mémoire local du périphérique pour une utilisation efficace pendant le rendu.
			 *
			 * @param _indices Les indices à utiliser pour créer le tampon.
			 */
			void CreateIndexBuffer(const std::vector<uint32_t>& _indices);

			LveDevice& lveDevice; /**< Référence vers l'objet LveDevice utilisé pour la création du modèle. */

			std::string filename = "model.obj";

			std::unique_ptr<LveBuffer> vertexBuffer; /**< Pointeur unique vers le tampon de vertex du modèle. */
			uint32_t                   vertexCount;  /**< Nombre de vertices dans le tampon de vertex. */

			bool                       hasIndexBuffer = false; /**< Indique si le modèle possède un tampon d'indices. */
			std::unique_ptr<LveBuffer> indexBuffer; /**< Pointeur unique vers le tampon d'indices du modèle. */
			uint32_t                   indexCount; /**< Nombre d'indices dans le tampon d'indices. */
	};
} //lve
