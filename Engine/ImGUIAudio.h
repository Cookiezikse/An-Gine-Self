#pragma once
#include "Modules/SoundSystemModule.h"
#include <imgui.h>

/**
 * @brief Classe pour gérer l'interface audio dans ImGui.
 */
class ImGUIAudio {
public:

    /**
    * @brief Constructeur de la classe ImGUIAudio.
    * @param _soundModule Module du système audio.
    */
    ImGUIAudio(SoundSystemModule* _soundModule);

    /**
    * @brief Fonction pour dessiner les contrôles audio dans l'interface utilisateur.
    */
    void DrawAudioControls();

private:
    SoundSystemModule* _soundModule; ///< Module du système audio.
};
