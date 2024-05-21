#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "BaseScene.h"
#include "Modules/Module.h"
#include "Modules/WindowModule.h"
#include "Scene/BaseScene.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace fs = std::filesystem;

/**
 * @brief Gestionnaire de sc�ne.
 *
 * G�re les op�rations li�es aux sc�nes, telles que la cr�ation, la destruction et la manipulation des objets de la sc�ne.
 */
class SceneManager final : public Module
{
public:
	SceneManager() = default;

	SceneManager(const SceneManager&&) = delete;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;


#pragma region Getter

	/**
	 * @brief Obtient la sc�ne principale.
	 *
	 * @return Un pointeur vers la sc�ne principale.
	 */
	[[nodiscard]] BaseScene* GetMainScene() const { return mainScene; }

	/**
	 * @brief Obtient une sc�ne sp�cifi�e par son nom.
	 *
	 * @param _sceneName Le nom de la sc�ne � r�cup�rer.
	 * @return Un pointeur vers la sc�ne demand�e, ou nullptr si elle n'est pas trouv�e.
	 */
	BaseScene* GetScene(const std::string& _sceneName);

	/**
	 * @brief Obtient le nom de la sc�ne active.
	 *
	 * @return Le nom de la sc�ne active.
	 */
	std::string GetActiveScene() const;

	/**
	 * @brief Obtient une liste des noms de toutes les sc�nes disponibles.
	 *
	 * @return Une cha�ne contenant les noms de toutes les sc�nes disponibles.
	 */
	std::string GetListScenes() const;

	/**
	 * @brief Obtient la sc�ne � l'index sp�cifi�.
	 *
	 * @param _index L'index de la sc�ne � r�cup�rer.
	 * @return Une paire contenant le nom de la sc�ne et un bool�en indiquant si elle est active.
	 */
	std::pair<std::string, bool> GetSceneAt(int _index);

	/**
	 * @brief Obtient la sc�ne actuellement active.
	 *
	 * @return Un pointeur vers la sc�ne actuellement active.
	 */
	BaseScene* GetCurrentScene() const;

	/**
	 * @brief Obtient une r�f�rence � la liste des sc�nes.
	 *
	 * @return Une r�f�rence � la liste des sc�nes.
	 */
	std::vector<std::unique_ptr<BaseScene>>& GetScenes() { return scenes; }

	std::map<std::string, bool> GetListScenes2() { return listScenes; }

#pragma endregion

#pragma region Setter

	/**
	 * @brief D�finit la sc�ne principale.
	 *
	 * @param _sceneName Le nom de la sc�ne principale.
	 */
	void SetMainScene(const std::string& _sceneName);

	/**
		 * @brief D�finit la sc�ne � activer en fonction de son index dans la liste des sc�nes.
		 *
		 * @param _sceneIndex L'index de la sc�ne � activer.
		 */
	void SetCurrentScene(int _sceneIndex);

	/**
	 * @brief Active la sc�ne suivante dans la liste des sc�nes.
	 */
	void SetNextSceneActive();

	/**
	 * @brief Active la sc�ne pr�c�dente dans la liste des sc�nes.
	 */
	void SetPreviousSceneActive();

	void SetListScenes(std::map<std::string, bool> _newlistScenes) { listScenes = _newlistScenes; }

#pragma endregion

	/**
	 * @brief Ex�cute une sc�ne sp�cifi�e.
	 *
	 * @param _sceneName Le nom de la sc�ne � ex�cuter.
	 */
	void RunScene(const std::string& _sceneName);


	/**
	 * @brief Renomme une sc�ne.
	 *
	 * @param _oldName Le nom actuel de la sc�ne.
	 * @param _newName Le nouveau nom de la sc�ne.
	 */
	void RenameScene(const std::string& _oldName, const std::string& _newName);

	/**
	 * @brief Cr�e une nouvelle sc�ne avec le nom sp�cifi�.
	 *
	 * @param _name Le nom de la nouvelle sc�ne.
	 * @param _isActive Indique si la nouvelle sc�ne doit �tre active ou non.
	 */
	void CreateScene(const std::string& _name, bool _isActive);

	/**
	 * @brief D�truit une sc�ne sp�cifi�e par son nom.
	 *
	 * @param _sceneName Le nom de la sc�ne � d�truire.
	 */
	void DestroyScene(const std::string& _sceneName, int index);

	/**
	 * @brief Charge une sc�ne � partir d'un fichier avec le nom sp�cifi�.
	 *
	 * @param _fileName Le nom du fichier de la sc�ne � charger.
	 * @return true si la sc�ne a �t� charg�e avec succ�s, sinon false.
	 */
	bool LoaddSceneFromFile(const std::string& _fileName);

	/**
	 * @brief Obtient le nombre total de sc�nes.
	 *
	 * @return Le nombre total de sc�nes.
	 */
	int SceneCount() const;

#pragma region Event

	/**
		* @brief Initialise le module.
		*/
	void Init() override;

	/**
	 * @brief D�marre le module.
	 */
	void Start() override;

	/**
	 * @brief Effectue une mise � jour fixe du module.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met � jour le module.
	 */
	void Update() override;

	/**
	 * @brief Fonction pr�-rendu du module.
	 */
	void PreRender() override;

	/**
	 * @brief Rendu du module.
	 */
	void Render() override;

	/**
	 * @brief Rendu de l'interface graphique du module.
	 */
	void RenderGui() override;

	/**
	 * @brief Fonction post-rendu du module.
	 */
	void PostRender() override;

	/**
	 * @brief Lib�re les ressources utilis�es par le module.
	 */
	void Release() override;

	/**
	 * @brief Finalise le module.
	 */
	void Finalize() override;

#pragma endregion

	void SaveSceneToFile(const BaseScene& _scene, const std::string& _filename)
	{
		// Convertir la sc�ne en JSON
		const json scene_json = _scene.toJson();

		// Enregistrer le JSON dans un fichier
		std::ofstream file(_filename);
		if (file.is_open())
		{
			file << std::setw(4) << scene_json << std::endl;
			file.close();
			std::cout << "La sc�ne a �t� enregistr�e dans " << _filename << std::endl;
		}
		else
		{
			std::cerr << "Erreur: Impossible d'ouvrir le fichier pour l'enregistrement" << std::endl;
		}
	}

	void SaveScenesToFile(const std::string& _projectName) const
	{
		std::string directory = "Saves/Projects/" + _projectName;
		// V�rifier si le dossier existe, sinon le cr�er
		if (!fs::exists(directory))
		{
			fs::create_directories(directory);
		}


		for (const auto& scene_ptr : scenes)
		{
			const BaseScene& scene = *scene_ptr; // D�r�f�rencer le pointeur unique pour acc�der � l'objet Scene
			std::string filename = directory + "/" + scene.name + ".json";
			// Convertir la sc�ne en JSON
			json scene_json = scene.toJson();

			// Enregistrer le JSON dans un fichier
			if (std::ofstream file(filename); file.is_open())
			{
				file << std::setw(4) << scene_json << std::endl;
				file.close();
				std::cout << "La sc�ne a �t� enregistr�e dans " << filename << std::endl;
			}
			else
			{
				std::cerr << "Erreur: Impossible d'ouvrir le fichier pour l'enregistrement" << std::endl;
			}
		}
	}

	void LoadAllScenes(const std::string& _projectName)
	{
		if (const std::string directory = "Saves/Projects/" + _projectName; fs::exists(directory) &&
			fs::is_directory(directory))
		{
			for (const auto& entry : fs::directory_iterator(directory))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json")
				{
					const std::string filename = entry.path().filename().stem().string();
					LoadScenesFromFile(_projectName, filename);
				}
			}
		}
		else
		{
			std::cerr << "Le r�pertoire de sauvegarde n'existe pas : " << directory << std::endl;
		}
	}
	void LoadAllScenesSPath(const std::string& _sPath)
	{
		if (const std::string directory = _sPath; fs::exists(directory) &&
			fs::is_directory(directory))
		{
			for (const auto& entry : fs::directory_iterator(directory))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json")
				{
					const std::string filename = entry.path().filename().stem().string();
					LoadScenesFromFile(_sPath, filename);
				}
			}
		}
		else
		{
			std::cerr << "Le r�pertoire de sauvegarde n'existe pas : " << directory << std::endl;
		}
	}

	void LoadScenesFromFile(const std::string& _projectName, const std::string& _sceneName)
	{
		const std::string directory = "Saves/Projects/" + _projectName;
		const std::string filename = directory + "/" + _sceneName + ".json";
		// V�rifier si le fichier existe
		if (fs::exists(filename))
		{
			// Lire le fichier JSON
			std::ifstream file(filename);
			if (file.is_open())
			{
				json sceneJson;
				file >> sceneJson;
				file.close();

				// R�cup�rer les donn�es de la sc�ne � partir du JSON
				const int id = sceneJson["id"];
				const std::string name = sceneJson["name"];
				CreateScene(name, id);
				// R�cup�rer les GameObjects
				for (const auto& gameObjectJson : sceneJson["gameObjects"])
				{
					int gameObjectId = gameObjectJson["id"];
					std::string gameObjectName = gameObjectJson["name"];
					GameObject* game_object = new GameObject();
					game_object->fromJson(gameObjectJson);
					GetScene(name)->rootObjects.push_back(game_object);
				}

				std::cout << "La sc�ne a �t� charg�e depuis " << filename << std::endl;
			}
			else
			{
				std::cerr << "Erreur: Impossible d'ouvrir le fichier pour la lecture" << std::endl;
			}
		}
		else
		{
			std::cerr << "Erreur: Le fichier " << filename << " n'existe pas" << std::endl;
		}
	}
	void LoadSceneFromFile(const std::string& _filename)
	{
		const std::string directory = "Saves/Projects/project";
		const std::string filename = directory + "/" + _filename ;
		// V�rifier si le fichier existe
		if (fs::exists(filename))
		{
			// Lire le fichier JSON
			std::ifstream file(filename);
			if (file.is_open())
			{
				json sceneJson;
				file >> sceneJson;
				file.close();

				// R�cup�rer les donn�es de la sc�ne � partir du JSON
				const int id = sceneJson["id"];
				const std::string name = sceneJson["name"];
				CreateScene(name, id);
				// R�cup�rer les GameObjects
				for (const auto& gameObjectJson : sceneJson["gameObjects"])
				{
					int gameObjectId = gameObjectJson["id"];
					std::string gameObjectName = gameObjectJson["name"];
					GameObject* game_object = new GameObject();
					game_object->fromJson(gameObjectJson);
					GetScene(name)->rootObjects.push_back(game_object);
				}

				std::cout << "La sc�ne a �t� charg�e depuis " << filename << std::endl;
			}
			else
			{
				std::cerr << "Erreur: Impossible d'ouvrir le fichier pour la lecture" << std::endl;
			}
		}
		else
		{
			std::cerr << "Erreur: Le fichier " << filename << " n'existe pas" << std::endl;
		}
	}

private:
	/**
	 * @brief V�rifie si le fichier de la sc�ne existe.
	 *
	 * @param _filePath Le chemin du fichier de la sc�ne.
	 * @return true si le fichier de la sc�ne existe, sinon false.
	 */
	bool SceneFileExists(const std::string& _filePath) const;

	/**
	 * @brief Cr�e un GameObject � partir des donn�es de la sc�ne.
	 *
	 * @return Un pointeur vers le GameObject cr��.
	 */
	GameObject* CreateGameObjectFromSceneData();

	WindowModule* windowModule = nullptr; /**< Le module de la fen�tre associ� au SceneManager. */

	std::map<std::string, bool> listScenes; /**< La liste des noms de sc�nes avec leur �tat actif. */

	int sceneCount; /**< Le nombre total de sc�nes. */

	bool sceneActive; /**< Indique si une sc�ne est active. */

	BaseScene* mainScene = nullptr; /**< La sc�ne principale du SceneManager. */

	std::vector<std::unique_ptr<BaseScene>> scenes; /**< Les sc�nes g�r�es par le SceneManager. */

	int currentSceneIndex = -1; /**< L'index de la sc�ne actuellement active. */
};
