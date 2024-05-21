#include "GameObject/PreGameObject/VaseGameObject.h"

#include "GameObject/Components/Transform.h"


namespace lve
{
	GameObject* VaseGameObject::CreateFlat(LveDevice& _lveDevice, const glm::vec3 _position,
		const glm::vec3 _scale, const glm::vec3 _rotation)
	{
		const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(_lveDevice, "Models\\flat_vase.obj");

		GameObject* game_object = GameObject::CreatePGameObject();
		game_object->SetModel(lve_model);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_scale);
		game_object->GetTransform()->SetRotation(_rotation);
		game_object->SetFileModel("Models\\flat_vase.obj");

		return game_object;
	}

	GameObject* VaseGameObject::CreateSmooth(LveDevice& _lveDevice, const glm::vec3 _position,
		const glm::vec3 _scale, const glm::vec3 _rotation)
	{
		const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(_lveDevice, "Models\\smooth_vase.obj");

		GameObject* game_object = GameObject::CreatePGameObject();
		game_object->SetModel(lve_model);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_scale);
		game_object->GetTransform()->SetRotation(_rotation);
		game_object->SetFileModel("Models\\smooth_vase.obj");

		return game_object;
	}
}
