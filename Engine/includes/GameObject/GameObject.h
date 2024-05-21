#pragma once
#ifdef ENGINECORE_EXPORTS
#define ENGINEDLL_API __declspec(dllexport)
#else
#define ENGINEDLL_API __declspec(dllimport)
#endif

#include <glm.hpp>
#include <string>
#include <vector>
#include "Components/Component.h"
#include "LveEngine/lve_model.h"
#include <iostream>



#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class GameObjectType {
	Cube,
	Light,
	Plane,
	Vase,
	Girl,
	Noob,
	Sphere,
	Triangle,
	Multiple
	// TODO: Ajouter plus de types d'objets
};

enum class LayerType
{
	Normal,
	HUD,
	Background
};

class Component;

class Transform;


struct PointLightComponent;

/**
 * @brief Représente un objet générique dans le jeu.
 *
 * Cette classe représente un objet générique dans le jeu. Un objet peut être un cube, un plan, une lumière, etc.
 * Elle sert de base pour d'autres types d'objets spécifiques.
 */
//class ENGINEDLL_API GameObject
class  GameObject
{
	public:
		using id_t = unsigned int;

		/**
		 * @brief Constructeur par défaut de la classe GameObject.
		 *
		 * Ce constructeur crée un nouvel objet GameObject.
		 */
		GameObject();

		/**
		 * @brief Destructeur de la classe GameObject.
		 *
		 * Ce destructeur libère les ressources associées à l'objet GameObject.
		 */
		~GameObject();

		/**
		* @brief Constructeur de GameObject avec ID spécifique.
		*
		* Initialise un GameObject avec un identifiant unique. Cela garantit que chaque GameObject
		* a un identifiant unique au sein de la scène.
		*
		* @param _id Identifiant unique du GameObject.
		*/
		explicit GameObject(id_t _id);

		#pragma region Getter

		#pragma region Information

		/**
		 * @brief Obtient l'identifiant unique de l'objet.
		 *
		 * Cette méthode retourne l'identifiant unique de l'objet GameObject.
		 *
		 * @return L'identifiant unique de l'objet.
		 */
		[[nodiscard]] id_t GetId() const { return id; }

		/**
		 * @brief Obtient le nom de l'objet.
		 *
		 * Cette méthode retourne le nom de l'objet GameObject.
		 *
		 * @return Le nom de l'objet.
		 */
		[[nodiscard]] std::string GetName() const { return name; }

		#pragma endregion

		#pragma region Transform

		/**
		 * @brief Obtient le transform de l'objet.
		 *
		 * Cette méthode retourne un pointeur vers l'objet Transform associé à l'objet GameObject.
		 *
		 * @return Un pointeur vers l'objet Transform associé à l'objet GameObject. Si aucun Transform n'est associé, retourne nullptr.
		 */
		[[nodiscard]] Transform* GetTransform() const;

		/**
		 * @brief Obtient la position de l'objet.
		 *
		 * Cette méthode retourne la position de l'objet GameObject dans l'espace tridimensionnel.
		 *
		 * @return La position de l'objet.
		 */
		[[nodiscard]] glm::vec3 GetPosition() const;

		/**
		 * @brief Obtient l'échelle de l'objet.
		 *
		 * Cette méthode retourne l'échelle de l'objet GameObject dans l'espace tridimensionnel.
		 *
		 * @return L'échelle de l'objet.
		 */
		[[nodiscard]] glm::vec3 GetScale() const;

		/**
		 * @brief Obtient la rotation de l'objet.
		 *
		 * Cette méthode retourne la rotation de l'objet GameObject dans l'espace tridimensionnel.
		 *
		 * @return La rotation de l'objet.
		 */
		[[nodiscard]] glm::vec3 GetRotation() const;

		#pragma endregion

		#pragma region State

		/**
		 * @brief Obtient l'état d'activation de l'objet.
		 *
		 * Cette méthode retourne l'état d'activation de l'objet GameObject.
		 *
		 * @return L'état d'activation de l'objet (true s'il est activé, false s'il est désactivé).
		 */
		[[nodiscard]] bool GetActive() const { return isActive; }

		/**
		 * @brief Obtient l'état d'activation de l'objet.
		 *
		 * Cette méthode retourne l'état d'activation de l'objet GameObject.
		 *
		 * @return L'état d'activation de l'objet (true s'il est activé, false s'il est désactivé).
		 */
		[[nodiscard]] bool GetVisible() const { return isVisible; }

		std::string GetDesc() const { return desc; }

		#pragma endregion

		#pragma region Layer

		/**
		 * @brief Obtient le type de couche de l'objet.
		 *
		 * Cette méthode retourne le type de couche de l'objet GameObject.
		 *
		 * @return Le type de couche de l'objet.
		 */
		[[nodiscard]] LayerType GetLayer() const { return layerType; }

		#pragma endregion

		#pragma region Components

		/**
		 * @brief Obtient le modèle associé au module de débogage.
		 *
		 * Cette méthode permet d'obtenir le modèle associé au module de débogage.
		 *
		 * @return Un pointeur partagé vers le modèle associé au module de débogage.
		 */
		std::shared_ptr<lve::LveModel> GetModel() { return model; }

		/**
		 * @brief Obtient la couleur associée au module de débogage.
		 *
		 * Cette méthode permet d'obtenir la couleur associée au module de débogage.
		 *
		 * @return La couleur associée au module de débogage.
		 */
		[[nodiscard]] glm::vec3                      GetColor() const { return color; }

		/**
		 * @brief Obtient l'identifiant de texture associé au module de débogage.
		 *
		 * Cette méthode permet d'obtenir l'identifiant de texture associé au module de débogage.
		 *
		 * @return L'identifiant de texture associé au module de débogage.
		 */
		[[nodiscard]] int              GetTexture() const { return texture; }

		/**
		 * @brief Obtient les composants de l'objet.
		 *
		 * Cette méthode retourne un vecteur contenant des pointeurs vers tous les composants attachés à l'objet GameObject.
		 *
		 * @return Un vecteur contenant des pointeurs vers les composants de l'objet.
		 */
		std::vector<Component*> GetComponents() { return components; }

		/**
		 * @brief Obtient un composant de type spécifique.
		 *
		 * Cette méthode recherche un composant de type spécifique attaché à l'objet GameObject.
		 *
		 * @tparam T Le type de composant à rechercher.
		 * @return Un pointeur vers le premier composant de type spécifique trouvé attaché à l'objet, ou nullptr si aucun composant n'est trouvé.
		 */
		template <typename T>
		T* GetComponent()
		{
			for (size_t i = 0; i < components.size(); i++)
			{
				// Vérifie si le composant est un Component
				if (T* component_result = dynamic_cast<T*>(components[i])) return component_result;
				// Renvoie le Component trouvé
			}
			return nullptr; // Renvoie nullptr si aucun Component n'est trouvé
		}

		/**
		 * @brief Obtient tous les composants d'un type spécifique attachés à l'objet.
		 *
		 * Cette méthode recherche tous les composants d'un type spécifique attachés à l'objet GameObject.
		 *
		 * @tparam T Le type de composant à rechercher.
		 * @return Un vecteur contenant des pointeurs vers tous les composants de type spécifique attachés à l'objet, ou un vecteur vide si aucun composant n'est trouvé.
		 */
		template <typename T>
		std::vector<T*> GetComponentsByType()
		{
			std::vector<T*> components_by_type;
			for (size_t i = 0; i < components.size(); i++)
			{
				// Vérifie si le composant est un Component
				if (T* component_result = dynamic_cast<T*>(components[i]))
					components_by_type.
						push_back(component_result); // Ajout le Component trouvé
			}
			return components_by_type;
		}

		/**
		 * @brief Obtient un composant d'un type spécifique par son nom.
		 *
		 * Cette méthode recherche un composant d'un type spécifique attaché à l'objet GameObject
		 * dont le nom correspond au nom spécifié.
		 *
		 * @tparam T Le type de composant à rechercher.
		 * @param _name Le nom du composant à rechercher.
		 * @return Un pointeur vers le premier composant de type spécifique trouvé avec le nom correspondant, ou nullptr si aucun composant n'est trouvé.
		 */
		template <typename T>
		T* GetComponentByName(const std::string& _name)
		{
			std::vector<T*> components_by_type = GetComponentsByType<T>();
			for (size_t i = 0; i < components_by_type.size(); i++)
			{
				// Vérifie si le composant est un Component
				T* component_result = dynamic_cast<T*>(components_by_type[i]);
				if (component_result && static_cast<Component*>(component_result)->GetName() == _name)
					return
						component_result; // Renvoie le Component trouvé
			}
			return nullptr; // Renvoie nullptr si aucun Component n'est trouvé
		}

		/**
		 * @brief Obtient un composant récursivement par son nom.
		 *
		 * Cette méthode recherche récursivement un composant dans l'arborescence des enfants de l'objet GameObject
		 * dont le nom correspond au nom spécifié.
		 *
		 * @param _componentName Le nom du composant à rechercher.
		 * @return Un pointeur vers le premier composant trouvé dont le nom correspond au nom spécifié.
		 *         Si aucun composant correspondant n'est trouvé dans cet objet ou ses enfants, retourne nullptr.
		 */
		Component* GetComponentRecursive(const std::string& _componentName) const;

		#pragma endregion

		#pragma region GameObjects


		/**
		 * @brief Recherche les enfants de l'objet par leur nom.
		 *
		 * Cette méthode recherche les enfants de l'objet GameObject dont le nom correspond au nom spécifié.
		 *
		 * @param _name Le nom des enfants à rechercher.
		 * @return Un vecteur contenant des pointeurs vers les enfants dont le nom correspond au nom spécifié. Si aucun enfant n'est trouvé, le vecteur est vide.
		 */
		[[nodiscard]] std::vector<GameObject*> FindChildrenByName(const std::string& _name) const;

		/**
		 * @brief Obtient les enfants de l'objet.
		 *
		 * Cette méthode retourne une référence constante vers le vecteur contenant les pointeurs vers les enfants de l'objet GameObject.
		 *
		 * @return Une référence constante vers le vecteur contenant les enfants de l'objet.
		 */
		[[nodiscard]] const std::vector<GameObject*>& GetChildren() const
		{
			return children;
		}

		#pragma endregion

		#pragma endregion

		#pragma region Setter

		#pragma region Information

		/**
		 * @brief Définit le nom de l'objet.
		 *
		 * Cette méthode définit le nom de l'objet GameObject.
		 *
		 * @param _newName Le nouveau nom à définir pour l'objet.
		 */
		void SetName(const std::string& _newName) { name = _newName; }

	std::string GetFileModel() { return fileModel; }
	float GetTexMultiplier() { return texMultiplier; }

	void SetTexMultiplier(float _newTexMultiplier) { texMultiplier = _newTexMultiplier; }
	void SetFileModel(std::string _newFileModel) { fileModel = _newFileModel; }
		#pragma endregion

		#pragma region Transform

		/**
		 * @brief Définit la position de l'objet.
		 *
		 * Cette méthode définit la position de l'objet GameObject dans l'espace tridimensionnel.
		 *
		 * @param _newPosition La nouvelle position à définir pour l'objet.
		 */
		void SetPosition(glm::vec3 _newPosition) const;

		/**
		 * @brief Définit l'échelle de l'objet.
		 *
		 * Cette méthode définit l'échelle de l'objet GameObject dans l'espace tridimensionnel.
		 *
		 * @param _newScale La nouvelle échelle à définir pour l'objet.
		 */
		void SetScale(glm::vec3 _newScale) const;

		/**
		 * @brief Définit la rotation de l'objet autour de l'axe Y.
		 *
		 * Cette méthode définit la rotation de l'objet GameObject autour de l'axe Y dans l'espace tridimensionnel.
		 *
		 * @param _newRotation La nouvelle rotation à définir pour l'objet (en radians).
		 */
		void SetRotation(float _newRotation) const;

		#pragma endregion

		#pragma region State

		/**
		 * @brief Active ou désactive l'objet.
		 *
		 * Cette méthode active ou désactive l'objet GameObject en fonction de l'état spécifié.
		 *
		 * @param _state L'état d'activation à définir pour l'objet (true pour activer, false pour désactiver).
		 */
		void SetActive(const bool& _state) { isActive = _state; }

		/**
		 * @brief Active ou désactive le rendu de l'objet.
		 *
		 * Cette méthode active ou désactive le rendu de l'objet GameObject en fonction de l'état spécifié.
		 *
		 * @param _state L'état de visibilité à définir pour l'objet (true pour le rendre visible, false pour le rendre invisible).
		 */
		void SetVisible(const bool& _state) { isVisible = _state; }

		/**
		 * @brief Active ou désactive l'objet et son rendu simultanément.
		 *
		 * Cette méthode active ou désactive l'objet GameObject et son rendu simultanément en fonction de l'état spécifié.
		 *
		 * @param _state L'état à définir pour l'activation et la visibilité de l'objet (true pour activer et rendre visible, false pour désactiver et rendre invisible).
		 */
		void SetActiveAndVisible(const bool& _state)
		{
			isActive  = _state;
			isVisible = _state;
		}

		void SetDesc(std::string _newDesc) { desc = _newDesc; }

		#pragma endregion

		#pragma region Layer

		/**
		 * @brief Définit le type de couche de l'objet.
		 *
		 * Cette méthode définit le type de couche de l'objet GameObject.
		 *
		 * @param _layerType Le nouveau type de couche à définir pour l'objet.
		 */
		void SetLayer(const LayerType& _layerType) { layerType = _layerType; }

		#pragma endregion

		#pragma region GameObjects

		/**
		 * @brief Définit le parent de l'objet.
		 *
		 * Cette méthode définit le parent de l'objet GameObject.
		 *
		 * @param _parent Un pointeur vers l'objet parent à définir pour cet objet. Si nullptr est passé, l'objet n'aura aucun parent.
		 */
		void SetParent(GameObject* _parent) { parent = _parent; }
		#pragma endregion

		#pragma endregion

		#pragma region Add

		#pragma region GameObjects


		/**
		 * @brief Ajoute un enfant à l'objet.
		 *
		 * Cette méthode ajoute un nouvel enfant à l'objet GameObject.
		 *
		 * @param _child Un pointeur vers l'objet enfant à ajouter.
		 */
		void AddChildObject(GameObject* _child);

		#pragma endregion

		#pragma region Components

		/**
		 * @brief Définit le modèle associé au module de débogage.
		 *
		 * Cette méthode permet de définir le modèle associé au module de débogage.
		 *
		 * @param _model Un pointeur partagé vers le modèle à associer au module de débogage.
		 */
		void SetModel(const std::shared_ptr<lve::LveModel>& _model) {  model = _model; }

		/**
		 * @brief Définit la couleur associée au module de débogage.
		 *
		 * Cette méthode permet de définir la couleur associée au module de débogage.
		 *
		 * @param _color La couleur à associer au module de débogage.
		 */
		void SetColor(const glm::vec3 _color) {  color = _color; }

		/**
		 * @brief Définit l'identifiant de texture associé au module de débogage.
		 *
		 * Cette méthode permet de définir l'identifiant de texture associé au module de débogage.
		 *
		 * @param _texture L'identifiant de texture à associer au module de débogage.
		 */
		void SetTexture(const int _texture) { texture = _texture; }

		/**
		 * @brief Ajoute un composant à l'objet.
		 *
		 * Cette méthode ajoute un nouveau composant à l'objet GameObject.
		 *
		 * @param _component Un pointeur vers le composant à ajouter.
		 */
		void AddComponent(Component* _component);


		#pragma endregion

		#pragma endregion


		#pragma region Create

		#pragma region GameObjects

		/**
		 * @brief Crée un nouvel objet GameObject.
		 *
		 * Cette méthode statique crée un nouvel objet GameObject avec un identifiant unique.
		 *
		 * @return Un nouvel objet GameObject créé avec un identifiant unique.
		 */
		static GameObject CreateGameObject()
		{
			static id_t current_id = 0;
			return GameObject{current_id++};
		}

		/**
		 * @brief Crée un nouvel objet GameObject sur le tas.
		 *
		 * Cette méthode statique crée un nouvel objet GameObject avec un identifiant unique sur le tas (allocation dynamique).
		 *
		 * @return Un pointeur vers le nouvel objet GameObject créé avec un identifiant unique sur le tas.
		 */
		static GameObject* CreatePGameObject()
		{
			static id_t current_id = 0;
			return new GameObject{current_id++};
		}

		#pragma endregion

		#pragma region Components

		/**
		 * @brief Crée un nouveau composant de type spécifique et l'attache à l'objet.
		 *
		 * Cette méthode crée un nouveau composant de type spécifique et l'attache à l'objet GameObject.
		 *
		 * @tparam T Le type de composant à créer.
		 * @return Un pointeur vers le nouveau composant créé et attaché à l'objet, ou nullptr en cas d'échec de création.
		 */
		template <typename T>
		T* CreateComponent()
		{
			T* component = new T();
			component->SetOwner(this);
			components.push_back(component);
			return component;
		}

		#pragma endregion

		#pragma endregion
		/**
		 * @brief Supprime directement cet objet de jeu.
		 *
		 * Cette méthode supprime directement l'objet de jeu de la scène et libère la mémoire associée.
		 * À utiliser avec prudence, car elle ne gère pas les relations entre objets.
		 */
		void DestroyGameObject() {}

		/**
		 * @brief Marque cet objet de jeu pour suppression.
		 *
		 * Cette méthode marque l'objet de jeu pour suppression par le gestionnaire de scènes.
		 * La suppression réelle aura lieu plus tard, gérée par le gestionnaire de scènes.
		 */
		void MarkForDeletion()
		{
		}

		/**
		 * @brief Supprime un composant de l'objet.
		 *
		 * Cette méthode supprime un composant spécifié de l'objet GameObject.
		 *
		 * @param _component Un pointeur vers le composant à supprimer.
		 */
		void RemoveComponent(const Component* _component);

		#pragma region Event
		/**
		* @brief Initialise le module.
		*/
		virtual void Init();

		/**
		 * @brief Démarre le module.
		 */
		virtual void Start();

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		virtual void FixedUpdate() const;

		/**
		 * @brief Met à jour le module.
		 */
		virtual void Update() const;

		virtual void UpdateEditor() const;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		virtual void PreRender();

		/**
		 * @brief Rendu du module.
		 */
		virtual void Render() const;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		virtual void RenderGui();

		/**
		 * @brief Fonction post-rendu du module.
		 */
		virtual void PostRender();

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		virtual void Release();

		/**
		 * @brief Finalise le module.
		 */
		virtual void Finalize();

		#pragma endregion

		json toJson() const;

		void fromJson(const json& _j);

	protected:

		float						texMultiplier = 1;
		std::string             fileModel;

		std::string name = "GameObject"; /**< Le nom de l'objet GameObject. */

		std::string desc = "";

		id_t id; /**< L'identifiant unique de l'objet GameObject. */

		GameObject*              parent = nullptr; /**< Le parent de l'objet GameObject. */
		std::vector<GameObject*> children;         /**< Les enfants de l'objet GameObject. */

		std::shared_ptr<lve::LveModel> model{};     /**< Le modèle de l'objet GameObject. */
		int                       texture = 0; /**< L'identifiant de texture de l'objet GameObject. */
		glm::vec3                 color{};     /**< La couleur de l'objet GameObject. */

		std::vector<Component*> components; /**< Les composants attachés à l'objet GameObject. */

		Transform* transform = nullptr; /**< Le composant de transformation attaché à l'objet GameObject. */

		LayerType layerType = LayerType::Normal; /**< Le type de couche de l'objet GameObject. */

		float depth = 0.f;
		/**< La profondeur de l'objet GameObject (plus proche de 1 signifie plus proche de l'écran). */

		bool isActive  = true; /**< Indique si l'objet GameObject est actif (true) ou non (false). */
		bool isVisible = true; /**< Indique si l'objet GameObject est visible (true) ou non (false). */
};
