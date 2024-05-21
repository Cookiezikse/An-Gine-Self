#include "GameObject/GameObject.h"

#include "GameObject/Components/Light.h"
#include "GameObject/Components/Transform.h"
#include "Engine/CoreEngine.h"
#include "Modules/RHIVulkanModule.h"

GameObject::GameObject(): id(0)
{
	this->isActive  = true;
	this->transform = new Transform();
}

GameObject::GameObject(const id_t _id) : id(_id)
{
	this->isActive  = true;
	this->transform = new Transform();
}

GameObject::~GameObject()
{
	std::cout << "deleted game object";
	for (const Component* component : components)
	{
		delete component;
	}
	components.clear();
}

Transform* GameObject::GetTransform() const { return transform; }

glm::vec3 GameObject::GetPosition() const { return transform->GetPosition(); }
void      GameObject::SetPosition(const glm::vec3 _newPosition) const { transform->SetPosition(_newPosition); }

glm::vec3 GameObject::GetScale() const { return transform->GetScale(); }
void      GameObject::SetScale(const glm::vec3 _newScale) const { transform->SetScale(_newScale); }

glm::vec3 GameObject::GetRotation() const { return transform->GetRotation(); }
void      GameObject::SetRotation(const float _newRotation) const { transform->SetRotation(_newRotation); }

void GameObject::AddComponent(Component* _component)
{
	_component->SetOwner(this);
	components.push_back(_component);
}

void GameObject::RemoveComponent(const Component* _component)
{
	const auto it = std::find(components.begin(), components.end(), _component);
	if (it != components.end()) {
		components.erase(it); // Supprimer l'élément du vecteur
		delete _component; // Supprimer l'objet pointé par _component
	}
}

void GameObject::Init()
{
}

void GameObject::Start()
{
	for (Component* const& component : components)
	{
		component->Start();
	}
}

void GameObject::FixedUpdate() const
{
	if (this->isActive)
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) components[i]->FixedUpdate();
		}
}

void GameObject::Update() const
{
	if (this->isActive)
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) components[i]->Update();
		}
}

void GameObject::UpdateEditor() const
{
	if (this->isActive)
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) components[i]->UpdateEditor();
		}
}

void GameObject::PreRender()
{
}

void GameObject::Render() const
{
	if (this->isVisible)
		for (const auto component : components)
		{
			if (component->GetVisible()) component->Render();
		}
}

void GameObject::RenderGui()
{
}

void GameObject::PostRender()
{
}

void GameObject::Release()
{
}

void GameObject::Finalize()
{
}

json GameObject::toJson() const
{
	json j;
	j["id"] = id;
	j["name"] = name;
	j["fileModel"] = fileModel;
	if (parent) j["parent_id"] = parent->GetId();
	if(model) j["model_filename"] = model->GetFilename();
	else j["model_filename"] = "";
	j["texture"] = texture;
	j["color"] = { color.x, color.y,color.z };
	j["isActive"] = isActive;
	j["isVisible"] = isVisible;
	json componentsJson = json::array();
	for (const auto& component : components) {
		componentsJson.push_back(component->toJson());
	}
	j["components"] = componentsJson;
	return j;
}

void GameObject::fromJson(const json& _j)
{
	id = _j["id"];
	name = _j["name"];
	fileModel = _j["fileModel"];
	/*parent = j["parent_id"];*/
	std::string modelname = _j["model_filename"];
	if (!fileModel.empty()) model = lve::LveModel::CreateModelFromFile(*Engine::GetInstance()->GetModuleManager()->GetModule<RHIVulkanModule>()->GetDevice(), fileModel);
	texture = _j["texture"];
	color = glm::vec3(_j["color"][0], _j["color"][1], _j["color"][2]);
	isActive = _j["isActive"];
	isVisible = _j["isVisible"];

	for (const auto& componentJson : _j["components"]) {
		std::string type = componentJson["type"];
		if (type == "Transform") {
			const auto transformComponent = new Transform();
			transformComponent->fromJson(componentJson);
			components.push_back(transformComponent);
		}
		else if (type == "Light")
		{
			const auto light_component = new Light();
			light_component->fromJson(componentJson);
			components.push_back(light_component);
		}
		// Ajoutez des conditions pour d'autres types de composants si nécessaire
	}
}


std::vector<GameObject*> GameObject::FindChildrenByName(const std::string& _name) const
{
	std::vector<GameObject*> found_objects;

	// Parcourir tous les enfants du GameObject
	for (GameObject* child : children)
	{
		// V�rifier si le nom du GameObject correspond au nom recherch�
		if (child->GetName() == _name)
			// Ajouter le GameObject � la liste des objets trouv�s
			found_objects.push_back(child);

		// R�cursivement chercher les enfants du GameObject actuel
		std::vector<GameObject*> nested_found = child->FindChildrenByName(_name);
		found_objects.insert(found_objects.end(), nested_found.begin(), nested_found.end());
	}

	return found_objects;
}


void GameObject::AddChildObject(GameObject* _child)
{
	children.push_back(_child);
	_child->SetParent(this);

	const glm::vec3 parent_position   = this->GetPosition();
	const glm::vec3 child_position    = _child->GetPosition();
	const glm::vec3 relative_position = child_position - parent_position;
	_child->SetPosition(relative_position);
}


Component* GameObject::GetComponentRecursive(const std::string& _componentName) const
{
	// V�rifier si le GameObject a le composant sp�cifi�
	for (Component* component : components)
	{
		if (component->GetName() == _componentName) return component;
	}

	// Recherche r�cursive dans les enfants
	for (GameObject* child : children)
	{
		Component* foundComponent = child->GetComponentRecursive(_componentName);
		if (foundComponent != nullptr) return foundComponent;
	}

	// Recherche r�cursive dans le parent
	if (parent != nullptr) return parent->GetComponentRecursive(_componentName);

	// Le composant n'a pas �t� trouv� dans ce GameObject ou ses enfants
	return nullptr;
}
