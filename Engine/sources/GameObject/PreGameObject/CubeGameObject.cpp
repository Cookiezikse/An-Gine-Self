#include "GameObject/PreGameObject/CubeGameObject.h"

#include "GameObject/Components/Transform.h"

namespace lve
{
	GameObject* CubeGameObject::Create(LveDevice&      _lveDevice, const glm::vec3 _position,
	                                   const glm::vec3 _scale, const glm::vec3     _rotation)
	{
		const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(_lveDevice, "Models\\cube.obj");

		const auto game_object = GameObject::CreatePGameObject();
		game_object->SetName("Cube");
		game_object->SetModel(lve_model);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_scale);
		game_object->GetTransform()->SetRotation(_rotation);
		game_object->SetFileModel("Models\\cube.obj");

		return game_object;
	}

	GameObject* CubeGameObject::CreateColor(LveDevice&      _lveDevice, const glm::vec3 _position,
	                                        const glm::vec3 _scale, const glm::vec3     _rotation)
	{
		const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(
			_lveDevice, "Models\\colored_cube.obj");

		const auto game_object = GameObject::CreatePGameObject();
		game_object->SetName("CubeColor");
		game_object->SetModel(lve_model);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_scale);
		game_object->GetTransform()->SetRotation(_rotation);
		game_object->SetFileModel("Models\\colored_cube.obj");

		return game_object;
	}
}
