#pragma once

#include <fmod.hpp>
#include <string>
#include "Modules/Module.h"

/**
 * @brief Classe SoundClass : G�re le syst�me Sound utilisant la biblioth�que FMOD
 */
using SoundClass = FMOD::Sound*;

/**
 * @brief Classe SoundGroup : G�re le syst�me SoundGroup utilisant la biblioth�que FMOD
 */
using SoundGroup = FMOD::SoundGroup*;

/**
 * @brief Classe Channel : G�re les canaux audio utilisant la biblioth�que FMOD
 */
using Channel = FMOD::Channel*;

/**
 * @brief Classe SoundSystemModule : G�re le syst�me audio utilisant la biblioth�que FMOD
 */
class SoundSystemModule final : public Module
{
	public:
		/**
		 * @brief Constructeur par d�faut de la classe SoundSystemModule
		 */
		SoundSystemModule();

		/**
		 * @brief Cr�e un objet Sound � partir d'un fichier audio sp�cifi�
		 * @param p_sound Pointeur vers l'objet Sound cr��
		 * @param p_pathAudio Chemin du fichier audio � charger
		 */
		void CreateSound(SoundClass* p_sound, const char* p_pathAudio) const;

		/**
		 * @brief Cr�e un objet SoundGroup � partir d'un fichier audio sp�cifi�
		 * @param p_soundGroup Pointeur vers l'objet SoundGroup cr��
		 * @param p_pathAudio Chemin du fichier audio � charger
		 */
		void CreateSoundGroup(SoundGroup* p_soundGroup, const char* p_pathAudio) const;

		/**
		 * @brief Obtient le groupe de sons principal
		 * @param p_Sound Pointeur vers le groupe de sons principal
		 */
		void GetMasterSoundGroup(SoundGroup* p_Sound) const;

		/**
		 * @brief Joue un son sp�cifi�
		 * @param p_sound Pointeur vers l'objet Sound � jouer
		 * @param _isPlay Bool�en indiquant si le son doit �tre jou� imm�diatement
		 * @param _loopCount Nombre de fois que le son doit �tre jou� en boucle
		 * @param _volume Volume du son � jouer
		 * @param p_channelPtr Pointeur vers le canal sur lequel le son est jou�
		 */
		void PlaySound(SoundClass p_sound, bool _isPlay, int _loopCount, float _volume, Channel* p_channelPtr) const;

		/**
		 * @brief Lib�re les ressources utilis�es par un son sp�cifi�
		 * @param p_sound Pointeur vers l'objet Sound � lib�rer
		 */
		void ReleaseSound(SoundClass p_sound);

		/**
		 * @brief Cr�e un groupe de canaux pour le syst�me audio
		 * @param pp_channelGroup Pointeur vers le groupe de canaux cr��
		 */
		// ReSharper disable once CppInconsistentNaming
		void CreateChannelGroup(FMOD::ChannelGroup** pp_channelGroup) const;

		/**
		 * @brief Ajoute un son � un groupe de canaux sp�cifi�
		 * @param _pSound Pointeur vers l'objet Sound � ajouter au groupe de canaux
		 * @param p_channelGroup Pointeur vers le groupe de canaux auquel ajouter le son
		 */
		void AddSoundToGroup(SoundClass _pSound, FMOD::ChannelGroup* p_channelGroup) const;

		/**
		 * @brief Obtient le nombre de boucles d'un son sp�cifi�
		 * @param _pSound Pointeur vers l'objet Sound
		 * @param p_loopCount Pointeur vers la variable pour stocker le nombre de boucles
		 * @return Le nombre de boucles du son sp�cifi�
		 */
		static int GetLoopCount(SoundClass _pSound, int* p_loopCount);


		/**
		* @brief Obtient le volume principal du syst�me audio
		* @return Le volume principal du syst�me audio
		*/
		float GetMasterVolume() const;

		/**
		* @brief D�finit le volume principal du syst�me audio
		* @param _volume Volume principal � d�finir
		*/
		void SetMasterVolume(float _volume);

		/**
		* @brief Est ce que le son est en cours de lecture
		*/
		bool IsPlaying() const;

		/*
		* @brief Joue un son sp�cifi� ou met en pause le son actuel
		*/
		void TogglePlayPause();

		/*
		* @brief Arr�te le son actuellement jou�
		*/
		void StopSound();

		/**
		* @brief Charge et joue un fichier audio sp�cifi�
		* @param _filePath Chemin du fichier audio � charger et jouer
		*/
		void loadAndPlaySound(const char* _filePath);

		std::string GetCurrentTrackName() const; // Obtenir le nom de la piste actuelle

		/**
		 * @brief Attache un groupe de canaux � un port sp�cifi� pour le rendu audio
		 * @param _portType Type de port auquel attacher le groupe de canaux
		 * @param _portIndex Index du port auquel attacher le groupe de canaux
		 * @param p_channelGroup Pointeur vers le groupe de canaux � attacher
		 * @param _passThru Indique si le signal doit �tre achemin� directement sans traitement (par d�faut, false)
		 * @return R�sultat de l'op�ration d'attache du groupe de canaux au port
		 */
		FMOD_RESULT AttachChannelGroupToPort(FMOD_PORT_TYPE      _portType, FMOD_PORT_INDEX _portIndex,
		                                     FMOD::ChannelGroup* p_channelGroup, bool       _passThru = false) const;

		/**
		 * @brief D�tache un groupe de canaux d'un port auquel il �tait pr�c�demment attach�
		 * @param p_channelGroup Pointeur vers le groupe de canaux � d�tacher du port
		 * @return R�sultat de l'op�ration de d�tachement du groupe de canaux du port
		 */
		FMOD_RESULT DetachChannelGroupFromPort(FMOD::ChannelGroup* p_channelGroup) const;

		/**
		 * @brief D�finit les propri�t�s de r�verb�ration pour une instance de r�verb�ration sp�cifi�e
		 * @param _instance Instance de r�verb�ration pour laquelle d�finir les propri�t�s
		 * @param p_prop Pointeur vers les propri�t�s de r�verb�ration � d�finir
		 * @return R�sultat de l'op�ration de d�finition des propri�t�s de r�verb�ration
		 */
		FMOD_RESULT SetReverbProperties(int _instance, const FMOD_REVERB_PROPERTIES* p_prop) const;

		/**
		 * @brief Obtient les propri�t�s de r�verb�ration pour une instance de r�verb�ration sp�cifi�e
		 * @param _instance Instance de r�verb�ration pour laquelle obtenir les propri�t�s
		 * @param p_prop Pointeur vers les propri�t�s de r�verb�ration � remplir avec les valeurs obtenues
		 * @return R�sultat de l'op�ration de r�cup�ration des propri�t�s de r�verb�ration
		 */
		FMOD_RESULT GetReverbProperties(int _instance, FMOD_REVERB_PROPERTIES* p_prop) const;

#pragma region Event

		/**
		* @brief Initialise le module.
		*/
		void Init() override;

		/**
		* @brief V�rifie si le module est initialis�.
		* @return Vrai si le module est initialis�, sinon faux.
		*/
		bool IsInitialized() const { return isInitialized; }

		/**
		 * @brief D�marre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise � jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met � jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pr�-rendu du module.
		 */
		void PreRender() override;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override;

		/**
		 * @brief Lib�re les ressources utilis�es par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;

#pragma endregion

	private:
		FMOD::System*       system;       // Pointeur vers l'objet syst�me FMOD
		FMOD::ChannelGroup* channelGroup; // Pointeur vers le groupe de canaux utilis�
		FMOD::Channel* mainChannel; // Canal principal
		FMOD::Channel* currentChannel = nullptr; // Canal actuel
		SoundClass currentSound = nullptr; // Actuellement son jou�
		float masterVolume; // Volume principal
		bool isPlaying = false; // Indique si un son est actuellement jou�
		std::string currentTrackName; // Nom de la piste actuelle
		bool isInitialized; // Indique si le module est initialis�

		/**< Distance minimale d'un son 3D. */
		float min;

		/**< Distance maximale d'un son 3D. */
		float max;
};
