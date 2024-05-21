#pragma once

#include <string>

#include <GameObject/PreGameObject/LightGameObject.h>
#include "GameObject/GameObject.h"
#include "LveEngine/lve_window.h"

#include <nlohmann/json.hpp>

class GameObject;
using json = nlohmann::json;
/**
 * @brief Interface de base pour les scènes du jeu.
 *
 * Cette classe définit une interface abstraite pour les scènes du jeu.
 * Les classes dérivées sont responsables de définir le comportement spécifique
 * de chaque scène
 */
class BaseScene
{
	public:
		/**
		 * @brief Constructeur explicite de la classe BaseScene.
		 *
		 * Initialise une nouvelle instance de la classe BaseScene avec le nom spécifié.
		 *
		 * @param _fileName Le nom de la scène.
		 * @param _id
		 */
		explicit BaseScene(const std::string& _fileName, const int _id) : id(_id), name(_fileName){
			const auto sun = lve::LightGameObject::Create(1000000.f, 2.0f, glm::vec3{ 0.f, -1000.f, 0.f });
			sun->SetName("Sun");
			rootObjects.push_back(sun);
		}
		~BaseScene() = default;

		BaseScene(const BaseScene&)            = delete;
		BaseScene& operator=(const BaseScene&) = delete;

#pragma region Event

		/**
		 * @brief Initialise le module.
		 */
		virtual void Init();

		/**
		 * @brief D�marre le module.
		 */
		virtual void Start();

		/**
		 * @brief Effectue une mise � jour fixe du module.
		 */
		virtual void FixedUpdate();

		/**
		 * @brief Met � jour le module.
		 */
		virtual void Update();

		virtual void UpdateEditor();

		/**
		 * @brief Fonction pr�-rendu du module.
		 */
		virtual void PreRender();

		/**
		 * @brief Rendu du module.
		 */
		virtual void Render();

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		virtual void RenderGui();

		/**
		 * @brief Fonction post-rendu du module.
		 */
		virtual void PostRender();

		/**
		 * @brief Lib�re les ressources utilis�es par le module.
		 */
		virtual void Release();

		/**
		 * @brief Finalise le module.
		 */
		virtual void Finalize();

#pragma endregion

#pragma region Getter

		/**
		 * @brief Obtient un pointeur vers l'objet GameObject ayant l'identifiant spécifié.
		 *
		 * @param _gameObjectId L'identifiant de l'objet GameObject à rechercher.
		 * @return Un pointeur vers l'objet GameObject trouvé, ou nullptr s'il n'est pas trouvé.
		 */
		GameObject* GetGameObjectById(const GameObject::id_t& _gameObjectId) const;

		/**
		 * @brief Recherche et retourne tous les objets GameObject portant un nom spécifique.
		 *
		 * @param _name Le nom des objets GameObject à rechercher.
		 * @return Un vecteur contenant tous les objets GameObject trouvés avec le nom spécifié.
		 */
		std::vector<GameObject*> FindGameObjectsByName(const std::string& _name) const;

		/**
		 * @brief Obtient le nom de la scène.
		 *
		 * @return Le nom de la scène.
		 */
		std::string GetName() const;

		/**
		 * @brief Obtient le chemin de fichier relatif par défaut pour la scène.
		 *
		 * @return Le chemin de fichier relatif par défaut pour la scène.
		 */
		std::string GetDefaultRelativeFilePath() const;

		/**
		 * @brief Récupère le nom du fichier associé à la scène.
		 *
		 * @return Le nom du fichier de la scène.
		 */
		std::string GetFileName() const;

		/**
		 * @brief Récupère une référence vers le vecteur des objets racine de la scène.
		 *
		 * @return Une référence vers le vecteur des objets racine de la scène.
		 */
		std::vector<GameObject*>& GetRootObjects();

		/**
		 * @brief Obtient tous les objets de jeu.
		 *
		 * @return Un vecteur contenant tous les objets de jeu.
		 */
		std::vector<GameObject*> GetAllGameObject() {
			std::vector<GameObject*> allGameObject;
			for (const auto& gameObject : rootObjects) {
				AddGameObjectInVector(allGameObject, gameObject);
			}
			return allGameObject;
		}

		bool IsActive() const { return isActive; }


#pragma endregion

#pragma region Setter

		/**
		 * @brief Définit le nom de la scène.
		 *
		 * @param _name Le nouveau nom de la scène.
		 */
		void        SetName(const std::string& _name);

		/**
		 * @brief Définit le nom de fichier de la scène et supprime les fichiers précédents si spécifié.
		 *
		 * @param _fileName Le nouveau nom de fichier de la scène.
		 * @param _bDeletePreviousFiles Indique s'il faut supprimer les fichiers précédents ou non.
		 * @return true si le nom de fichier a été correctement défini, sinon false.
		 */
		bool        SetFileName(const std::string& _fileName, bool _bDeletePreviousFiles) const;

		/**
		 * @brief Définit l'indice d'un objet racine dans la liste des objets racines de la scène.
		 *
		 * @param _rootObject L'objet racine dont l'indice doit être défini.
		 * @param _newIndex Le nouvel indice de l'objet racine.
		 */
		void SetRootObjectIndex(GameObject* _rootObject, uint32_t _newIndex) {}

		void SetActive(const bool _state) { isActive = _state; }

#pragma endregion

#pragma region Create

		/**
		 * @brief Crée un nouveau GameObject et l'ajoute à la scène.
		 *
		 * @return Un pointeur vers le nouveau GameObject créé.
		 */
		GameObject* CreateGameObject();

#pragma endregion

#pragma region Destroy

		/**
		 * @brief Détruit le GameObject spécifié et le retire de la scène.
		 *
		 * @param _gameObject Le GameObject à détruire.
		 */
		void                     DestroyGameObject(const GameObject* _gameObject);

#pragma endregion

		/**
		 * @brief Vérifie si la scène a été initialisée.
		 *
		 * @return true si la scène est initialisée, sinon false.
		 */
		bool IsInitialized() const;

		/**
		 * @brief Vérifie si un fichier existe.
		 *
		 * @param _filePath Le chemin du fichier à vérifier.
		 * @return true si le fichier existe, sinon false.
		 */
		bool        FileExists(const std::string& _filePath);

		/**
		 * @brief Indique si la scène utilise un fichier de sauvegarde.
		 *
		 * @return true si la scène utilise un fichier de sauvegarde, sinon false.
		 */
		bool        IsUsingSaveFile() const;

		/**
		 * @brief Supprime les fichiers de sauvegarde associés à la scène.
		 */
		void        DeleteSaveFiles();

		/**
		 * @brief Vérifie si une valeur est contenue dans un conteneur.
		 *
		 * @param _container Le conteneur à vérifier.
		 * @param _value La valeur à rechercher dans le conteneur.
		 * @return true si la valeur est présente dans le conteneur, sinon false.
		 */
		bool        Contains(const std::vector<GameObject::id_t>& _container, const GameObject::id_t& _value);
	
		/**
		 * @brief Ajoute un objet racine à la scène.
		 *
		 * @param _gameObject L'objet à ajouter comme racine.
		 * @return Un pointeur vers l'objet ajouté en tant que racine.
		 */
		GameObject* AddRootObject(GameObject* _gameObject);

		/**
		 * @brief Ajoute immédiatement un objet racine à la scène.
		 *
		 * Cette fonction ajoute immédiatement un objet racine à la scène sans attendre la prochaine mise à jour.
		 *
		 * @param _gameObject L'objet à ajouter comme racine.
		 * @return Un pointeur vers l'objet ajouté en tant que racine.
		 */
		GameObject* AddRootObjectImmediate(GameObject* _gameObject){}

		/**
		 * @brief Ajoute un objet enfant à un parent donné dans la scène.
		 *
		 * @param _parent Le parent auquel ajouter l'objet enfant.
		 * @param _child L'objet à ajouter comme enfant.
		 * @return Un pointeur vers l'objet ajouté comme enfant.
		 */
		GameObject* AddChildObject(GameObject* _parent, GameObject* _child) {}

		/**
		 * @brief Ajoute immédiatement un objet enfant à un parent donné dans la scène.
		 *
		 * Cette fonction ajoute immédiatement un objet enfant à un parent donné dans la scène sans attendre la prochaine mise à jour.
		 *
		 * @param _parent Le parent auquel ajouter l'objet enfant.
		 * @param _child L'objet à ajouter comme enfant.
		 * @return Un pointeur vers l'objet ajouté comme enfant.
		 */
		GameObject* AddChildObjectImmediate(GameObject* _parent, GameObject* _child) {}

		/**
		 * @brief Ajoute immédiatement un nouvel objet frère à un objet donné dans la scène.
		 *
		 * Cette fonction ajoute immédiatement un nouvel objet frère à un objet donné dans la scène sans attendre la prochaine mise à jour.
		 *
		 * @param _gameObject L'objet auquel ajouter le nouveau frère.
		 * @param _newSibling Le nouvel objet frère à ajouter.
		 * @return Un pointeur vers le nouvel objet frère ajouté.
		 */
		GameObject* AddSiblingObjectImmediate(GameObject* _gameObject, GameObject* _newSibling) {}

#pragma region Remove

		/**
		 * @brief Supprime et détruit tous les objets de la scène à la fin de la trame.
		 */
		void RemoveAllObjects();

		/**
		 * @brief Supprime et détruit tous les objets de la scène immédiatement.
		 */
		void RemoveAllObjectsImmediate(){}

		/**
		 * @brief Supprime et détruit tous les objets de l'éditeur de la scène immédiatement.
		 */
		void RemoveAllEditorObjectsImmediate(){}

		/**
		 * @brief Supprime et détruit un objet de la scène.
		 *
		 * @param _gameObjectId L'identifiant de l'objet à supprimer.
		 * @param _bDestroy Booléen indiquant s'il faut détruire l'objet ou non.
		 */
		void RemoveObject(const GameObject::id_t& _gameObjectId, bool _bDestroy);

		/**
		 * @brief Supprime et détruit un objet de la scène.
		 *
		 * @param _gameObject L'objet à supprimer.
		 * @param _bDestroy Booléen indiquant s'il faut détruire l'objet ou non.
		 */
		void RemoveObject(const GameObject* _gameObject, bool _bDestroy);

		//void RemoveObjectImmediate(const GameObject::id_t& _gameObjectID, bool _bDestroy);
		//void RemoveObjectImmediate(GameObject* _gameObject, bool _bDestroy);

		/**
		 * @brief Supprime et détruit une liste d'objets de la scène.
		 *
		 * @param _gameObjects Le vecteur d'identifiants des objets à supprimer.
		 * @param _bDestroy Booléen indiquant s'il faut détruire les objets ou non.
		 */
		void RemoveObjects(const std::vector<GameObject::id_t>& _gameObjects, bool _bDestroy);

		/**
		 * @brief Supprime et détruit une liste d'objets de la scène.
		 *
		 * @param _gameObjects Le vecteur d'objets à supprimer.
		 * @param _bDestroy Booléen indiquant s'il faut détruire les objets ou non.
		 */
		void RemoveObjects(const std::vector<GameObject*>& _gameObjects, bool _bDestroy);

		/**
		 * @brief Supprime et détruit immédiatement une liste d'objets de la scène.
		 *
		 * @param _gameObjects Le vecteur d'identifiants des objets à supprimer.
		 * @param _bDestroy Booléen indiquant s'il faut détruire les objets ou non.
		 */
		void RemoveObjectsImmediate(const std::vector<GameObject::id_t>& _gameObjects, bool _bDestroy){}

		/**
		 * @brief Supprime et détruit immédiatement une liste d'objets de la scène.
		 *
		 * @param _gameObjects Le vecteur d'objets à supprimer.
		 * @param _bDestroy Booléen indiquant s'il faut détruire les objets ou non.
		 */
		void RemoveObjectsImmediate(const std::vector<GameObject*>& _gameObjects, bool _bDestroy){}

#pragma endregion

		/**
		 * @brief Obtient l'identifiant du premier objet avec l'étiquette spécifiée.
		 *
		 * @param _tag L'étiquette de l'objet recherché.
		 * @return L'identifiant du premier objet avec l'étiquette spécifiée, ou ID_INVALID si aucun objet correspondant n'est trouvé.
		 */
		GameObject::id_t FirstObjectWithTag(const std::string& _tag){}

		/**
		 * @brief Charge les objets de jeu pour les tests.
		 *
		 * Cette fonction charge les objets de jeu pour les tests.
		 */
		void TestLoadGameObjects();

		/**
		 * @brief Ajoute un objet de jeu et tous ses enfants à un vecteur.
		 *
		 * @param _gameObjects Le vecteur où ajouter les objets.
		 * @param _gameObject L'objet de jeu à ajouter.
		 */
		void AddGameObjectInVector(std::vector<GameObject*>& _gameObjects, GameObject* _gameObject) {
			_gameObjects.push_back(_gameObject);
			for (auto& gameObject : _gameObject->GetChildren()) {
				AddGameObjectInVector(_gameObjects, gameObject);
			}
		}

		/**
		 * @brief Crée un nouvel GameObject représentant un cube.
		 *
		 * @return Un pointeur vers le GameObject créé.
		 */
		GameObject* CreateCubeGameObject(int _type);

		/**
		 * @brief Crée un nouvel GameObject représentant une lumière.
		 *
		 * @return Un pointeur vers le GameObject créé.
		 */
		GameObject* CreateLightGameObject();

		/**
		 * @brief Crée un nouvel GameObject représentant un plan.
		 *
		 * @return Un pointeur vers le GameObject créé.
		 */
		GameObject* CreatePlaneGameObject();

		json toJson() const {
			json j;
			j["id"] = id;
			j["name"] = name;
			j["fileName"] = fileName;
			j["isActive"] = isActive;
			// Créer un tableau JSON pour stocker les GameObjects
			json gameObjectsJson = json::array();
			for (const auto& gameObjectPtr : rootObjects) {
				if (gameObjectPtr) { // Vérifier si le pointeur est valide
					// Appeler toJson() sur le GameObject pointé et l'ajouter au tableau
					gameObjectsJson.push_back(gameObjectPtr->toJson());
				}
			}
			j["gameObjects"] = gameObjectsJson; // Assigner le tableau JSON à la clé "gameObjects"
			return j;
		}

		int id = 0;
		std::string name; /**< Le nom de la scène. */
		std::string fileName; /**< Le nom du fichier de la scène. */

		std::vector<GameObject::id_t> pendingDestroyObjects; /**< Liste des objets à détruire à la fin de la mise à jour de cette frame. */
		std::vector<GameObject*> pendingAddObjects; /**< Liste des objets à ajouter en tant qu'objets racine à la fin de la mise à jour de cette frame. */
		std::vector<GameObject*> rootObjects; /**< Liste des objets racine de la scène. */
		std::vector<GameObject::id_t> pendingRemoveObjects; /**< Liste des objets à supprimer. */

		const GameObject::id_t invalidGameObjectId = {}; /**< Identifiant invalide d'un GameObject. */

		bool bInitialized = false; /**< Indique si la scène est initialisée. */
		bool bLoaded = false; /**< Indique si la scène est chargée. */
		bool isActive = false;
		GameObject* CreateVaseGameObject(int _type = 0);
		GameObject* CreateGirlGameObject();
		GameObject* CreateNoobGameObject();
		GameObject* CreateSphereGameObject();
		GameObject* CreateMultipleGameObject(int _type = 0);
};
