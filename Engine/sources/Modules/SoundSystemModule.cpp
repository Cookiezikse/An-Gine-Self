#include "Modules/SoundSystemModule.h"
#include <fmod_errors.h>
#include <iostream>

SoundSystemModule::SoundSystemModule() : isInitialized(false) {
	FMOD_RESULT result = System_Create(&system);
	if (result != FMOD_OK) {
		std::cerr << "Erreur lors de la cr�ation du syst�me FMOD: " << FMOD_ErrorString(result) << std::endl;
		return;  // Important pour �viter d'ex�cuter les �tapes suivantes si le syst�me n'est pas cr��
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);
	if (driverCount == 0) {
		std::cerr << "FMOD n'a pas trouv� de sortie de son." << std::endl;
		return;
	}

	result = system->init(36, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK) {
		std::cerr << "Erreur lors de l'initialisation de FMOD: " << FMOD_ErrorString(result) << std::endl;
		return;
	}

	std::cout << "SoundSystemModule initialis� avec succ�s." << std::endl;
}

// Permet la cr�ation du son avec son chemin d'acc�s //
void SoundSystemModule::CreateSound(SoundClass* p_sound, const char* p_pathAudio) const
{
	if (const FMOD_RESULT result = system->createSound(p_pathAudio, FMOD_LOOP_OFF, nullptr, p_sound); result != FMOD_OK) std::cout << "le son n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la cr�ation d'un groupe de son. //
void SoundSystemModule::CreateSoundGroup(SoundGroup* p_soundGroup, const char* p_pathAudio) const
{
	if (const FMOD_RESULT result = system->createSoundGroup(p_pathAudio, p_soundGroup);
		result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la r�cup�ration d'un groupe de son. //
void SoundSystemModule::GetMasterSoundGroup(SoundGroup* p_sound) const
{
	if (const FMOD_RESULT result = system->getMasterSoundGroup(p_sound);
		result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet de jou� un son sp�cifique avec ces propres param�tres. //
void SoundSystemModule::PlaySound(const SoundClass p_sound, const bool _isPlay, const int _loopCount,
                                  const float      _volume, Channel*   p_channelPtr) const
{
	FMOD::Channel* channel = nullptr;

	if (!_isPlay)
	{
		p_sound->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		p_sound->setMode(FMOD_LOOP_NORMAL);
		p_sound->setLoopCount(_loopCount);
	}

	system->playSound(p_sound, channelGroup, false, &channel);

	channel->setVolume(_volume);

	// Affecter le canal cr�� au pointeur de pointeur pass� en param�tre
	*p_channelPtr = channel;
}

void SoundSystemModule::StopSound() {
	if (mainChannel) {
		mainChannel->stop();
		mainChannel = nullptr;	// R�initialisez le canal principal apr�s l'arr�t
		isPlaying = false;
	}
}

// Permet de lib�rer la m�moire et de couper le son. //
void SoundSystemModule::ReleaseSound(const SoundClass p_sound)
{
	p_sound->release();
}

// Permet d'obtenir le nombre total de tour sur le son //
int SoundSystemModule::GetLoopCount(const SoundClass p_sound, int* p_loopCount)
{
	return p_sound->getLoopCount(p_loopCount);
}

// Permet la cr�ation d'un channel de groupe. //
void SoundSystemModule::CreateChannelGroup(FMOD::ChannelGroup** pp_channelGroup) const
{
	system->createChannelGroup(nullptr, pp_channelGroup);
}

// Permet d'ajouter un son � un channel de groupe pour pouvoir modifier les param�tres du son du tout les sons du channel. //
void SoundSystemModule::AddSoundToGroup(const SoundClass _pSound, FMOD::ChannelGroup* p_channelGroup) const
{
	system->playSound(_pSound, p_channelGroup, false, nullptr);
}

float SoundSystemModule::GetMasterVolume() const {
	float volume;
	if (mainChannel) {
		mainChannel->getVolume(&volume);
	}
	else {
		volume = 0.0f; // Aucun canal actif, retourne 0 ou un �tat de volume inactif
	}
	return volume;
}

void SoundSystemModule::SetMasterVolume(float _volume) {
	if (mainChannel) {  
		mainChannel->setVolume(_volume);
	}
}

bool SoundSystemModule::IsPlaying() const {
	return isPlaying;
}

void SoundSystemModule::TogglePlayPause() {
	if (mainChannel) {
		bool currentlyPaused;
		mainChannel->getPaused(&currentlyPaused);
		mainChannel->setPaused(!currentlyPaused);
		isPlaying = !currentlyPaused;
	}
}

std::string SoundSystemModule::GetCurrentTrackName() const {
	return currentTrackName;
}

void SoundSystemModule::loadAndPlaySound(const char* _filePath) {
	if (!isPlaying || currentTrackName != _filePath) {  // V�rifier si le son n'est pas d�j� en cours de lecture ou si un nouveau fichier est choisi
		if (mainChannel) {
			mainChannel->stop();  // Arr�tez le son actuel s'il y en a un
		}

		FMOD::Sound* newSound;
		FMOD_RESULT result = system->createSound(_filePath, FMOD_DEFAULT, nullptr, &newSound);
		if (result == FMOD_OK) {
			system->playSound(newSound, nullptr, false, &mainChannel);
			currentSound = newSound;
			currentTrackName = _filePath;
			isPlaying = true;
		}
		else {
			std::cerr << "Failed to load sound: " << FMOD_ErrorString(result) << std::endl;
		}
	}
}

// Impl�mentation des fonctions pour le routing vers les ports et la gestion de la r�verb�ration //
FMOD_RESULT SoundSystemModule::AttachChannelGroupToPort(const FMOD_PORT_TYPE _portType,
                                                        const FMOD_PORT_INDEX _portIndex,
                                                        FMOD::ChannelGroup* p_channelGroup, const bool _passThru) const
{
	return system->attachChannelGroupToPort(_portType, _portIndex, p_channelGroup, _passThru);
}

FMOD_RESULT SoundSystemModule::DetachChannelGroupFromPort(FMOD::ChannelGroup* p_channelGroup) const
{
	return system->detachChannelGroupFromPort(p_channelGroup);
}

FMOD_RESULT SoundSystemModule::SetReverbProperties(const int _instance, const FMOD_REVERB_PROPERTIES* p_prop) const
{
	return system->setReverbProperties(_instance, p_prop);
}

FMOD_RESULT SoundSystemModule::GetReverbProperties(const int _instance, FMOD_REVERB_PROPERTIES* p_prop) const
{
	return system->getReverbProperties(_instance, p_prop);
}

void SoundSystemModule::Init()
{
	if (FMOD::System_Create(&system) != FMOD_OK) {
		std::cerr << "Erreur lors de la cr�ation du syst�me FMOD." << std::endl;
		isInitialized = false;
		return;
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);
	if (driverCount == 0) {
		std::cerr << "FMOD n'a pas trouv� de sortie de son." << std::endl;
		isInitialized = false;
		return;
	}

	if (system->init(36, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) {
		std::cerr << "�chec de l'initialisation de FMOD." << std::endl;
		isInitialized = false;
		return;
	}

	isInitialized = true;

	Module::Init();
}

void SoundSystemModule::Start()
{
	if (!isInitialized) {
		std::cerr << "Tentative de d�marrage d'un module non initialis�." << std::endl;
		return;
	}
	Module::Start();
	std::cout << "SoundSystemModule::Start appel�." << std::endl;
}

void SoundSystemModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void SoundSystemModule::Update()
{
	if (system) {
		system->update();
	}
	Module::Update();
}

void SoundSystemModule::PreRender()
{
	Module::PreRender();
}

void SoundSystemModule::Render()
{
	Module::Render();
}

void SoundSystemModule::RenderGui()
{
	Module::RenderGui();
}

void SoundSystemModule::PostRender()
{
	Module::PostRender();
}

void SoundSystemModule::Release()
{
	Module::Release();
}

void SoundSystemModule::Finalize()
{
	Module::Finalize();
}
