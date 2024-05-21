#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/Components/Light.h"
#include "GameObject/Components/Transform.h"

namespace lve
{
	GameObject* LightGameObject::Create(const float     _intensity, const float          _radius, const glm::vec3 _position,
		const glm::vec3 _rotate, const glm::vec3 _color)
	{
		GameObject* game_object = GameObject::CreatePGameObject();
		game_object->SetName("Light");
		game_object->SetColor(_color);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_radius, game_object->GetTransform()->GetScale().y, game_object->GetTransform()->GetScale().z);
		game_object->GetTransform()->SetRotation(_rotate);
		Light* light_component = game_object->CreateComponent<Light>();
		light_component->lightIntensity = _intensity;
		return game_object;
	}
}
