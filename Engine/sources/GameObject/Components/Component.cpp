#include "GameObject/Components/Component.h"


Component::Component(GameObject* _owner)
{
	this->owner = _owner;
}


void Component::Init()
{
}

void Component::Start()
{
}

void Component::FixedUpdate()
{
}

void Component::Update()
{
}

void Component::UpdateEditor()
{
}

void Component::PreRender()
{
}

void Component::Render()
{
}

void Component::RenderGui()
{
}

void Component::PostRender()
{
}

void Component::Release()
{
}

void Component::Finalize()
{
}
