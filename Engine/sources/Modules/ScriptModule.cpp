#include "Modules/ScriptModule.h"
#include "Modules/TimeModule.h"
#include "Engine/CoreEngine.h"

bool ScriptModule::StartScripts()
{
    while (!scriptsToStart.empty())
    {
        scriptsToStart.front()->Start();
        scriptsToStart.pop();
    }

    return true;
}

IScript* ScriptModule::GetIScriptByName(const std::string& _name) const
{
    for (IScript* script : scripts)
    {
        if (script->GetScriptName() == _name)
            return script;
    }

    return nullptr;
}


void ScriptModule::UpdateScripts()
{
    StartScripts();


    TimeModule* timeModule = Engine::GetInstance()->GetModuleManager()->GetModule<TimeModule>();
    float deltaTime = timeModule->GetDeltaTime();

    for (IScript* script : scripts)
    {
        script->Update();
        script->Update(deltaTime);
    }
}

void ScriptModule::Stop()
{
    std::queue<IScript*>().swap(scriptsToStart);

    for (IScript* script : scripts)
    {
        script->Destruct();
        delete script;
    }
}
