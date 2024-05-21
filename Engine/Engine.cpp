#ifdef ENGINECORE_EXPORTS
/**
 * \brief If ENGINECORE_EXPORTS defined, export the symbol
 */
#define ENGINEDLL_API __declspec(dllexport)
#else
/**
 * \brief If ENGINECORE_EXPORTS is not defined, import the symbol
 */
#define ENGINEDLL_API __declspec(dllimport)
#endif


#include "TCP/Client/TCPClientStart.h"
#include "TCP/Server/TCPServerStart.h"

#include "UDP/Network/ClientUDP/ClientUDPStart.h"
#include "UDP/Network/ServerUDP/ServerUDPStart.h"

#include <cstdlib>
#include <iostream>

#include "Engine/CoreEngine.h"
#include "Engine/EngineTestCode.h"

int main(int argc, char* argv[])
{
	EngineTestCode engine_test_code;
	engine_test_code.TestCode();

	printf("You have entered %d arguments:\n", argc);

	//for (int i = 0; i < argc; i++) {
	//	if (strcmp(argv[i], "clientTCP") == 0)
	//	{
	//		TCPClientStart clientTCP;
	//		clientTCP.TCPClient();
	//	}
	//	if (strcmp(argv[i], "serverTCP") == 0)
	//	{
	//		TCPServerStart serverTCP;
	//		serverTCP.TCPServer();
	//	}
	//	if (strcmp(argv[i], "clientUDP") == 0)
	//	{
	//		ClientUDPStart clientUDP;
	//		clientUDP.ClientStartUDP();
	//	}
	//	if (strcmp(argv[i], "serverUDP") == 0)
	//	{
	//		ServerUDPStart serverUDP;
	//		serverUDP.ServerStartUDP();
	//	}
	//}

	//SoundSystemModule sound;
	//SoundClass soundSample;
	//SoundGroup soundGroup;
	//Channel channel;

	//sound.createSound(&soundSample, "");
	//sound.playSound(soundSample, true, 5, 0.9f, &channel);

	/**
	 * @brief Fonction principale du programme.
	 *
	 * Cette fonction cr�e une instance de l'application FirstApp et lance son ex�cution en appelant la m�thode Run().
	 *
	 * @return EXIT_SUCCESS si l'application s'est termin�e avec succ�s, EXIT_FAILURE sinon.
	 */
	//lve::FirstApp app{};

	const Engine* engine = Engine::GetInstance();

	engine->Init();

	//SceneManager* scene_module = engine->GetModuleManager()->GetModule<SceneManager>();
	//scene_module->SetScene<MenuScene>();

	/*try
	{*/
		engine->Run();
		//app.Run();
	/*}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}*/

	//sound.releaseSound(soundSample);


	return EXIT_SUCCESS;
}
