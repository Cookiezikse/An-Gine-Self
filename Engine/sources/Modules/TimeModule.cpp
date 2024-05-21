#include "Modules/TimeModule.h"

float TimeModule::deltaTime = 0.0f;

void TimeModule::Init()
{
	Module::Init();
	currentTime = std::chrono::high_resolution_clock::now();
	beginTime   = std::chrono::high_resolution_clock::now();
	deltaTime   = 0.0f;
}

void TimeModule::Start()
{
	Module::Start();
}

void TimeModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void TimeModule::Update()
{
	Module::Update();
	const auto                         current_time = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<float> duration     = current_time - currentTime;

	deltaTime   = duration.count();
	currentTime = current_time;

}

void TimeModule::PreRender()
{
	Module::PreRender();
}

void TimeModule::Render()
{
	Module::Render();
}

void TimeModule::RenderGui()
{
	Module::RenderGui();
}

void TimeModule::PostRender()
{
	Module::PostRender();
}

void TimeModule::Release()
{
	Module::Release();
}

void TimeModule::Finalize()
{
	Module::Finalize();
}