#include "ImGUIInterface.h"


// Déclaration des couleurs
ImVec4 ImGUIInterface::mainColor = ImVec4(0.12549, 0.08235, 0.19216, 1.0f);
ImVec4 ImGUIInterface::accentColor = ImVec4(0.29f, 0.27f, 0.33f, 1.0f);
ImVec4 ImGUIInterface::textColor = ImVec4(0.69f, 0.66f, 0.73f, 1.0f);
ImVec4 ImGUIInterface::areaBgColor = ImVec4(0.066f, 0.066f, 0.066f, 1.0f);
ImVec4 ImGUIInterface::secondaryColor = ImVec4(0.121f, 0.165f, 0.333f, 1.0f);

// Réinitialiser les couleurs aux valeurs par défaut d'ImGui
void ResetToDefaults() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);  // Vous pouvez aussi utiliser StyleColorsClassic() ou StyleColorsLight()

    // Réinitialisation des couleurs personnalisées
    ImGUIInterface::mainColor = style.Colors[ImGuiCol_WindowBg];    // Exemple de récupération d'une couleur de base d'ImGui
    ImGUIInterface::accentColor = style.Colors[ImGuiCol_Border];
    ImGUIInterface::textColor = style.Colors[ImGuiCol_Text];
    ImGUIInterface::areaBgColor = ImVec4(0.066f, 0.066f, 0.066f, 0.666f);
    ImGUIInterface::secondaryColor = style.Colors[ImGuiCol_Button];

    ImGUIInterface::ApplyCustomTheme();  // Applique le thème personnalisé après réinitialisation
}

void ImGUIInterface::ApplyCustomTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Configuration des couleurs de base
    style.Colors[ImGuiCol_Text] = textColor; // Couleur du texte
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(textColor.x, textColor.y, textColor.z, 0.58f); // Couleur du texte désactivé

    // Couleurs des cadres et arrière-plans
    style.Colors[ImGuiCol_FrameBg] = mainColor; // Fond des cadres (ex : champs de saisie)
    style.Colors[ImGuiCol_FrameBgHovered] = accentColor; // Fond des cadres au survol
    style.Colors[ImGuiCol_FrameBgActive] = secondaryColor; // Fond des cadres activés

    style.Colors[ImGuiCol_WindowBg] = areaBgColor; // Fond des fenêtres
    style.Colors[ImGuiCol_ChildBg] = areaBgColor; // Fond des fenêtres enfants
    style.Colors[ImGuiCol_PopupBg] = secondaryColor; // Fond des pop-ups

    // Configuration des bordures
    style.Colors[ImGuiCol_Border] = mainColor; // Bordures
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Ombres des bordures (transparentes)

    // Couleurs des boutons
    style.Colors[ImGuiCol_Button] = secondaryColor; // Boutons
    style.Colors[ImGuiCol_ButtonHovered] = accentColor; // Boutons au survol
    style.Colors[ImGuiCol_ButtonActive] = secondaryColor; // Boutons activés

    // Couleurs des en-têtes et séparateurs
    style.Colors[ImGuiCol_Header] = secondaryColor; // En-têtes
    style.Colors[ImGuiCol_HeaderHovered] = accentColor; // En-têtes au survol
    style.Colors[ImGuiCol_HeaderActive] = secondaryColor; // En-têtes activés
    style.Colors[ImGuiCol_Separator] = mainColor; // Séparateurs
    style.Colors[ImGuiCol_SeparatorHovered] = accentColor; // Séparateurs au survol
    style.Colors[ImGuiCol_SeparatorActive] = secondaryColor; // Séparateurs activés

    // Couleurs des barres de titre et menus
    style.Colors[ImGuiCol_TitleBg] = mainColor; // Barres de titre
    style.Colors[ImGuiCol_TitleBgActive] = accentColor; // Barres de titre des fenêtres actives
    style.Colors[ImGuiCol_TitleBgCollapsed] = mainColor; // Barres de titre des fenêtres réduites
    style.Colors[ImGuiCol_MenuBarBg] = secondaryColor; // Arrière-plan des menus

    // Couleurs des barres de défilement
    style.Colors[ImGuiCol_ScrollbarBg] = areaBgColor; // Fond des barres de défilement
    style.Colors[ImGuiCol_ScrollbarGrab] = mainColor; // Poignées des barres de défilement
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = accentColor; // Poignées des barres de défilement au survol
    style.Colors[ImGuiCol_ScrollbarGrabActive] = secondaryColor; // Poignées des barres de défilement activées

    // Configuration des cases à cocher et sliders
    style.Colors[ImGuiCol_CheckMark] = textColor; // Symboles des cases à cocher
    style.Colors[ImGuiCol_SliderGrab] = mainColor; // Curseurs des sliders
    style.Colors[ImGuiCol_SliderGrabActive] = accentColor; // Curseurs des sliders activés

    // Configuration des onglets
    style.Colors[ImGuiCol_Tab] = mainColor; // Onglets
    style.Colors[ImGuiCol_TabHovered] = accentColor; // Onglets au survol
    style.Colors[ImGuiCol_TabActive] = secondaryColor; // Onglets actifs
    style.Colors[ImGuiCol_TabUnfocused] = mainColor; // Onglets non focalisés
    style.Colors[ImGuiCol_TabUnfocusedActive] = secondaryColor; // Onglets actifs non focalisés

    // Couleurs de navigation et panneau de dessin
    style.Colors[ImGuiCol_NavHighlight] = accentColor; // Élément de navigation sélectionné
    style.Colors[ImGuiCol_NavWindowingHighlight] = secondaryColor; // Mise en avant lors du changement de fenêtre
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(areaBgColor.x, areaBgColor.y, areaBgColor.z, 0.8f); // Fond atténué lors du changement de fenêtre

    // Configuration des info-bulles et tableaux
    style.Colors[ImGuiCol_PopupBg] = secondaryColor; // Fond des info-bulles
    style.Colors[ImGuiCol_TableHeaderBg] = mainColor; // Fond des en-têtes de tableau
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.5f); // Bordures fortes des tableaux
}

void ImGUIInterface::EditTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    if (ImGui::TreeNode("Parameters")) {
        // Marges et espacements
        ImGui::Separator();
        ImGui::DragFloat2("Window Padding", (float*)&style.WindowPadding, 0.1f, 0.0f, 20.0f, "%.1f");
        ImGui::DragFloat2("Item Spacing", (float*)&style.ItemSpacing, 0.1f, 0.0f, 20.0f, "%.1f");
        ImGui::DragFloat2("Item Inner Spacing", (float*)&style.ItemInnerSpacing, 0.1f, 0.0f, 20.0f, "%.1f");
        ImGui::DragFloat("Indent Spacing", &style.IndentSpacing, 0.1f, 0.0f, 30.0f, "%.1f");

        // Tailles et rayons
        ImGui::Separator();
        ImGui::DragFloat2("Frame Padding", (float*)&style.FramePadding, 0.1f, 0.0f, 20.0f, "%.1f");

        // Rounding            
        ImGui::Separator();
        ImGui::DragFloat("Window Rounding", &style.WindowRounding, 0.1f, 0.0f, 12.0f, "%.1f");
        ImGui::DragFloat("Child Rounding", &style.ChildRounding, 0.1f, 0.0f, 12.0f, "%.1f");
        ImGui::DragFloat("Popup Rounding", &style.PopupRounding, 0.1f, 0.0f, 12.0f, "%.1f");
        ImGui::DragFloat("Frame Rounding", &style.FrameRounding, 0.1f, 0.0f, 12.0f, "%.1f");
        ImGui::DragFloat("Grab Rounding", &style.GrabRounding, 0.1f, 0.0f, 12.0f, "%.1f");
		ImGui::DragFloat("Tab Rounding", &style.TabRounding, 0.1f, 0.0f, 12.0f, "%.1f");


        // Alpha et transparence
        ImGui::Separator();
        ImGui::DragFloat("Window Min Alpha", &style.Alpha, 0.01f, 0.1f, 1.0f, "%.2f");
        ImGui::DragFloat("Window Title Alignment", &style.WindowTitleAlign.x, 0.01f, 0.0f, 1.0f, "%.2f");

        // Les options avancées
        if (ImGui::TreeNode("Advanced")) {
            // Alpha et transparence
            ImGui::Separator();
            ImGui::DragFloat("Window Min Alpha", &style.Alpha, 0.01f, 0.1f, 1.0f, "%.2f");
            ImGui::DragFloat("Window Title Alignment", &style.WindowTitleAlign.x, 0.01f, 0.0f, 1.0f, "%.2f");

            // Barres de défilement
            ImGui::Separator();
            ImGui::DragFloat("Scrollbar Size", &style.ScrollbarSize, 0.1f, 1.0f, 20.0f, "%.1f");
            ImGui::DragFloat("Scrollbar Rounding", &style.ScrollbarRounding, 0.1f, 0.0f, 12.0f, "%.1f");

            // Bordures
            ImGui::Separator();
            ImGui::DragFloat("Window Border Size", &style.WindowBorderSize, 0.1f, 0.0f, 1.0f, "%.1f");
            ImGui::DragFloat("Child Border Size", &style.ChildBorderSize, 0.1f, 0.0f, 1.0f, "%.1f");
            ImGui::DragFloat("Popup Border Size", &style.PopupBorderSize, 0.1f, 0.0f, 1.0f, "%.1f");
			ImGui::DragFloat("Frame Border Size", &style.FrameBorderSize, 0.1f, 0.0f, 1.0f, "%.1f");
            ImGui::DragFloat("Tab Border Size", &style.TabBorderSize, 0.1f, 0.0f, 1.0f, "%.1f");

            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Colors")) {
        ImGui::ColorEdit4("Main Color", (float*)&mainColor);     
        ImGui::Separator();

        ImGui::ColorEdit4("Accent Color", (float*)&accentColor);
        ImGui::Separator();

        ImGui::ColorEdit4("Text Color", (float*)&textColor);
        ImGui::Separator();

        ImGui::ColorEdit4("Area Background Color", (float*)&areaBgColor);
        ImGui::Separator();

        ImGui::ColorEdit4("Secondary Color", (float*)&secondaryColor);
        ImGui::Separator();

        ImGui::TreePop();
    }
    if (ImGui::Button("Apply Theme")) {
        ApplyCustomTheme();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset to Default")) {
        ResetToDefaults();
    }
}
