#include "Modules/ModuleManager.h"

#include "Modules/ImGUIModule.h"
#include "Modules/RHIVulkanModule.h"
#include "Modules/TimeModule.h"
#include "Modules/WindowModule.h"
#include "Scene/SceneManager.h"

ModuleManager::~ModuleManager()
{
	Release();
	modules.clear();
}

void ModuleManager::CreateDefaultModules()
{
	CreateModule<WindowModule>();
	CreateModule<SceneManager>();
	CreateModule<RHIVulkanModule>();
	CreateModule<TimeModule>();
	CreateModule<ImGuiModule>();
	CreateModule<SoundSystemModule>();
}

void ModuleManager::Init() const
{
	for (Module* module : modules)
	{
		module->Init();
	}
}

void ModuleManager::Start() const
{
	for (Module* module : modules)
	{
		module->Start();
	}
}

void ModuleManager::FixedUpdate() const
{
	for (Module* module : modules)
	{
		module->FixedUpdate();
	}
}

void ModuleManager::Update() const
{
	for (Module* module : modules)
	{
		module->Update();
	}
}

void ModuleManager::PreRender() const
{
	for (Module* module : modules)
	{
		module->PreRender();
	}
}

void ModuleManager::Render() const
{
	for (Module* module : modules)
	{
		module->Render();
	}
}

void ModuleManager::RenderGui() const
{
	for (Module* module : modules)
	{
		module->RenderGui();
	}
}

void ModuleManager::PostRender() const
{
	for (Module* module : modules)
	{
		module->PostRender();
	}
}

void ModuleManager::Release() const
{
	for (Module* module : modules)
	{
		module->Release();
	}
}

void ModuleManager::Finalize() const
{
	for (Module* module : modules)
	{
		module->Finalize();
	}
}
