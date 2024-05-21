#include "Scene/SceneManager.h"
#include <fstream>
#include <sstream>

#include "Engine/CoreEngine.h"
#include "Modules/ModuleManager.h"
#include "Modules/WindowModule.h"


/**
 * @brief Crée une nouvelle scène.
 * @param _name Nom de la nouvelle scène.
 * @param _isActive Indique si la nouvelle scène est active.
 */
void SceneManager::CreateScene(const std::string& _name, bool _isActive)
{
	listScenes.insert(std::make_pair(_name, _isActive));
	scenes.push_back(std::make_unique<BaseScene>(_name, scenes.size()));
}

/**
 * @brief Récupère le nombre de scènes.
 * @return Le nombre de scènes.
 */
int SceneManager::SceneCount() const
{
	return listScenes.size();
}

/**
 * @brief Récupère le nom de la scène active.
 * @return Le nom de la scène active, ou "None" si aucune scène n'est active.
 */
std::string SceneManager::GetActiveScene() const
{
	for (const auto scene : listScenes)
	{
		std::string name = scene.first;
		if (bool is_active = scene.second) return name;
	}
	return "None";
}

/**
 * @brief Récupère le nom de la première scène dans la liste des scènes.
 * @return Le nom de la première scène, ou "None" si la liste des scènes est vide.
 */
std::string SceneManager::GetListScenes() const
{
	for (const auto scene : listScenes)
	{
		std::string name = scene.first;
		return name;
	}
	return "None";
}

/**
 * @brief Récupère la scène à un index donné dans la liste des scènes.
 * @param _index Index de la scène à récupérer.
 * @return Une paire contenant le nom de la scène et son état d'activité, ou une paire "None" si l'index est invalide.
 */
std::pair<std::string, bool> SceneManager::GetSceneAt(const int _index)
{
	if (!listScenes.empty())
	{
		auto find_name = listScenes.begin();
		std::advance(find_name, _index);
		return *find_name;
	}
	return {"None", false};
}


/**
 * @brief Supprime une scène de la liste des scènes.
 * @param _sceneName Nom de la scène à supprimer.
 */
void SceneManager::DestroyScene(const std::string& _sceneName, int index)
{
	const auto it = listScenes.find(_sceneName);
	if (it != listScenes.end()) listScenes.erase(it);
	auto sceneToRemove = scenes.begin() + index;
	scenes.erase(sceneToRemove);
}

/**
 * @brief Définit la scène courante par son index dans la liste des scènes.
 * @param _sceneIndex Index de la scène à définir comme scène courante.
 */
void SceneManager::SetCurrentScene(const int _sceneIndex)
{
	if (_sceneIndex >= 0 && _sceneIndex < static_cast<int>(scenes.size())) {
		currentSceneIndex = _sceneIndex;
		mainScene = scenes[currentSceneIndex].get();
		mainScene->SetActive(true);
	}
}

/**
 * @brief Active la scène suivante dans la liste des scènes.
 * Si la liste des scènes n'est pas vide, l'index de la scène courante est mis à jour pour passer à la scène suivante circulairement.
 */
void SceneManager::SetNextSceneActive()
{
	if (!scenes.empty()) currentSceneIndex = (currentSceneIndex + 1) % scenes.size();
}

/**
 * @brief Active la scène précédente dans la liste des scènes.
 * Si la liste des scènes n'est pas vide, l'index de la scène courante est mis à jour pour passer à la scène précédente circulairement.
 */
void SceneManager::SetPreviousSceneActive()
{
	if (!scenes.empty()) currentSceneIndex = (currentSceneIndex - 1 + scenes.size()) % scenes.size();
}


/**
 * @brief Récupère la scène courante.
 * @return Un pointeur vers la scène courante si l'index de la scène courante est valide, sinon nullptr.
 */
BaseScene* SceneManager::GetCurrentScene() const
{
	return (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size()))
		       ? scenes[currentSceneIndex].get()
		       : nullptr;
}

/**
 * @brief Vérifie si un fichier de scène existe dans la liste des scènes.
 * @param fileName Nom du fichier de scène à rechercher.
 * @return true si le fichier de scène existe, sinon false.
 */
bool SceneManager::SceneFileExists(const std::string& fileName) const
{
	for (const auto& scene : scenes)
	{
		if (scene->GetFileName() == fileName) return true;
	}

	return false;
}


//--------------------------A MODIFIER TRES SIMPLIFIE------------------------------------
GameObject* SceneManager::CreateGameObjectFromSceneData()
{
	const auto game_object = new GameObject();

	return game_object;
}

/**
 * @brief Charge une scène à partir d'un fichier.
 * @param _fileName Nom du fichier de scène à charger.
 * @return true si la scène a été chargée avec succès, sinon false.
 */
bool SceneManager::LoaddSceneFromFile(const std::string& _fileName)
{
	// Vérifier si le fichier existe
	if (!SceneFileExists(_fileName))
	{
		std::cerr << "Erreur : Le fichier de scène '" << _fileName << "' n'existe pas." << std::endl;
		return false;
	}

	// Ouvrir le fichier de scène en lecture
	std::ifstream file(_fileName);
	if (!file.is_open())
	{
		std::cerr << "Erreur : Impossible d'ouvrir le fichier de scène '" << _fileName << "'." << std::endl;
		return false;
	}

	// Lire le contenu du fichier de scène
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string scene_data = buffer.str();

	// Fermer le fichier
	file.close();

	// Analyser les données de la scène
	std::istringstream scene_stream(scene_data);
	std::string        scene_object_name;
	while (scene_stream >> scene_object_name)
	{
		if (GameObject* game_object = CreateGameObjectFromSceneData())
			// Ajouter l'objet de scène à la scène courante
			GetCurrentScene()->AddRootObject(game_object);
		else std::cerr << "Erreur : Impossible de créer l'objet de scène '" << scene_object_name << "'." << std::endl;
	}

	std::cout << "Chargement de la scène depuis le fichier '" << _fileName << "' terminé." << std::endl;
	return true;
}

/**
 * @brief Lancer une scène
 * @param _sceneName Nom de la scène à lancer
 */
void SceneManager::RunScene(const std::string& _sceneName)
{
	if(listScenes.contains(_sceneName))
	{
		if(sceneActive)
		{
			GetCurrentScene()->Release();
			GetCurrentScene()->Finalize();
		}
		SetMainScene(_sceneName);
		GetCurrentScene()->Init();
		GetCurrentScene()->Start();
	}
}


/**
 * @brief Définit la scène principale à utiliser.
 * @param _sceneName Nom de la scène à définir comme scène principale.
 */
void SceneManager::SetMainScene(const std::string& _sceneName)
{
	if (const auto it = listScenes.find(_sceneName);
		it != listScenes.end()) currentSceneIndex = std::distance(listScenes.begin(), it);
	for (const auto& scene : scenes)
	{
		if(scene->name == _sceneName)
		{
			mainScene = scene.get();
			break;
		}
	}
}


/**
 * @brief Récupère une scène par son nom.
 * @param _sceneName Nom de la scène à récupérer.
 * @return Un pointeur vers la scène si elle existe, sinon nullptr.
 */
BaseScene* SceneManager::GetScene(const std::string& _sceneName)
{
	if (const auto it = listScenes.find(_sceneName); it != listScenes.end()) return scenes[std::distance(
		listScenes.begin(), it)].get();
	return nullptr;
}

/**
 * @brief Renomme une scène dans la liste des scènes.
 * @param _oldName Ancien nom de la scène à renommer.
 * @param _newName Nouveau nom de la scène.
 */
void SceneManager::RenameScene(const std::string& _oldName, const std::string& _newName)
{
	if (const auto it = listScenes.find(_oldName); it != listScenes.end())
	{
		listScenes[_newName] = it->second;
		listScenes.erase(it);
	}
}

void SceneManager::Init()
{
	Module::Init();
	CreateScene("SampleScene", true);
	SetMainScene("SampleScene");
}

/**
 * @brief Démarre le gestionnaire de scènes.
 * Initialise le gestionnaire de fenêtres.
 */
void SceneManager::Start()
{
	Module::Start();
	RunScene("SampleScene");
	windowModule = moduleManager->GetModule<WindowModule>();
}

void SceneManager::FixedUpdate()
{
	Module::FixedUpdate();
	if (mainScene) mainScene->FixedUpdate();
}

void SceneManager::Update()
{
	Module::Update();
	if(mainScene)
	{
		if(Engine::GetInstance()->GetEngineMode() == EngineMode::Editor) mainScene->UpdateEditor();
		else if(Engine::GetInstance()->GetEngineMode() == EngineMode::Play) mainScene->Update();
	}
}

void SceneManager::PreRender()
{
	Module::PreRender();
	if (mainScene) mainScene->PreRender();
}

void SceneManager::Render()
{
	Module::Render();
	if (mainScene) mainScene->Render();
}

void SceneManager::RenderGui()
{
	Module::RenderGui();
	if (mainScene) mainScene->RenderGui();
}

void SceneManager::PostRender()
{
	Module::PostRender();
	if (mainScene) mainScene->PostRender();
}

/**
 * @brief Détruit toutes les scènes et vide la liste des scènes.
 * L'index de la scène courante est réinitialisé à -1.
 */
void SceneManager::Release()
{
	if (mainScene) mainScene->Release();
	if (mainScene) mainScene->Finalize();
	if (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size()))
		scenes[currentSceneIndex]->
		Finalize();

	listScenes.clear();
	scenes.clear();
	currentSceneIndex = -1;
}

void SceneManager::Finalize()
{
	Module::Finalize();
}
