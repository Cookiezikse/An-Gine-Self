#include "Modules/DebugModule.h"


DebugModule::~DebugModule()
= default;

void DebugModule::Log(const std::string& _message, const TypeLog _level)
{
	std::string prefix_type;
	std::string color_code;
	switch (_level)
	{
	case TypeLog::INFO:
		prefix_type = "[INFO]";
		color_code = "\033[1;34m"; // Bleu
		break;
	case TypeLog::WARNING:
		prefix_type = "[WARNING]";
		color_code = "\033[1;33m"; // Jaune
		break;
	case TypeLog::ERROR:
		prefix_type = "[ERROR]";
		color_code = "\033[1;31m"; // Rouge
		break;
	case TypeLog::SUCCESS:
		prefix_type = "[SUCCESS]";
		color_code = "\033[1;32m"; // Vert
		break;
	default:
		break;
	}

	const std::string reset_color = "\033[0m"; // Reset la couleur
	std::cout << color_code << prefix_type << " : " << _message << reset_color << std::endl;
}

void DebugModule::Init()
{
	Module::Init();
}

void DebugModule::Start()
{
	Module::Start();
}

void DebugModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void DebugModule::Update()
{
	Module::Update();
}

void DebugModule::PreRender()
{
	Module::PreRender();
}

void DebugModule::Render()
{
	Module::Render();
}

void DebugModule::RenderGui()
{
	Module::RenderGui();
}

void DebugModule::PostRender()
{
	Module::PostRender();
}

void DebugModule::Release()
{
	Module::Release();
}

void DebugModule::Finalize()
{
	Module::Finalize();
}
