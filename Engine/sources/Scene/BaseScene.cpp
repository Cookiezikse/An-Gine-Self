#include "Scene/BaseScene.h"
#include <fstream>
#include <iostream>
#include <string>

#include "Engine/CoreEngine.h"
#include "GameObject/Components/Transform.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/GirlGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/MultipleGameObject.h"
#include "GameObject/PreGameObject/NoobGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"
#include "GameObject/PreGameObject/SphereGameObject.h"
#include "GameObject/PreGameObject/VaseGameObject.h"
#include "Modules/rhi.h"
#include "Modules/TimeModule.h"


GameObject* BaseScene::AddRootObject(GameObject* _gameObject)
{
	if (_gameObject == nullptr) return nullptr;

	pendingAddObjects.push_back(_gameObject);

	return _gameObject;
}

void BaseScene::RemoveAllObjects()
{
	for (const GameObject* root_object : rootObjects)
	{
		pendingDestroyObjects.push_back(root_object->GetId());
	}
}


bool BaseScene::IsInitialized() const
{
	return bInitialized;
}

void BaseScene::SetName(const std::string& _name)
{
	name = _name;
}

std::string BaseScene::GetName() const
{
	return name;
}

bool BaseScene::IsUsingSaveFile() const
{
	return false;
}


std::vector<GameObject*>& BaseScene::GetRootObjects()
{
	return rootObjects;
}


std::string BaseScene::GetFileName() const
{
	return fileName;
}

bool BaseScene::SetFileName(const std::string& _fileName, bool _bDeletePreviousFiles) const
{
	bool success = false;

	if (fileName == _fileName) success = true;
	return success;
}

std::string BaseScene::GetDefaultRelativeFilePath() const
{
	//A MODIFIIIIEEEEEEEEEERRRRRR
	const std::string DEFAULT_SCENE_DIRECTORY = "assets/scenes/";
	return DEFAULT_SCENE_DIRECTORY + fileName;
}

bool BaseScene::FileExists(const std::string& _filePath)
{
	const std::ifstream file(_filePath);
	return file.good();
}

void BaseScene::DeleteSaveFiles()
{
	const std::string default_save_file_path = "assets/scenes/" + fileName;
	const std::string saved_save_file_path = "saved/scenes/" + fileName;

	const bool b_default_file_exists = FileExists(default_save_file_path);

	if (const bool b_saved_file_exists = FileExists(saved_save_file_path); b_saved_file_exists || b_default_file_exists)
	{
		std::cout << "Deleting scene's save files from " << fileName << std::endl;

		if (b_default_file_exists)
		{
			if (remove(default_save_file_path.c_str()) !=
				0)
				std::cerr << "Error deleting default save file: " << default_save_file_path << std::endl;
			else std::cout << "Default save file deleted successfully." << std::endl;
		}

		if (b_saved_file_exists)
		{
			if (remove(saved_save_file_path.c_str()) !=
				0)
				std::cerr << "Error deleting saved save file: " << saved_save_file_path << std::endl;
			else std::cout << "Saved save file deleted successfully." << std::endl;
		}
	}
}

bool BaseScene::Contains(const std::vector<GameObject::id_t>& _container, const GameObject::id_t& _value)
{
	return std::ranges::find(_container, _value) != _container.end();
}

void BaseScene::RemoveObject(const GameObject::id_t& _gameObjectId, const bool _bDestroy)
{
	if (_bDestroy)
	{
		if (!Contains(pendingDestroyObjects, _gameObjectId)) pendingDestroyObjects.push_back(_gameObjectId);
	}
	else
	{
		if (!Contains(pendingRemoveObjects, _gameObjectId)) pendingRemoveObjects.push_back(_gameObjectId);
	}
}

void BaseScene::RemoveObject(const GameObject* _gameObject, const bool _bDestroy)
{
	RemoveObject(_gameObject->GetId(), _bDestroy);
}

void BaseScene::RemoveObjects(const std::vector<GameObject::id_t>& _gameObjects, const bool _bDestroy)
{
	if (_bDestroy) pendingDestroyObjects.insert(pendingDestroyObjects.end(), _gameObjects.begin(), _gameObjects.end());
	else pendingRemoveObjects.insert(pendingRemoveObjects.end(), _gameObjects.begin(), _gameObjects.end());
}

void BaseScene::RemoveObjects(const std::vector<GameObject*>& _gameObjects, const bool _bDestroy)
{
	if (_bDestroy)
	{
		pendingDestroyObjects.reserve(pendingDestroyObjects.size() + _gameObjects.size());
		for (const GameObject* game_object : _gameObjects)
		{
			if (!Contains(pendingDestroyObjects, game_object->GetId()))
				pendingDestroyObjects.push_back(
					game_object->GetId());
		}
	}
	else
	{
		pendingRemoveObjects.reserve(pendingRemoveObjects.size() + _gameObjects.size());
		for (const GameObject* game_object : _gameObjects)
		{
			if (!Contains(pendingRemoveObjects, game_object->GetId()))
				pendingRemoveObjects.push_back(
					game_object->GetId());
		}
	}
}

GameObject* BaseScene::CreateGameObject()
{
	const auto game_object = new GameObject();
	pendingAddObjects.push_back(game_object);
	return game_object;
}

void BaseScene::DestroyGameObject(const GameObject* _gameObject)
{
	if (_gameObject != nullptr) RemoveObject(_gameObject, true);
}

GameObject* BaseScene::GetGameObjectById(const GameObject::id_t& _gameObjectId) const
{
	for (GameObject* root_object : rootObjects)
	{
		if (root_object->GetId() == _gameObjectId) return root_object;
	}
	return nullptr;
}

std::vector<GameObject*> BaseScene::FindGameObjectsByName(const std::string& name) const
{
	std::vector<GameObject*> result;
	for (const GameObject* root_object : rootObjects)
	{
		std::vector<GameObject*> found = root_object->FindChildrenByName(name);
		result.insert(result.end(), found.begin(), found.end());
	}
	return result;
}


void BaseScene::Init()
{
}

void BaseScene::Start()
{
	isActive = true;
	TestLoadGameObjects();
}

void BaseScene::FixedUpdate()
{
}

void BaseScene::Update()
{
	if (isActive)
	{
		// Mettez � jour chaque objet de la sc�ne avec le delta time
		for (auto* obj : pendingAddObjects)
		{
			rootObjects.push_back(obj);
		}
		pendingAddObjects.clear();

		for (const GameObject* root_object : rootObjects)
		{
			root_object->Update(); // Mettez � jour chaque objet avec le delta time
		}
	}
}

void BaseScene::UpdateEditor()
{
	if (isActive)
	{
		// Mettez � jour chaque objet de la sc�ne avec le delta time
		for (auto* obj : pendingAddObjects)
		{
			rootObjects.push_back(obj);
		}
		pendingAddObjects.clear();

		for (const GameObject* root_object : rootObjects)
		{
			root_object->UpdateEditor(); // Mettez � jour chaque objet avec le delta time
		}
	}
}

void BaseScene::PreRender()
{
}

void BaseScene::Render()
{
	// Rendu de chaque objet de la sc�ne
	for (GameObject* root_object : rootObjects)
	{
		//RenderObject(rootObject);
	}
}

void BaseScene::RenderGui()
{
}

void BaseScene::PostRender()
{
}

void BaseScene::Release()
{
	isActive = false;
}

void BaseScene::Finalize()
{
	bLoaded = false;
	bInitialized = false;

	for (GameObject* root_object : rootObjects)
	{
		if (root_object != nullptr)
		{
			for (Component* component : root_object->GetComponents())
			{
				root_object->RemoveComponent(component);
			}
			delete root_object;
		}
	}
	rootObjects.clear();
}


void BaseScene::TestLoadGameObjects()
{
	lve::LveDevice* _p_lveDevice = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(
		*_p_lveDevice, "Models\\flat_vase.obj");

	const auto flat_vase_go = GameObject::CreatePGameObject();
	flat_vase_go->SetName("FlatVase");
	flat_vase_go->SetModel(lve_model);
	flat_vase_go->SetFileModel("Models\\flat_vase.obj");
	flat_vase_go->GetTransform()->SetPosition(glm::vec3{-.5f, .5f, 0.f});
	flat_vase_go->GetTransform()->SetScale(glm::vec3{3.f, 1.5f, 3.f});
	flat_vase_go->SetTexture(1);
	rootObjects.push_back(flat_vase_go);


	lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\smooth_vase.obj");
	const auto smooth_vase_go = GameObject::CreatePGameObject();
	smooth_vase_go->SetName("SmoothVase");
	smooth_vase_go->SetFileModel("Models\\smooth_vase.obj");
	smooth_vase_go->SetModel(lve_model);
	smooth_vase_go->GetTransform()->SetPosition(glm::vec3{.5f, .5f, 0.f});
	smooth_vase_go->GetTransform()->SetScale(glm::vec3{3.f, 1.5f, 3.f});
	rootObjects.push_back(smooth_vase_go);

	const auto quad_go =
		lve::PlaneGameObject::Create(*_p_lveDevice, glm::vec3{.0f, .5f, 0.f}, glm::vec3{3.f, 1.f, 3.f});
	quad_go->SetName("QuadGo");
	quad_go->SetTexture(1);
	rootObjects.push_back(quad_go);

	lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\viking_room.obj");
	const auto viking = GameObject::CreatePGameObject();
	viking->SetName("Viking");
	viking->SetFileModel("Models\\viking_room.obj");
	viking->SetModel(lve_model);
	viking->GetTransform()->SetPosition(glm::vec3{0.f, 0.f, 5.f});
	viking->GetTransform()->SetScale(glm::vec3{3.f, 3.f, 3.f});
	viking->GetTransform()->SetRotation(glm::vec3{glm::radians(90.0f), glm::radians(90.0f), 0.0f});
	viking->SetTexture(2);
	rootObjects.push_back(viking);

	const auto cube = lve::CubeGameObject::Create(*_p_lveDevice);
	cube->SetName("Cube");
	rootObjects.push_back(cube);

	const auto color_cube = lve::CubeGameObject::CreateColor(*_p_lveDevice, glm::vec3{0.f, 0.f, 10.f});
	color_cube->SetName("ColorCube");
	rootObjects.push_back(color_cube);

	const std::vector<glm::vec3> light_colors{
		{1.f, .1f, .1f},
		{.1f, .1f, 1.f},
		{.1f, 1.f, .1f},
		{1.f, 1.f, .1f},
		{.1f, 1.f, 1.f},
		{1.f, 1.f, 1.f}
	};

	for (int i = 0; i < light_colors.size(); i++)
	{
		auto point_light = lve::LightGameObject::Create(0.2f, 0.1f);
		point_light->SetName("Point Light " + std::to_string(i));
		point_light->SetColor(light_colors[i]);
		auto rotate_light = rotate(
			glm::mat4(1.f),
			(i * glm::two_pi<float>()) / light_colors.size(),
			{0.f, -1.f, 0.f});
		point_light->GetTransform()->SetPosition(glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f)));
		rootObjects.push_back(point_light);
	}
}

GameObject* BaseScene::CreateCubeGameObject(const int _type)
{
	if (_type == 0)
	{
		lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
		return lve::CubeGameObject::Create(*device);
	}
	if (_type == 1)
	{
		lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
		return lve::CubeGameObject::CreateColor(*device);
	}
	return nullptr;
}

GameObject* BaseScene::CreateLightGameObject()
{
	float intensity = 10.0f; // Exemple d'intensité pour la lumière
	float radius = 1.0f; // Rayon de la lumière
	auto position = glm::vec3(0.0f, 0.0f, 0.0f); // Position initiale
	auto rotation = glm::vec3(0.0f, 0.0f, 0.0f); // Rotation initiale
	auto color = glm::vec3(1.0f, 1.0f, 1.0f); // Couleur de la lumière (blanche)

	// Crée un GameObject représentant une lumière avec les paramètres spécifiés
	return lve::LightGameObject::Create(intensity, radius, position, rotation, color);
}


GameObject* BaseScene::CreatePlaneGameObject()
{
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	return lve::PlaneGameObject::Create(*device);
}

GameObject* BaseScene::CreateVaseGameObject(const int _type)
{
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	if (_type == 0) return lve::VaseGameObject::CreateFlat(*device);
	if (_type == 1) return lve::VaseGameObject::CreateSmooth(*device);
	return lve::VaseGameObject::CreateFlat(*device);
}

GameObject* BaseScene::CreateGirlGameObject()
{
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	return lve::GirlGameObject::Create(*device);
}

GameObject* BaseScene::CreateNoobGameObject()
{
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	return lve::NoobGameObject::Create(*device);
}

GameObject* BaseScene::CreateSphereGameObject()
{
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	return lve::SphereGameObject::Create(*device);
}

GameObject* BaseScene::CreateMultipleGameObject(int _type)
{
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	switch (_type)
	{
	case 0:
		return lve::MultipleGameObject::CreateCone(*device);
		break;
	case 1:
		return lve::MultipleGameObject::CreateTriangle(*device);
		break;
	case 2:
		return lve::MultipleGameObject::CreateCapsule(*device);
		break;
	case 3:
		return lve::MultipleGameObject::CreateTube(*device);
		break;
	case 4:
		return lve::MultipleGameObject::CreateAnneau(*device);
		break;
	case 5:
		return lve::MultipleGameObject::CreateCylindre(*device);
		break;
	case 6:
		return lve::MultipleGameObject::CreateCylindreCoupe(*device);
		break;
	default:
		return lve::MultipleGameObject::CreateCylindreCoupe(*device);
		break;
	}
}
