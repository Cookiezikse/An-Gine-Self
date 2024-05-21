#include "Engine/EngineTestCode.h"

#include "Modules/DebugModule.h"

void EngineTestCode::TestCode()
{
	//sceneModule.CreateScene("Bongo", false);
	//sceneModule.CreateScene("OLYSHEET", false);
	//std::cout << "SCENE NAME AT INDEX 1 " + sceneModule.GetSceneAt(1).first << std::endl;
	//std::cout << "SCENE ACTIVE NAME : " + sceneModule.GetActiveScene() << std::endl;
	//std::cout << "SCENE COUNT : " + std::to_string(sceneModule.SceneCount()) << std::endl;


	DebugModule::LogInfo("Initialisation terminee avec succes.");
	DebugModule::LogWarning("Attention : Memoire faible.");
	DebugModule::LogError("Echec de l'ouverture du fichier.");
	DebugModule::LogSuccess("Succes de l'ouverture du fichier.");
}
