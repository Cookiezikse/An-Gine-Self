#include "GameObject/PreGameObject/PlaneGameObject.h"

#include "GameObject/Components/Transform.h"

namespace lve
{
	GameObject* PlaneGameObject::Create(LveDevice& _lveDevice, const glm::vec3 _position,
		const glm::vec3 _scale, const glm::vec3 _rotation)
	{
		const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(_lveDevice, "Models\\quad.obj");

		GameObject* game_object = GameObject::CreatePGameObject();
		game_object->SetName("Plane");
		game_object->SetModel(lve_model);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_scale);
		game_object->GetTransform()->SetRotation(_rotation);
		game_object->SetFileModel("Models\\quad.obj");

		return game_object;
	}
}
