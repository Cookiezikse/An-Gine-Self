#include "ImGUIAudio.h"
#include <iostream>
#include <fmod.hpp>


ImGUIAudio::ImGUIAudio(SoundSystemModule* _soundModule) : _soundModule(_soundModule) {
    if (_soundModule) {
        _soundModule->loadAndPlaySound("Sounds/babooshka.mp3");  // Chemin du fichier audio
    }
}

void ImGUIAudio::DrawAudioControls() {
    if (!_soundModule) {
        ImGui::Text("Erreur: SoundSystemModule n'est pas initialisé.");
        return;
    }

    if (ImGui::Button("Start")) {
        _soundModule->loadAndPlaySound("Sounds/babooshka.mp3");
    }
    ImGui::SameLine();
    if (ImGui::Button(_soundModule->IsPlaying() ? "Pause" : "Play")) {
        _soundModule->TogglePlayPause();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        _soundModule->StopSound();
    }
    float volume = _soundModule->GetMasterVolume();
    if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "Volume %.3f")) {
        _soundModule->SetMasterVolume(volume);
    }
    ImGui::Separator();
    ImGui::Text("Current Track: %s", _soundModule->GetCurrentTrackName().c_str());

    // Bouton pour ouvrir le dialogue de sélection de fichier
    if (ImGui::Button("Load Sound File")) {
        // Ouvrir le dialogue de sélection de fichier
        // ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".mp3,.wav,.ogg");
    }

    // Vérifier si un fichier a été sélectionné et validé
    //if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
    //    if (ImGuiFileDialog::Instance()->IsOk()) {
    //        std::string _filePath = ImGuiFileDialog::Instance()->GetFilePathName();
    //        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
    //        // Vous pouvez ici utiliser _filePath ou fileName selon vos besoins
    //        _soundModule->loadAndPlaySound(_filePath.c_str());
    //    }
    //    // Fermer le dialogue une fois fini
    //    ImGuiFileDialog::Instance()->Close();
    //}
}
