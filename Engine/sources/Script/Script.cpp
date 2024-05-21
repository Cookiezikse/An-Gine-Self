#include "Script/Script.h"

#include "Engine/CoreEngine.h"

Script::Script()
{
	timeModuleReference = Engine::GetInstance()->GetModuleManager()->GetModule<TimeModule>();
	windowModuleReference = Engine::GetInstance()->GetModuleManager()->GetModule<WindowModule>();
	renderInterfaceModuleReference = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>();
	inputModuleReference = Engine::GetInstance()->GetModuleManager()->GetModule<InputModule>();
}

TimeModule* Script::GetTimeModule() const
{
	return timeModuleReference;
}

WindowModule* Script::GetWindowModule() const
{
	return windowModuleReference;
}

RHIModule* Script::GetRenderInterfaceModule() const
{
	return renderInterfaceModuleReference;
}


InputModule* Script::GetInputModule() const
{
	return inputModuleReference;
}
